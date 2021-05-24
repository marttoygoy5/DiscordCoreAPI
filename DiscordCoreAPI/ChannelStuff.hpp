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
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;

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
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			return;
		}

		task<Channel> getObjectAsync(DiscordCoreInternal::FetchChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			try {
				agent::wait(&requestAgent);
			}
			catch (const exception& e) {
				cout << "ChannelManagerAgent::getObjectAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
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
			try {
				agent::wait(&requestAgent);
			}
			catch (const exception& e) {
				cout << "ChannelManagerAgent::postObjectAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
			}
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->coreClient);
			co_return channelNew;
		}

		void run() {
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
			DiscordCoreInternal::GetDMChannelData getDataNewest;
			if(try_receive(ChannelManagerAgent::requestDMChannelBuffer, getDataNewest)) {
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
			try {
				agent::wait(&channelManagerAgent);
			}
			catch (const exception& e) {
				cout << "ChannelManager::fetchAsync() Error: " << e.what() << endl << endl;
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
			try {
				agent::wait(&channelManagerAgent);
			}
			catch (const exception& e) {
				cout << "ChannelManager::getAsync() Error: " << e.what() << endl << endl;
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
			try {
				agent::wait(&channelManagerAgent);
			}
			catch (const exception& e) {
				cout << "ChannelManager::getDMChanneAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->coreClient);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<void> insertChannelAsync(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			channelManagerAgent.start();
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.wait(&channelManagerAgent);
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
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif
