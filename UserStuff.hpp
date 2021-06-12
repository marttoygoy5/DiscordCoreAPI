// UserStuff.hpp - Header for user-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _USER_STUFF_
#define _USER_STUFF_

#include "pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreAPI {
	
	class UserManager;

	class DiscordCoreClient;

	class Application {
	public:
		DiscordCoreInternal::ApplicationData data;

		Application() {};

	protected:
		friend class UserManager;
		friend class UserManagerAgent;

		Application(DiscordCoreInternal::ApplicationData dataNew) {
			this->data = dataNew;
		}
	};

	class User {
	public:

		DiscordCoreInternal::UserData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		User() {};

	protected:
		friend class DiscordCoreClient;
		friend class UserManagerAgent;
		friend class UserManager;
		friend class Guild;
		friend class GuildManager;

		User(DiscordCoreInternal::UserData dataNew,   DiscordCoreClient* coreClientNew) {
			this->initialize(dataNew, coreClientNew).get();
		}

		task<void> initialize(DiscordCoreInternal::UserData dataNew, DiscordCoreClient* coreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = coreClientNew;
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
		static unbounded_buffer<DiscordCoreInternal::FetchUserData>* requestFetchUserBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetUserData>* requestGetUserBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetApplicationData>* requestGetApplicationBuffer;
		static unbounded_buffer<User>* outUserBuffer;
		static unbounded_buffer<Application>* outApplicationBuffer;
		static concurrent_queue<User> usersToInsert;
		static overwrite_buffer<map<string, User>> cache2;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew,  Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			UserManagerAgent::requestFetchUserBuffer = new unbounded_buffer<DiscordCoreInternal::FetchUserData>;
			UserManagerAgent::requestGetUserBuffer = new unbounded_buffer<DiscordCoreInternal::GetUserData>;
			UserManagerAgent::requestGetApplicationBuffer = new unbounded_buffer<DiscordCoreInternal::GetApplicationData>;
			UserManagerAgent::outApplicationBuffer = new unbounded_buffer<Application>;
			UserManagerAgent::outUserBuffer = new unbounded_buffer<User>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<User> getObjectDataAsync(DiscordCoreInternal::FetchUserData dataPackage) {
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			UserData userData;
			DiscordCoreInternal::parseObject(returnData.data, &userData);
			User userNew(userData, this->discordCoreClient);
			co_return userNew;
		}

		task<Application> getObjectDataAsync(DiscordCoreInternal::GetApplicationData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_APPLICATION;
			workload.relativePath = "/oauth2/applications/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ApplicationData applicationData;
			DiscordCoreInternal::parseObject(returnData.data, &applicationData);
			Application application(applicationData);
			co_return application;
		}

		void run() {
			try {
				DiscordCoreInternal::GetUserData dataPackage01;
				if (try_receive(UserManagerAgent::requestGetUserBuffer, dataPackage01)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.userId)) {
							User user = cacheTemp.at(dataPackage01.userId);
							send(UserManagerAgent::outUserBuffer, user);
						}
					}
				}
				DiscordCoreInternal::FetchUserData dataPackage02;
				if (try_receive(UserManagerAgent::requestFetchUserBuffer, dataPackage02)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.userId)) {
							cacheTemp.erase(dataPackage02.userId);
						}
					}
					User user = getObjectDataAsync(dataPackage02).get();
					cacheTemp.insert(make_pair(dataPackage02.userId, user));
					send(UserManagerAgent::outUserBuffer, user);
					asend(UserManagerAgent::cache2, cacheTemp);
				}
				DiscordCoreInternal::GetApplicationData dataPackage03;
				if (try_receive(UserManagerAgent::requestGetApplicationBuffer, dataPackage03)) {
					Application application = getObjectDataAsync(dataPackage03).get();
					send(UserManagerAgent::outApplicationBuffer, application);
				}
				DiscordCoreInternal::UserData dataPackage04;
				User user(dataPackage04, this->discordCoreClient);
				while (UserManagerAgent::usersToInsert.try_pop(user)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(user.data.id)) {
							cacheTemp.erase(user.data.id);
						}
					}
					cacheTemp.insert(make_pair(user.data.id, user));
					asend(UserManagerAgent::cache2, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class UserManager {
	public:

		task<User> fetchAsync(FetchUserData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(UserManagerAgent::outUserBuffer, user);
			co_await mainThread;
			co_return user;
		}

		task<User> fetchCurrentUserAsync() {
			DiscordCoreInternal::FetchUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManager::fetchCurrentUserAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(UserManagerAgent::outUserBuffer, user);
			co_return user;
		}

		task<User> getUserAsync(GetUserData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.userId = dataPackage.userId;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManager::getUserAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(UserManagerAgent::outUserBuffer, user);
			co_await mainThread;
			co_return user;
		}

		task<Application> getApplicationDataAsync() {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetApplicationData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetApplicationBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "UserManager::getApplicationDataAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ApplicationData applicationData;
			Application application(applicationData);
			try_receive(UserManagerAgent::outApplicationBuffer, application);
			co_await mainThread;
			co_return application;
		}

		task<void> insertUserAsync(User user) {
			UserManagerAgent userManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			UserManagerAgent::usersToInsert.push(user);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			exception error;
			while (userManagerAgent.getError(error)) {
				cout << "UserManager::inserUserAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

	};
	overwrite_buffer<map<string, User>> UserManagerAgent::cache2;
	unbounded_buffer<DiscordCoreInternal::FetchUserData>* UserManagerAgent::requestFetchUserBuffer;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestGetUserBuffer;
	unbounded_buffer<DiscordCoreInternal::GetApplicationData>* UserManagerAgent::requestGetApplicationBuffer;
	unbounded_buffer<Application>* UserManagerAgent::outApplicationBuffer;
	unbounded_buffer<User>* UserManagerAgent::outUserBuffer;
	concurrent_queue<User> UserManagerAgent::usersToInsert;
	
}
#endif