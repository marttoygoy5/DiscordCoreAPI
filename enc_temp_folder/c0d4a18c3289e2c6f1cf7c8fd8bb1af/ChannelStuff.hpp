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
		ChannelManager* channels{ nullptr };

		Channel() {};

	protected:
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;

		Channel(DiscordCoreInternal::ChannelData dataNew, ChannelManager* channelsNew) {
			this->initialize(dataNew, channelsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew,  ChannelManager* channelsNew) {
			this->data = dataNew;
			this->channels = channelsNew;
			co_return;
		}
	};

	struct GetChannelData {
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

		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* requestDMChannelBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;
		DiscordCoreInternal::HttpAgentResources agentResources;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::ChannelManager* channels;

		ChannelManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreAPI::ChannelManager* channelsNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->channels = channelsNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestDMChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetDMChannelData>;
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			co_return;
		}

		task<Channel> getChannelAsync(DiscordCoreInternal::GetChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			Channel channelNew(channelData, this->channels);
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
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			Channel channelNew(channelData, this->channels);
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
					else {
						Channel channel = getChannelAsync(getData).get();
						cacheTemp.insert(make_pair(getData.channelId, channel));
						send(ChannelManagerAgent::outBuffer, channel);
						asend(cache, cacheTemp);
					}
				}
				else {
					Channel channel = getChannelAsync(getData).get();
					cacheTemp.insert(make_pair(getData.channelId, channel));
					send(ChannelManagerAgent::outBuffer, channel);
					asend(cache, cacheTemp);
				}
			}
			if (try_receive(ChannelManagerAgent::requestFetchBuffer, getData)) {
				map<string, Channel> cacheTemp;
				if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.channelId)) {
						cacheTemp.erase(getData.channelId);
					}
				}
				Channel channel = getChannelAsync(getData).get();
				cacheTemp.insert(make_pair(getData.channelId, channel));
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
			Channel channelNew(channelData, this->channels);
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

		task<Channel> fetchAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Channel> getDMChannelAsync(GetDMChannelData getDMChannelData) {
			DiscordCoreInternal::GetDMChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = getDMChannelData.userId;
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestDMChannelBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<void> insertChannel(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			channelManagerAgent.start();
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.wait(&channelManagerAgent);
			co_return;
		}

	protected:
		friend class Guild;
		friend class ChannelManagerAgent;
		friend class DiscordCoreClient;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;
		
		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
		}

	};

	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* ChannelManagerAgent::requestDMChannelBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif
