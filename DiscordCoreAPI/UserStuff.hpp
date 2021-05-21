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

	class User {
	public:

		DiscordCoreInternal::UserData data;
		UserManager* users{ nullptr };

	protected:
		friend class DiscordCoreClient;
		friend class UserManagerAgent;
		friend class UserManager;
		friend class Guild;
		friend class GuildManager;

		User(DiscordCoreInternal::UserData dataNew,  UserManager* usersNew) {
			this->initialize(dataNew, usersNew).get();
		}

		task<void> initialize(DiscordCoreInternal::UserData dataNew, UserManager* usersNew) {
			this->data = dataNew;
			this->users = usersNew;
			co_return;
		}

	};

	struct GetUserData {
		string userId;
	};

	class UserManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class UserManager;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestGetBuffer;
		static unbounded_buffer<User>* outBuffer;
		static concurrent_queue<User> usersToInsert;
		static overwrite_buffer<map<string, User>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		UserManager* users{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, UserManager* usersNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->users = usersNew;
		}

		static task<void> initialize() {
			UserManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetUserData>;
			UserManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetUserData>;
			UserManagerAgent::outBuffer = new unbounded_buffer<User>;
			co_return;
		}

		task<User> getObjectAsync(DiscordCoreInternal::GetUserData dataPackage) {
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
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::UserData userData;
			DiscordCoreInternal::parseObject(jsonValue, &userData);
			User userNew(userData, this->users);
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
					else {
						User user = getObjectAsync(getData).get();
						cacheTemp.insert(make_pair(getData.userId, user));
						send(UserManagerAgent::outBuffer, user);
						asend(UserManagerAgent::cache, cacheTemp);
					}
				}
				else {
					User user = getObjectAsync(getData).get();
					cacheTemp.insert(make_pair(getData.userId, user));
					send(UserManagerAgent::outBuffer, user);
					asend(UserManagerAgent::cache, cacheTemp);
				}
			}
			if (try_receive(UserManagerAgent::requestFetchBuffer, getData)) {
				map<string, User> cacheTemp;
				if (try_receive(UserManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.userId)) {
						cacheTemp.erase(getData.userId);
					}
				}
				User user = getObjectAsync(getData).get();
				cacheTemp.insert(make_pair(getData.userId, user));
				send(UserManagerAgent::outBuffer, user);
				asend(UserManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->users);
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

		task<User> fetchAsync(GetUserData getUserData) {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getUserData.userId;
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<User> getUserAsync(GetUserData getUserData) {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getUserData.userId;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<User> fetchCurrentUser() {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			DiscordCoreInternal::UserData userData;
			User user(userData, this);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<void> insertUser(User user) {
			UserManagerAgent userManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			userManagerAgent.start();
			UserManagerAgent::usersToInsert.push(user);
			userManagerAgent.wait(&userManagerAgent);
			co_return;
		}

	protected:

		friend class DiscordCoreClient;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };

		DiscordCoreInternal::HttpAgentResources agentResources;

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
		}

	};
	overwrite_buffer<map<string, User>> UserManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestGetBuffer;
	unbounded_buffer<User>* UserManagerAgent::outBuffer;
	concurrent_queue<User> UserManagerAgent::usersToInsert;
	
}
#endif