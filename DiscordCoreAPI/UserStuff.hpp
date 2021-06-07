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
		DiscordCoreClient* coreClient{ nullptr };

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
			this->coreClient = coreClientNew;
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
		static unbounded_buffer<DiscordCoreInternal::GetApplicationData>* requestGetApplicationBuffer;
		static unbounded_buffer<User>* outBuffer;
		static unbounded_buffer<Application>* outApplicationBuffer;
		static concurrent_queue<User> usersToInsert;
		static overwrite_buffer<map<string, User>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew,  Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			UserManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::FetchUserData>;
			UserManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetUserData>;
			UserManagerAgent::requestGetApplicationBuffer = new unbounded_buffer<DiscordCoreInternal::GetApplicationData>;
			UserManagerAgent::outApplicationBuffer = new unbounded_buffer<Application>;
			UserManagerAgent::outBuffer = new unbounded_buffer<User>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
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
			User userNew(userData, this->coreClient);
			co_return userNew;
		}

		task<Application> getObjectAsync(DiscordCoreInternal::GetApplicationData dataPackage) {
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
				DiscordCoreInternal::GetApplicationData getAppData;
				if (try_receive(UserManagerAgent::requestGetApplicationBuffer, getAppData)) {
					Application application = getObjectAsync(getAppData).get();
					send(UserManagerAgent::outApplicationBuffer, application);
				}
				DiscordCoreInternal::UserData userData;
				User user(userData, this->coreClient);
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
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class UserManager {
	public:

		task<User> fetchAsync(FetchUserData fetchUserData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = fetchUserData.userId;
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			exception error;
			while (userManagerAgent.getError(error)) {
				cout << "UserManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->coreClient);
			try_receive(UserManagerAgent::outBuffer, user);
			co_await mainThread;
			co_return user;
		}

		task<User> fetchCurrentUserAsync() {
			DiscordCoreInternal::FetchUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestFetchBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			exception error;
			while (userManagerAgent.getError(error)) {
				cout << "UserManager::fetchCurrentUserAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->coreClient);
			try_receive(UserManagerAgent::outBuffer, user);
			co_return user;
		}

		task<User> getUserAsync(GetUserData getUserData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetUserData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getUserData.userId;
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			exception error;
			while (userManagerAgent.getError(error)) {
				cout << "UserManager::getUserAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::UserData userData;
			User user(userData, this->coreClient);
			try_receive(UserManagerAgent::outBuffer, user);
			co_await mainThread;
			co_return user;
		}

		task<Application> getApplicationDataAsync() {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetApplicationData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			UserManagerAgent userManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(UserManagerAgent::requestGetApplicationBuffer, dataPackage);
			userManagerAgent.start();
			agent::wait(&userManagerAgent);
			exception error;
			while (userManagerAgent.getError(error)) {
				cout << "UserManager::getApplicationDataAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ApplicationData applicationData;
			Application application(applicationData);
			try_receive(UserManagerAgent::outApplicationBuffer, application);
			co_await mainThread;
			co_return application;
		}

		task<void> insertUserAsync(User user) {
			UserManagerAgent userManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
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
		DiscordCoreClient* coreClient{ nullptr };

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->coreClient = coreClientNew;
		}

	};
	overwrite_buffer<map<string, User>> UserManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchUserData>* UserManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetUserData>* UserManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::GetApplicationData>* UserManagerAgent::requestGetApplicationBuffer;
	unbounded_buffer<Application>* UserManagerAgent::outApplicationBuffer;
	unbounded_buffer<User>* UserManagerAgent::outBuffer;
	concurrent_queue<User> UserManagerAgent::usersToInsert;
	
}
#endif