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
			WorkloadType workloadType = WorkloadType::UNSET;
			string relativeURL;
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

			void setWorkloadData(WorkloadData pWorkload) {
				this->workloadData = pWorkload;
			}

			json getData() {
				return receive(_source).data;
			}

		protected:
			ISource<HTTPData>& _source;
			ITarget<WorkloadData>& _target;
			HttpAgents::WorkloadData workloadData;

			void run() {
				send(_target, this->workloadData);
				done();
			};
		};

		class HTTPHandler :public concurrency::agent {
		public:
			explicit HTTPHandler(Scheduler* scheduler, com_ptr<RestAPI> pRestAPI, ITarget<HTTPData>& target, ISource<WorkloadData>& source)
				: _target(target),
				_source(source),
				agent(*scheduler)
			{
				this->pRestAPI = pRestAPI;
			}

			void run() {
				transformer<WorkloadData, WorkloadData> collectTimeLimitData([this](WorkloadData workload) -> WorkloadData {
					if (HTTPHandler::rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
						workload.rateLimitData.bucket = HTTPHandler::rateLimitDataBucketValues.at(workload.rateLimitData.rateLimitType);
						workload.rateLimitData = HTTPHandler::rateLimitData.at(workload.rateLimitData.bucket);
					}
					return workload;
					});
				transformer<WorkloadData, HTTPData> collectHTTPData([this](WorkloadData workload)-> HTTPData {
					HTTPData returnData;
					float targetTime = static_cast<float>(workload.rateLimitData.timeStartedAt) + static_cast<float> (workload.rateLimitData.msRemain);
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (workload.rateLimitData.getsRemaining == 0) {
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = targetTime - currentTime;
						}
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

					HTTPHandler::rateLimitDataBucketValues.insert(std::make_pair(workload.rateLimitData.rateLimitType, workload.rateLimitData.bucket));
					HTTPHandler::rateLimitData.insert(std::make_pair(workload.rateLimitData.bucket, workload.rateLimitData));
					cout << "CURRENT WORKLOAD 22222: " << workload.content << endl;
					cout << "GETS REMAINING: " << workload.rateLimitData.getsRemaining << endl;
					cout << "MS REMAINING: " << workload.rateLimitData.msRemain << endl;
					cout << "BUCKET: " << workload.rateLimitData.bucket << endl;
					return returnData;
					});
				WorkloadData workload = receive(&_source);
				send(&collectTimeLimitData, workload);
				collectTimeLimitData.link_target(&collectHTTPData);
				collectHTTPData.link_target(&this->_target);
				done();
			};

			ITarget<HTTPData>& _target;
			ISource<WorkloadData>& _source;
		protected:
			com_ptr<RestAPI> pRestAPI;
			static map<FoundationClasses::RateLimitType, string> rateLimitDataBucketValues;
			static map<string, FoundationClasses::RateLimitData> rateLimitData;
			
		};
		map<FoundationClasses::RateLimitType, string> HTTPHandler::rateLimitDataBucketValues;
		map<string, FoundationClasses::RateLimitData> HTTPHandler::rateLimitData;
	};
};
#endif
