// HttpAgents.hpp - Header for the "http agents" classes.
// May 6, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_AGENTS_
#define _HTTP_AGENTS_

#include "pch.h"
#include "FoundationClasses.hpp"
#include "FoundationClasses.hpp"
#include "SystemThreads.hpp"
#include "RestAPI.hpp"

namespace CommanderNS {

	namespace HttpAgents {

		enum class WorkloadType {
			GET = 0,
			PUT = 1,
			POST = 2,
			DELETED = 3
		};

		struct HTTPData {
			json data;
		};

		struct WorkloadData {
			string relativeURL;
			WorkloadType workloadType;
			FoundationClasses::RateLimitData rateLimitData;
			ITarget<HTTPData>* outputTarget;
		};

		class RequestSender : concurrency::agent {
		public:
			explicit RequestSender(ITarget<WorkloadData>& target, ISource<HTTPData>& source, Scheduler& scheduler) 
				:
				agent(scheduler),
				_target(target),
				_source(source)
			{}

		protected:
			void run(WorkloadData workloadData) {
				
				workloadData.outputTarget = (ITarget<CommanderNS::HttpAgents::HTTPData>*)this;
				send(_target, workloadData);
			};

			ITarget<WorkloadData>& _target;
			ISource<HTTPData>& _source;
		};

		class HTTPHandler : concurrency::agent {
		public:
			explicit HTTPHandler(Scheduler& scheduler)
				:agent(scheduler)
			{
			}

			~HTTPHandler(){
				delete this->_source;
				delete this->_target;
			}
			unbounded_buffer<HTTPData>* _target = new unbounded_buffer<HTTPData>();
			unbounded_buffer<WorkloadData>* _source = new unbounded_buffer<WorkloadData>();
		protected:

			void run() {
				// Receive a workload.
				WorkloadData workload = receive(this->_source);

				transformer<WorkloadData, WorkloadData> collectTimeLimitData([](WorkloadData workloadData) -> WorkloadData
					{
						if (workloadData.rateLimitData.rateLimitType == FoundationClasses::RateLimitType::CHANNEL_GET){
							workloadData.rateLimitData = ClientClasses::ChannelManager::channelGetRateLimit;
						}
						else if (workloadData.rateLimitData.rateLimitType == FoundationClasses::RateLimitType::GUILD_GET) {
							workloadData.rateLimitData = ClientClasses::GuildManager::guildGetRateLimit;
						}
						else if (workloadData.rateLimitData.rateLimitType == FoundationClasses::RateLimitType::MESSAGE_CREATE) {
							workloadData.rateLimitData = ClientClasses::MessageManager::messageCreateRateLimit;
						}
						else if (workloadData.rateLimitData.rateLimitType == FoundationClasses::RateLimitType::MESSAGE_DELETE) {
							workloadData.rateLimitData = ClientClasses::MessageManager::messageDeleteRateLimit;
						}
						else if (workloadData.rateLimitData.rateLimitType == FoundationClasses::RateLimitType::MESSAGE_GET) {
							workloadData.rateLimitData = ClientClasses::MessageManager::messageGetRateLimit;
						}
						return workloadData;
					});
				transformer<WorkloadData, WorkloadData> collectHTTPData([](WorkloadData workloadData) -> WorkloadData{

					return workloadData;
				});
			}
		};

	};


}
#endif