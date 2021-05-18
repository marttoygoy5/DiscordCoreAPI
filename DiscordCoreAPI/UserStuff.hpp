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

		User() {}

		DiscordCoreInternal::UserData data;

		UserManager* users{ nullptr };

	protected:
		friend class DiscordCoreClient;
		friend class UserManagerAgent;
		friend class UserManager;
		friend class Guild;
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;

		User(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::UserData dataNew,  UserManager* usersNew) {
			this->initialize(threadsNew, agentResourcesNew, dataNew, usersNew).get();
		}

		task<void> initialize(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::UserData dataNew, UserManager* usersNew) {
			this->threads = threadsNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->users = usersNew;
			co_return;
		}

	};

	struct GetUserData {
		string userId;
	};

	class UserManagerAgent : public agent {
	public:
		static map<string, User> cache;

	protected:
		friend class DiscordCoreClient;
		friend class UserManager;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestGetBuffer;
		static unbounded_buffer<User>* outBuffer;
		static concurrent_queue<User> usersToInsert;
		

		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::UserManager* pUserManager;

		UserManagerAgent(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::UserManager* pUserManagerNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pUserManager = pUserManagerNew;
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
			workload.relativePath = "/users/" + dataPackage.userId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::UserData userData;
			DiscordCoreInternal::parseObject(jsonValue, &userData);
			User userNew(this->threads, this->agentResources, userData, this->pUserManager);
			co_return userNew;
		}

		void run() {
			try {
				DiscordCoreInternal::GetUserData getData = receive(UserManagerAgent::requestGetBuffer, 1U);
				if (UserManagerAgent::cache.contains(getData.userId)) {
					User user = UserManagerAgent::cache.at(getData.userId);
					send(UserManagerAgent::outBuffer, user);
				}
				else {
					DiscordCoreInternal::UserData userData;
					User user(this->threads, this->agentResources, userData, this->pUserManager);
					send(UserManagerAgent::outBuffer, user);
				}
			}
			catch (exception error) {}
			try {
				DiscordCoreInternal::GetUserData getData = receive(UserManagerAgent::requestFetchBuffer, 1U);
				if (UserManagerAgent::cache.contains(getData.userId)) {
					UserManagerAgent::cache.erase(getData.userId);
					User user = getObjectAsync(getData).get();
					UserManagerAgent::cache.insert(make_pair(getData.userId, user));
					send(UserManagerAgent::outBuffer, user);
				}
				else {
					User user = getObjectAsync(getData).get();
					UserManagerAgent::cache.insert(make_pair(getData.userId, user));
					send(UserManagerAgent::outBuffer, user);
				}
			}
			catch (exception error) {}
			DiscordCoreInternal::UserData userData;
			User user(this->threads, this->agentResources, userData, this->pUserManager);
			while (UserManagerAgent::usersToInsert.try_pop(user)) {
				if (UserManagerAgent::cache.contains(user.data.id)) {
					UserManagerAgent::cache.erase(user.data.id);
				}
				UserManagerAgent::cache.insert(make_pair(user.data.id, user));
			}
			done();
		}
	};

	class UserManager {
	public:

		task<User> fetchAsync(GetUserData getUserData) {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(4);
			dataPackage.userId = getUserData.userId;
			UserManagerAgent userManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(3).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			User user = receive(UserManagerAgent::outBuffer);
			agent::wait(&userManagerAgent);
			co_return user;
		}

		task<User> getUserAsync(GetUserData getUserData) {
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(4);
			dataPackage.userId = getUserData.userId;
			UserManagerAgent userManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(3).scheduler);
			send(UserManagerAgent::requestGetBuffer, dataPackage);
			userManagerAgent.start();
			User user = receive(UserManagerAgent::outBuffer);
			agent::wait(&userManagerAgent);
			co_return user;
		}

		task<void> insertUser(User user) {
			UserManagerAgent userManagerAgent(this->threads, this->agentResources, this, this->threads->at(3).scheduler);
			userManagerAgent.start();
			UserManagerAgent::usersToInsert.push(user);
			userManagerAgent.wait(&userManagerAgent);
			//cout << "CURRENT USER NAME: " << UserManagerAgent::cache.at(user.data.id).data.username << endl;
			co_return;
		}

	protected:

		friend class DiscordCoreClient;

		vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };

		DiscordCoreInternal::HttpAgentResources agentResources;

		UserManager(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
		}

	};
	map<string, User> UserManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestGetBuffer;
	unbounded_buffer<User>* UserManagerAgent::outBuffer;
	concurrent_queue<User> UserManagerAgent::usersToInsert;
	
}
#endif