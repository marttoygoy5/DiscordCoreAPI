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
		DiscordCoreInternal::ChannelData data;
		DiscordCoreClient* coreClient{ nullptr };

		Channel() {};

	protected:
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;

		Channel(DiscordCoreInternal::ChannelData dataNew, DiscordCoreClient* coreClientNew) {
			this->initialize(dataNew, coreClientNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew, DiscordCoreClient* coreClientNew) {
			this->data = dataNew;
			this->coreClient = coreClientNew;
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

		static unbounded_buffer<DiscordCoreInternal::FetchChannelData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* requestDMChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* requestPutChannelPermsBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* requestDeleteChannelPermsBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		ChannelManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::FetchChannelData>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestDMChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetDMChannelData>;
			ChannelManagerAgent::requestPutChannelPermsBuffer = new unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>;
			ChannelManagerAgent::requestDeleteChannelPermsBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>;
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Channel> getObjectAsync(DiscordCoreInternal::FetchChannelData dataPackage) {
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
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->coreClient);
			co_return channelNew;
		}

		task<Channel> postObjectAsync(DiscordCoreInternal::GetDMChannelData dataPackage) {
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
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->coreClient);
			co_return channelNew;
		}

		task<void> putObjectAsync(DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage) {
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

		task<void> deleteObjectAsync(DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage) {
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
				DiscordCoreInternal::GetChannelData getData;
				if (try_receive(ChannelManagerAgent::requestGetBuffer, getData)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(getData.channelId)) {
							Channel channel = cacheTemp.at(getData.channelId);
							send(ChannelManagerAgent::outBuffer, channel);
						}
					}
				}
				DiscordCoreInternal::FetchChannelData fetchData;
				if (try_receive(ChannelManagerAgent::requestFetchBuffer, fetchData)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(fetchData.channelId)) {
							cacheTemp.erase(fetchData.channelId);
						}
					}
					Channel channel = getObjectAsync(fetchData).get();
					cacheTemp.insert(make_pair(fetchData.channelId, channel));
					send(ChannelManagerAgent::outBuffer, channel);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::EditChannelPermissionOverwritesData editData;
				if (try_receive(ChannelManagerAgent::requestPutChannelPermsBuffer, editData)) {
					putObjectAsync(editData).get();
				}
				DiscordCoreInternal::DeleteChannelPermissionOverwritesData deleteData;
				if (try_receive(ChannelManagerAgent::requestDeleteChannelPermsBuffer, deleteData)) {
					deleteObjectAsync(deleteData).get();
				}
				DiscordCoreInternal::GetDMChannelData getDataNewest;
				if (try_receive(ChannelManagerAgent::requestDMChannelBuffer, getDataNewest)) {
					Channel channel = postObjectAsync(getDataNewest).get();
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channel.data.id)) {
							cacheTemp.erase(channel.data.id);
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outBuffer, channel);
							asend(cache, cacheTemp);
						}
	 					else {
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outBuffer, channel);
							asend(cache, cacheTemp);
						}
					}
					else {
						cacheTemp.insert(make_pair(channel.data.id, channel));
						send(ChannelManagerAgent::outBuffer, channel);
						asend(cache, cacheTemp);
					}
				}
				DiscordCoreInternal::ChannelData channelData;
				Channel channelNew(channelData, this->coreClient);
				while (ChannelManagerAgent::channelsToInsert.try_pop(channelNew)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channelNew.data.id)) {
							cacheTemp.erase(channelNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
					asend(ChannelManagerAgent::cache, cacheTemp);
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

		task<Channel> fetchAsync(FetchChannelData getChannelData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->coreClient);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::getChannelAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->coreClient);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<Channel> getDMChannelAsync(GetDMChannelData getDMChannelData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetDMChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getDMChannelData.userId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestDMChannelBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::getDMChannelAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->coreClient);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData editChannelPermissionOverwritesData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = editChannelPermissionOverwritesData.channelId;
			dataPackage.allow = editChannelPermissionOverwritesData.allow;
			dataPackage.deny = editChannelPermissionOverwritesData.deny;
			dataPackage.roleOrUserId = editChannelPermissionOverwritesData.roleOrUserId;
			dataPackage.type = editChannelPermissionOverwritesData.type;
			dataPackage.content = DiscordCoreInternal::getEditChannelPermissionOverwritesPayload(dataPackage);
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestPutChannelPermsBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::editChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData deleteChannelPermissionOverwritesData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.channelId = deleteChannelPermissionOverwritesData.channelId;
			dataPackage.roleOrUserId = deleteChannelPermissionOverwritesData.roleOrUserId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(6).scheduler);
			send(ChannelManagerAgent::requestDeleteChannelPermsBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::deleteChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> insertChannelAsync(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.start();
			channelManagerAgent.wait(&channelManagerAgent);
			exception error;
			while (channelManagerAgent.getError(error)) {
				cout << "ChannelManager::insertChannelAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}
	protected:

		friend class Guild;
		friend class ChannelManagerAgent;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* coreClient;
		
		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->coreClient = coreClientNew;
		}

	};

	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchChannelData>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* ChannelManagerAgent::requestDMChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* ChannelManagerAgent::requestPutChannelPermsBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* ChannelManagerAgent::requestDeleteChannelPermsBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif
