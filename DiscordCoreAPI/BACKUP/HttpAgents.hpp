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
			inline static bool doWeQuit = false;

			explicit TimedRequestSender(Scheduler* scheduler, ISource<WorkloadData>& source00, ITarget<WorkloadData>& target00, ISource<HTTPData>& source01, ITarget<HTTPData>& target01, ISource<int>& source0, timer<int>& timer)
				:
				agent(*scheduler),
				source00(source00),
				target00(target00),
				source01(source01),
				target01(target01),
				_source0(source0),
				_timer(timer)
			{};

			~TimedRequestSender() {};

		protected:
			ISource<WorkloadData>& source00;
			ITarget<WorkloadData>& target00;
			ISource<HTTPData>& source01;
			ISource<int>& _source0;
			timer<int>& _timer;
			HttpAgents::WorkloadData workloadData;

			void run() {
				WorkloadData workload;
				receive(&_source0);
				receive(&source00);
				send(&target00, workload);
				HTTPData httpData = receive(&source01);
				send(&target01, httpData);
				done();
			};
		};

		struct RequestSender :  public concurrency::agent,  implements<RequestSender, winrt::Windows::Foundation::IInspectable> {
		public:
			ITarget<HTTPData>& target01;

			inline static bool doWeQuit = false;

			explicit RequestSender(Scheduler* scheduler, ISource<WorkloadData>& source00, ITarget<WorkloadData>& target00, ISource<HTTPData>& source01, ITarget<HTTPData>& target01)
				:
				agent(*scheduler),
				source00(source00),
				target00(target00),
				source01(source01),
				target01(target01)
			{};

			bool getError(exception& e) {
				return try_receive(_error, e);
			}

			~RequestSender() {};

		protected:
			ISource<WorkloadData>& source00;
			ISource<HTTPData>& source01;
			ITarget<WorkloadData>& target00;
			HttpAgents::WorkloadData workloadData;
			single_assignment<exception> _error;

			void run() {
				try {
					WorkloadData workload = receive(&source00);
					send(&target00, workload);
					HTTPData httpData = receive(&source01);
					send(&target01, httpData);
				}
				catch (exception e) {
					send(_error, e);
				}
				done();
			}
			
		};

		struct HTTPHandler :public concurrency::agent,  implements<HTTPHandler, winrt::Windows::Foundation::IInspectable> {
		public:
			ITarget<HTTPData>& _target;
			ISource<WorkloadData>& _source;
			static map<string, RateLimitData> rateLimitData;
			static map<RateLimitType, string> rateLimitDataBucketValues;

			inline static bool doWeQuit = false;

			HTTPHandler(Scheduler* scheduler, ITarget<HTTPData>& target, ISource<WorkloadData>& source)
				: _target(target),
				_source(source),
				agent(*scheduler)
			{};

			bool getError(exception& e) {
				return try_receive(_error, e);
			}

			void run() {
				transformer<WorkloadData, WorkloadData> collectTimeLimitData([this](WorkloadData workload) -> WorkloadData {
					try {
						if (HTTPHandler::rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
							workload.rateLimitData.bucket = HTTPHandler::rateLimitDataBucketValues.at(workload.rateLimitData.rateLimitType);
								workload.rateLimitData = HTTPHandler::rateLimitData.at(workload.rateLimitData.bucket);
							if (HTTPHandler::rateLimitData.contains(workload.rateLimitData.bucket)) {
							}
						}
						return workload;
					}
					catch (exception e) {
						send(_error, e);
					}
					});
				transformer<WorkloadData, HTTPData> collectHTTPData([this](WorkloadData workload)-> HTTPData {
					try {

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
							getData = RestAPI::httpGETObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
							returnData.data = getData.data;
						}
						else if (workload.workloadType == WorkloadType::POST) {
							HTTPData postData;
							postData = RestAPI::httpPOSTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
							returnData.data = postData.data;
						}
						else if (workload.workloadType == WorkloadType::PUT) {
							HTTPData putData;
							putData = RestAPI::httpPUTObjectDataAsync(workload.relativeURL, workload.content, &workload.rateLimitData).get();
							returnData.data = putData.data;
						}
						else if (workload.workloadType == WorkloadType::DELETED) {
							HTTPData deleteData;
							cout << "WE'RE HERE WE'RE HERE WE'RE HERE!" << endl;
							deleteData = RestAPI::httpDELETEObjectDataAsync(workload.relativeURL, &workload.rateLimitData).get();
							returnData.data = deleteData.data;
						}
						if (HTTPHandler::rateLimitDataBucketValues.contains(workload.rateLimitData.rateLimitType)) {
							cout << "WE'RE HERE 2222 WE'RE HERE 2222 WE'RE HERE!22222" << endl;
							HTTPHandler::rateLimitDataBucketValues.erase(workload.rateLimitData.rateLimitType);
						}
						HTTPHandler::rateLimitDataBucketValues.insert(std::make_pair(workload.rateLimitData.rateLimitType, workload.rateLimitData.bucket));
						cout << "WE'RE HERE 33333 WE'RE HERE 33333 WE'RE HERE!3333" << endl;
						if (HTTPHandler::rateLimitData.contains(workload.rateLimitData.bucket)) {
							HTTPHandler::rateLimitData.erase(workload.rateLimitData.bucket);
						}
						cout << "WE'RE HERE 44444 WE'RE HERE 44444 WE'RE HERE!4444" << endl;
						HTTPHandler::rateLimitData.insert(std::make_pair(workload.rateLimitData.bucket, workload.rateLimitData));
						cout << "WE'RE HERE 55555 WE'RE HERE 5555 WE'RE HERE!5555" << endl;
						return returnData;
					}
					catch (exception e) {
						send(_error, e);
					}

					});
				collectTimeLimitData.link_target(&collectHTTPData);
				collectHTTPData.link_target(&this->_target);
				WorkloadData workload = receive(&_source);
				send(&collectTimeLimitData, workload);
				done();
			};

			~HTTPHandler() {};

		protected:
			single_assignment<exception> _error;
		};
		map<RateLimitType, string> HTTPHandler::rateLimitDataBucketValues;
		map<string, RateLimitData> HTTPHandler::rateLimitData;
	};
};
#endif
