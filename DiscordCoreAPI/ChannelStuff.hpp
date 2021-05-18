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

	class ChannelManagerAgent;

	class Guild;

	class Channel {
	public:
		DiscordCoreInternal::ChannelData data;
		MessageManager* messages{ nullptr };
		ChannelManager* channels{ nullptr };
		Guild* guild{ nullptr };

		Channel() {}
		
	protected:
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;

		Channel(DiscordCoreInternal::ChannelData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->initialize(dataNew, guildNew, agentResourcesNew, channelsNew, threadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->channels = channelsNew;
			this->guild = guildNew;
			this->threads = threadsNew;
			this->messages = new MessageManager(this->agentResources, this->guild, this->threads);
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
		static map<string, Channel> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::ChannelManager* pChannelManager;
		Guild* pGuild;

		ChannelManagerAgent(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ChannelManager* pChannelManagerNew, Guild* pGuildNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pChannelManager = pChannelManagerNew;
			this->pGuild = pGuildNew;
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
			Channel channelNew(channelData, this->pGuild, this->agentResources, this->pChannelManager, this->threads);
			co_return channelNew;
		}

		void run() {
			DiscordCoreInternal::GetChannelData getData;
			try {
				getData = receive(ChannelManagerAgent::requestGetBuffer, 1U);
				if (ChannelManagerAgent::cache.contains(getData.channelId)) {
					Channel channel = ChannelManagerAgent::cache.at(getData.channelId);
					send(ChannelManagerAgent::outBuffer, channel);
				}
				else {
					Channel channel;
					send(ChannelManagerAgent::outBuffer, channel);
				}
			}
			catch (exception error) {}
			try {
				getData = receive(ChannelManagerAgent::requestFetchBuffer, 1U);
				if (ChannelManagerAgent::cache.contains(getData.channelId)) {
					ChannelManagerAgent::cache.erase(getData.channelId);
					Channel channel = getChannelAsync(getData).get();
					ChannelManagerAgent::cache.insert(make_pair(getData.channelId, channel));
					send(ChannelManagerAgent::outBuffer, channel);
				}
				else {
					Channel Channel = getChannelAsync(getData).get();
					ChannelManagerAgent::cache.insert(make_pair(getData.channelId, Channel));
					send(ChannelManagerAgent::outBuffer, Channel);
				}
			}
			catch (exception error) {}
			Channel Channel;
			while (ChannelManagerAgent::channelsToInsert.try_pop(Channel)) {
				if (ChannelManagerAgent::cache.contains(Channel.data.id)) {
					ChannelManagerAgent::cache.erase(Channel.data.id);
				}
				ChannelManagerAgent::cache.insert(make_pair(Channel.data.id, Channel));
			}
			done();
		}
	};


	class ChannelManager : concurrent_unordered_map<string, Channel> {
	public:
		Guild* guild{ nullptr };

		task<Channel> fetchAsync(GetChannelData getChannelData) {
			critical_section lock;
			lock.lock();
			co_await resume_foreground(*this->threads->at(0).threadQueue.get());
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(4);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->guild, this->threads->at(3).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			lock.unlock();
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(4);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->guild, this->threads->at(3).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			co_return channel;
		}

	protected:
		friend class Guild;
		vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		ChannelManager(Guild* guild, DiscordCoreInternal::HttpAgentResources agentResourcesNew, vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->guild = guild;
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}

	};
	map<string, Channel> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif