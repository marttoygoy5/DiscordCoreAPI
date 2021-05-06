// HttpAgents.hpp - Header for the "http agents" classes.
// May 6, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_AGENTS_
#define _HTTP_AGENTS_

#include "pch.h"
#include "FoundationClasses.hpp"
#include "SystemThreads.hpp"
#include "RestAPI.hpp"

namespace CommanderNS {

	namespace HttpAgents {

		enum class WorkloadType {
			UNSET = 0,
			GET = 1,
			PUT = 2,
			POST = 3,
			DELETED = 4
		};

		struct HTTPData {
			json data;
		};

		struct WorkloadData {
			string relativeURL;
			WorkloadType workloadType = WorkloadType::UNSET;
			FoundationClasses::RateLimitData rateLimitData;
			ITarget<HTTPData>* outputTarget;
			string content;
		};

		class RequestSender : public concurrency::agent {
		public:
			explicit RequestSender(ITarget<WorkloadData>& target, ISource<HTTPData>& source, Scheduler* scheduler) 
				:
				agent(*scheduler),
				_target(target),
				_source(source)
			{}

			void sendWorkload(WorkloadData workload) {
				workload.outputTarget = (ITarget<CommanderNS::HttpAgents::HTTPData>*)this;
				send(_target, workload);
			}

			json getData() {
				return receive(_source).data;
			}

		protected:
			ITarget<WorkloadData>& _target;
			ISource<HTTPData>& _source;

			void run() {
				return;
			};
		};

		class HTTPHandler : public concurrency::agent {
		public:
			explicit HTTPHandler(Scheduler* scheduler, com_ptr<RestAPI> pRestAPI)
				:agent(*scheduler)
			{
				this->pRestAPI = pRestAPI;
			}

			void run() {
				cout << "CHECKING CHECKING 00000000000000000000000000000!" << endl;
				transformer<WorkloadData, WorkloadData> collectTimeLimitData([this](WorkloadData workload) -> WorkloadData {
					if (this->rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
						workload.rateLimitData.bucket = this->rateLimitDataBucketValues.at(workload.rateLimitData.rateLimitType);
						workload.rateLimitData = this->rateLimitData.at(workload.rateLimitData.bucket);
					}
					return workload;
					});
				cout << "CHECKING CHECKING CHECKING CHECKING CHECKING CHECKING!" << endl;
				transformer<WorkloadData, HTTPData> collectHTTPData([this](WorkloadData workload)-> HTTPData {
					HTTPData returnData;
					float targetTime = static_cast<float>(workload.rateLimitData.timeStartedAt) + static_cast<float> (workload.rateLimitData.msRemain);
					float currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = targetTime - currentTime;
					}

					if (workload.workloadType == WorkloadType::GET) {
						httpGETData getData;
						getData = this->pRestAPI->httpGETObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
						returnData.data = getData.data;
					}
					else if (workload.workloadType == WorkloadType::POST) {
						httpPOSTData postData;
						postData = this->pRestAPI->httpPOSTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
						returnData.data = postData.data;
					}
					else if (workload.workloadType == WorkloadType::PUT) {
						httpPUTData putData;
						putData = this->pRestAPI->httpPUTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
						returnData.data = putData.data;
					}
					else if (workload.workloadType == WorkloadType::DELETED) {
						httpDELETEData deleteData;
						deleteData = this->pRestAPI->httpDELETEObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
						returnData.data = deleteData.data;
					}

					this->rateLimitDataBucketValues.insert(std::make_pair(workload.rateLimitData.rateLimitType, workload.rateLimitData.bucket));
					this->rateLimitData.insert(std::make_pair(workload.rateLimitData.bucket, workload.rateLimitData));

					return returnData;
					});

				this->_source.link_target(&collectTimeLimitData);
				collectTimeLimitData.link_target(&collectHTTPData);
				collectHTTPData.link_target(&this->_target);

				WorkloadData workload = receive(this->_source);
			};

			unbounded_buffer<HTTPData> _target = unbounded_buffer<HTTPData>();
			unbounded_buffer<WorkloadData> _source = unbounded_buffer<WorkloadData>();
		protected:
			com_ptr<RestAPI> pRestAPI;
			map<FoundationClasses::RateLimitType, string> rateLimitDataBucketValues;
			map<string, FoundationClasses::RateLimitData> rateLimitData;

		};

	};
};
#endif
