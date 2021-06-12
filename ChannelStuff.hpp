// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "pch.h"
#include "FoundationEntities.hpp"
#include "MessageStuff.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	class ChannelManager;

	class Channel {
	public:
		ChannelData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Channel() {}

	protected:
		friend class DiscordCoreClient;
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;

		Channel(ChannelData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->initialize(dataNew, discordCoreClientNew).get();
		}

		task<void> initialize(ChannelData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
			co_return;
		}
	};

	struct GetChannelData {
		string channelId;
	};

	struct FetchChannelData {
		string channelId;
	};

	struct EditChannelPermissionOverwritesData {
		string allow;
		string deny;
		unsigned int type;
		string roleOrUserId;
		string channelId;
	};

	struct DeleteChannelPermissionOverwritesData {
		string channelId;
		string roleOrUserId;
	};

	struct GetDMChannelData {
		string userId;
	};

	class ChannelManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static unbounded_buffer<DiscordCoreInternal::FetchChannelData>* requestFetchChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* requestGetDMChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* requestPutChannelPermOWsBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* requestDeleteChannelPermOWsBuffer;
		static unbounded_buffer<Channel>* outChannelBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache2;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources; 
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		ChannelManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize() {
			ChannelManagerAgent::requestFetchChannelBuffer = new unbounded_buffer<DiscordCoreInternal::FetchChannelData>;
			ChannelManagerAgent::requestGetChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestGetDMChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetDMChannelData>;
			ChannelManagerAgent::requestPutChannelPermOWsBuffer = new unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>;
			ChannelManagerAgent::requestDeleteChannelPermOWsBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>;
			ChannelManagerAgent::outChannelBuffer = new unbounded_buffer<Channel>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Channel> getObjectDataAsync(DiscordCoreInternal::FetchChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)){
				cout << "ChannelManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->discordCoreClient);
			co_return channelNew;
		}

		task<Channel> postObjectDataAsync(DiscordCoreInternal::GetDMChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/users/@me/channels";
			json theValue = { {"recipient_id", dataPackage.userId } };
			workload.content = theValue.dump();
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::postObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::postObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->discordCoreClient);
			co_return channelNew;
		}

		task<void> putObjectDataAsync(DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::putObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::putObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::putObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::deleteObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::deleteObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::deleteObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		void run() {
			try {
				DiscordCoreInternal::GetChannelData dataPackage01;
				if (try_receive(ChannelManagerAgent::requestGetChannelBuffer, dataPackage01)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.channelId)) {
							Channel channel = cacheTemp.at(dataPackage01.channelId);
							send(ChannelManagerAgent::outChannelBuffer, channel);
						}
					}
				}
				DiscordCoreInternal::FetchChannelData dataPackage02;
				if (try_receive(ChannelManagerAgent::requestFetchChannelBuffer, dataPackage02)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId)) {
							cacheTemp.erase(dataPackage02.channelId);
						}
					}
					Channel channel = getObjectDataAsync(dataPackage02).get();
					cacheTemp.insert(make_pair(dataPackage02.channelId, channel));
					send(ChannelManagerAgent::outChannelBuffer, channel);
					asend(cache2, cacheTemp);
				}
				DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage03;
				if (try_receive(ChannelManagerAgent::requestPutChannelPermOWsBuffer, dataPackage03)) {
					putObjectDataAsync(dataPackage03).get();
				}
				DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage04;
				if (try_receive(ChannelManagerAgent::requestDeleteChannelPermOWsBuffer, dataPackage04)) {
					deleteObjectDataAsync(dataPackage04).get();
				}
				DiscordCoreInternal::GetDMChannelData dataPackage05;
				if (try_receive(ChannelManagerAgent::requestGetDMChannelBuffer, dataPackage05)) {
					Channel channel = postObjectDataAsync(dataPackage05).get();
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(channel.data.id)) {
							cacheTemp.erase(channel.data.id);
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outChannelBuffer, channel);
							asend(cache2, cacheTemp);
						}
	 					else {
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outChannelBuffer, channel);
							asend(cache2, cacheTemp);
						}
					}
					else {
						cacheTemp.insert(make_pair(channel.data.id, channel));
						send(ChannelManagerAgent::outChannelBuffer, channel);
						asend(cache2, cacheTemp);
					}
				}
				ChannelData dataPackage06;
				Channel channelNew(dataPackage06, this->discordCoreClient);
				while (ChannelManagerAgent::channelsToInsert.try_pop(channelNew)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(channelNew.data.id)) {
							cacheTemp.erase(channelNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
					asend(ChannelManagerAgent::cache2, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class ChannelManager {
	public:

		task<Channel> fetchAsync(FetchChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestFetchChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManager::getChannelAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<Channel> getDMChannelAsync(GetDMChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetDMChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.userId = dataPackage.userId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetDMChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManager::getDMChannelAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allow = dataPackage.allow;
			dataPackageNew.deny = dataPackage.deny;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			dataPackageNew.type = dataPackage.type;
			dataPackageNew.content = DiscordCoreInternal::getEditChannelPermissionOverwritesPayload(dataPackageNew);
			ChannelManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestPutChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManager::editChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(7);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(6).scheduler);
			send(ChannelManagerAgent::requestDeleteChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManager::deleteChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> insertChannelAsync(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.start();
			channelManagerAgent.wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::insertChannelAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeChannelAsync(string channelId) {
			apartment_context mainThread;
			co_await resume_background();
			map<string, Channel> cache2;;
			try_receive(ChannelManagerAgent::cache2, cache2);
			if (cache2.contains(channelId)) {
				cache2.erase(channelId);
			}
			asend(ChannelManagerAgent::cache2, cache2);
			co_await mainThread;
			co_return;
		}

	protected:

		friend class Guild;
		friend class ChannelManagerAgent;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* discordCoreClient;
		
		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

	};
	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache2;
	unbounded_buffer<DiscordCoreInternal::FetchChannelData>* ChannelManagerAgent::requestFetchChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* ChannelManagerAgent::requestGetDMChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* ChannelManagerAgent::requestPutChannelPermOWsBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* ChannelManagerAgent::requestDeleteChannelPermOWsBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outChannelBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif
