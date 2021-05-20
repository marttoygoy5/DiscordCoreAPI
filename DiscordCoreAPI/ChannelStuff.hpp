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
		MessageManager* messages{ nullptr };
		ChannelManager* channels{ nullptr };

		Channel() {}
		
	protected:
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Channel(DiscordCoreInternal::ChannelData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->initialize(dataNew, agentResourcesNew, channelsNew, threadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->channels = channelsNew;
			this->threads = threadsNew;
			this->messages = new MessageManager(this->agentResources, this->threads);
			co_return;
		}
	};
	struct GetChannelData {

		string channelId;
	};

	class ChannelManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;
		DiscordCoreInternal::HttpAgentResources agentResources;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::ChannelManager* pChannelManager;
		Guild* pGuild;

		ChannelManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ChannelManager* pChannelManagerNew,  Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pChannelManager = pChannelManagerNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
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
			Channel channelNew(channelData,  this->agentResources, this->pChannelManager, this->threads);
			co_return channelNew;
		}

		void run() {
			DiscordCoreInternal::GetChannelData getData;
			try {
				getData = receive(ChannelManagerAgent::requestGetBuffer, 1U);
				map<string, Channel> cacheTemp = receive(cache, 1U);
				if (cacheTemp.contains(getData.channelId)) {
					Channel channel = cacheTemp.at(getData.channelId);
					send(ChannelManagerAgent::outBuffer, channel);
				}
			}
			catch (exception error) {}
			try {
				getData = receive(ChannelManagerAgent::requestFetchBuffer, 1U);
				map<string, Channel> cacheTemp = receive(ChannelManagerAgent::cache, 1U);
				if (cacheTemp.contains(getData.channelId)) {
					cacheTemp.erase(getData.channelId);
				}
				Channel channel = getChannelAsync(getData).get();
				cacheTemp.insert(make_pair(getData.channelId, channel));
				send(ChannelManagerAgent::outBuffer, channel);
				asend(cache, cacheTemp);
			}
			catch (exception error) {}
			Channel channelNew;
			while (ChannelManagerAgent::channelsToInsert.try_pop(channelNew)) {
				map<string, Channel> cacheTemp;
				try {
					cacheTemp = receive(ChannelManagerAgent::cache, 1U);
				}
				catch (exception error) {}
				
				if (cacheTemp.contains(channelNew.data.id)) {
					cacheTemp.erase(channelNew.data.id);
				}
				cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
				
				asend(cache, cacheTemp);

			}
			done();
		}
	};

	class ChannelManager : concurrent_unordered_map<string, Channel> {
	public:

		task<Channel> fetchAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(4).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			critical_section critSection;
			critSection.lock();
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(4).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			critSection.unlock();
			co_return channel;
		}

		task<void> insertChannel(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this, this->threads->at(4).scheduler);
			channelManagerAgent.start();
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.wait(&channelManagerAgent);
			co_return;
		}

	protected:
		friend class Guild;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}

	};
	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif