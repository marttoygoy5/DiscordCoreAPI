// HttpAgents.hpp - Header for the "http agents" classes.
// May 6, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_AGENTS_
#define _HTTP_AGENTS_

#include "pch.h"
#include "SystemThreads.hpp"
#include "FoundationClasses.hpp"
#include "RestAPI.hpp"

namespace CommanderNS {

	namespace HttpAgents {

		enum class RateLimitType {
			MESSAGE_DELETE = 0,
			MESSAGE_CREATE = 1,
			MESSAGE_GET = 2,
			REACTION_ADD_REMOVE = 3,
			GUILD_GET = 4,
			CHANNEL_GET = 5
		};

		enum class WorkloadType {
			GET = 0,
			PUT = 1,
			POST = 2,
			DELETED = 3
		};

		struct WorkloadData {
			string relativeURL;
			WorkloadType workloadType;
			RateLimitType rateLimitType;
			FoundationClasses::RateLimitation rateLimitData;
		};

		class DataManager : concurrency::agent {
		public:
			explicit DataManager(transformer<WorkloadData, WorkloadData>& source, transformer<WorkloadData, WorkloadData>& target, com_ptr<RestAPI> pRestAPI)
				:	_source(source),
					_target(target)
			{
				this->pRestAPI = pRestAPI;
			}

		protected:
			com_ptr<RestAPI> pRestAPI;

			void run(WorkloadData workloadData)
			{
				// Read the request.
				WorkloadData request = receive(_source);

				stringstream ss;
				ss << "DataManager: Relative URL: " << request.relativeURL << "'." << endl;
				cout << ss.str();

				if (request.workloadType == WorkloadType::GET) {
					httpGETData getData;
					this->pRestAPI->httpGETObjectDataAsync(request.relativeURL, &getData);
				}

				// Send the response.
				ss = stringstream();
				ss << "DataManager: Sending Response: " << endl;
				cout << ss.str();

				send(_target, );

				// Move the agent to the finished state.
				done();
			}

		private:
			transformer<WorkloadData, WorkloadData>& _source;
			transformer<WorkloadData, WorkloadData>& _target;

		};

		class DataReceiver : concurrency::agent {
		public:
			explicit DataReceiver(ISource<string>& source, ITarget<WorkloadData>& target) {
			}

		protected:


		};

	};


}
#endif