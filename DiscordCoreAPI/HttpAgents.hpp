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

		struct WorkloadData {
			WorkloadType workloadType = WorkloadType::UNSET;
			string relativeURL;
			RateLimitData rateLimitData;
			string content;
		};

		struct TimedRequestSender : public concurrency::agent, implements<TimedRequestSender, winrt::Windows::Foundation::IInspectable> {
		public:
			ITarget<HTTPData>& target01;

			explicit TimedRequestSender(Scheduler* scheduler, ISource<WorkloadData>& source00, ITarget<WorkloadData>& target00, ISource<HTTPData>& source01, ITarget<HTTPData>& target01, unsigned int timeDelay)
				:
				agent(*scheduler),
				source00(source00),
				target00(target00),
				source01(source01),
				target01(target01),
				_source0(buffer00),
				_target0(&buffer00),
				timer(timeDelay, this->propValue, this->_target0, false)
			{};

			~TimedRequestSender() {};

		protected:
			unbounded_buffer<int> buffer00;
			ISource<WorkloadData>& source00;
			ISource<HTTPData>& source01;
			ITarget<WorkloadData>& target00;
			int propValue;
			ISource<int>& _source0;
			ITarget<int>* _target0;
			timer<int> timer;

			void run() {
				this->timer.start();
				receive(&_source0);
				WorkloadData workload = receive(&source00);
				send(&target00, workload);
				HTTPData httpData = receive(&source01);
				send(&target01, httpData);
				agent::wait(this);
				done();
			};
		};

		struct RequestSender :  public concurrency::agent,  implements<RequestSender, winrt::Windows::Foundation::IInspectable> {
		public:
			ITarget<HTTPData>& target01;

			explicit RequestSender(Scheduler* scheduler, ISource<WorkloadData>& source00, ITarget<WorkloadData>& target00, ISource<HTTPData>& source01, ITarget<HTTPData>& target01)
				:
				agent(*scheduler),
				source00(source00),
				target00(target00),
				source01(source01),
				target01(target01)
			{};

		protected:
			ISource<WorkloadData>& source00;
			ISource<HTTPData>& source01;
			ITarget<WorkloadData>& target00;

			void run() {
				WorkloadData workload = receive(&source00);
				send(&target00, workload);
				HTTPData httpData = receive(&source01);
				send(&target01, httpData);
				done();
			}
		};

		struct HTTPHandler :public concurrency::agent,  implements<HTTPHandler, winrt::Windows::Foundation::IInspectable> {
		public:
			ITarget<HTTPData>& _target;
			ISource<WorkloadData>& _source;
			static map<string, RateLimitData> rateLimitData;
			static map<RateLimitType, string> rateLimitDataBucketValues;

			HTTPHandler(Scheduler* scheduler, ITarget<HTTPData>& target, ISource<WorkloadData>& source, com_ptr<RestAPI> pRestAPI)
				: _target(target),
				_source(source),
				agent(*scheduler)
			{
				this->pRestAPI = pRestAPI;
			};

			void run() {
				transformer<WorkloadData, WorkloadData> collectTimeLimitData([this](WorkloadData workload) -> WorkloadData {
					if (HTTPHandler::rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
						workload.rateLimitData.bucket = HTTPHandler::rateLimitDataBucketValues.at(workload.rateLimitData.rateLimitType);
						workload.rateLimitData = HTTPHandler::rateLimitData.at(workload.rateLimitData.bucket);
					}
					return workload;
					});
				transformer<WorkloadData, HTTPData> collectHTTPData([this](WorkloadData workload)-> HTTPData {
					if (workload.rateLimitData.getsRemaining == 0) {
						float loopStartTime = workload.rateLimitData.timeStartedAt;
						float targetTime = loopStartTime + static_cast<float> (workload.rateLimitData.msRemain);
						float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						while (workload.rateLimitData.msRemain > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							workload.rateLimitData.msRemain = targetTime - currentTime;
						}
					}
					HTTPData returnData;
					if (workload.workloadType == WorkloadType::GET) {
						HTTPData getData;
						getData = this->pRestAPI->httpGETObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
						returnData.data = getData.data;
					}
					else if (workload.workloadType == WorkloadType::POST) {
						HTTPData postData;
						postData = this->pRestAPI->httpPOSTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
						returnData.data = postData.data;
					}
					else if (workload.workloadType == WorkloadType::PUT) {
						HTTPData putData;
						putData = this->pRestAPI->httpPUTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
						returnData.data = putData.data;
					}
					else if (workload.workloadType == WorkloadType::DELETED) {
						HTTPData deleteData;
						deleteData = this->pRestAPI->httpDELETEObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
						returnData.data = deleteData.data;
					}
					if (HTTPHandler::rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
						HTTPHandler::rateLimitDataBucketValues.erase(workload.rateLimitData.rateLimitType);
					}
					HTTPHandler::rateLimitDataBucketValues.insert(std::make_pair(workload.rateLimitData.rateLimitType, workload.rateLimitData.bucket));
					if (HTTPHandler::rateLimitData.contains(workload.rateLimitData.bucket)) {
						HTTPHandler::rateLimitData.erase(workload.rateLimitData.bucket);
					}
					HTTPHandler::rateLimitData.insert(std::make_pair(workload.rateLimitData.bucket, workload.rateLimitData));
					return returnData;

					});
				collectTimeLimitData.link_target(&collectHTTPData);
				collectHTTPData.link_target(&this->_target);
				WorkloadData workload = receive(&_source);
				send(&collectTimeLimitData, workload);
				done();
			};

		protected:
			com_ptr<RestAPI> pRestAPI;
		};
		map<RateLimitType, string> HTTPHandler::rateLimitDataBucketValues;
		map<string, RateLimitData> HTTPHandler::rateLimitData;
	};
};
#endif
