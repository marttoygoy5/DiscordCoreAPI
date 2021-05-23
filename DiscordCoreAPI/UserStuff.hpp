// UserStuff.hpp - Header for user-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _USER_STUFF_
#define _USER_STUFF_

#include "pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {
	
	class UserManager;

	class DiscordCoreClient;

	class User {
	public:

		DiscordCoreInternal::UserData data;
		DiscordCoreClient* clientCore{ nullptr };

		User() {};

	protected:
		friend class DiscordCoreClient;
		friend class UserManagerAgent;
		friend class UserManager;
		friend class Guild;
		friend class GuildManager;

		User(DiscordCoreInternal::UserData dataNew,   DiscordCoreClient* clientCoreNew) {
			this->initialize(dataNew, clientCoreNew).get();
		}

		task<void> initialize(DiscordCoreInternal::UserData dataNew, DiscordCoreClient* clientCoreNew) {
			this->data = dataNew;
			this->clientCore = clientCoreNew;
			co_return;
		}

	};

	struct GetUserData {
		string userId;
	};

	struct FetchUserData {
		string userId;
	};

	class UserManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class UserManager;
		static unbounded_buffer<DiscordCoreInternal::FetchUserData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestGetBuffer;
		static unbounded_buffer<User>* outBuffer;
		static concurrent_queue<User> usersToInsert;
		static overwrite_buffer<map<string, User>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* clientCore{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* clientCoreNew,  Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->clientCore = clientCoreNew;
		}

		static task<void> initialize() {
			UserManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::FetchUserData>;
			UserManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetUserData>;
			UserManagerAgent::outBuffer = new unbounded_buffer<User>;
			co_return;
		}

		task<User> getObjectAsync(DiscordCoreInternal::FetchUserData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			if (dataPackage.userType == DiscordCoreInternal::GetUserDataType::USER) {
				workload.relativePath = "/users/" + dataPackage.userId;
			}
			else if (dataPackage.userType == DiscordCoreInternal::GetUserDataType::SELF) {
				workload.relativePath = "/users/@me";
			}			
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			DiscordCoreInternal::UserData userData;
			DiscordCoreInternal::parseObject(jsonValue, &userData);
			User userNew(userData, this->clientCore);
			co_return userNew;
		}

		void run() {
			DiscordCoreInternal::GetUserData getData;
			if (try_receive(UserManagerAgent::requestGetBuffer, getData)) {
				map<string, User> cacheTemp;
				if (try_receive(UserManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.userId)) {
						User user = cacheTemp.at(getData.userId);
						send(UserManagerAgent::outBuffer, user);
					}
				}
			}
			DiscordCoreInternal::FetchUserData fetchData;
			if (try_receive(UserManagerAgent::requestFetchBuffer, fetchData)) {
				map<string, User> cacheTemp;
				if (try_receive(UserManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(fetchData.userId)) {
						cacheTemp.erase(fetchData.userId);
					}
				}
				User user = getObjectAsync(fetchData).get();
				cacheTemp.insert(make_pair(fetchData.userId, user));
				send(UserManagerAgent::outBuffer, user);
				asend(UserManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->clientCore);
			while (UserManagerAgent::usersToInsert.try_pop(user)) {
				map<string, User> cacheTemp;
				if (try_receive(UserManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(user.data.id)) {
						cacheTemp.erase(user.data.id);
					}
				}
				cacheTemp.insert(make_pair(user.data.id, user));
				asend(UserManagerAgent::cache, cacheTemp);
			}
			done();
		}
	};

	class UserManager {
	public:

		task<User> fetchAsync(FetchUserData fetchUserData) {
			DiscordCoreInternal::FetchUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = fetchUserData.userId;
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->clientCore, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this->clientCore);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<User> fetchCurrentUserAsync() {
			DiscordCoreInternal::FetchUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->clientCore, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this->clientCore);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<User> getUserAsync(GetUserData getUserData) {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getUserData.userId;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->clientCore, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this->clientCore);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<void> insertUserAsync(User user) {
			UserManagerAgent userManagerAgent(this->agentResources, this->threads, this->clientCore, this->threads->at(2).scheduler);
			userManagerAgent.start();
			UserManagerAgent::usersToInsert.push(user);
			userManagerAgent.wait(&userManagerAgent);
			co_return;
		}

	protected:

		friend class DiscordCoreClient;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* clientCore{ nullptr };

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* clientCoreNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->clientCore = clientCoreNew;
		}

	};
	overwrite_buffer<map<string, User>> UserManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchUserData>* UserManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestGetBuffer;
	unbounded_buffer<User>* UserManagerAgent::outBuffer;
	concurrent_queue<User> UserManagerAgent::usersToInsert;
	
}
#endif