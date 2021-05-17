// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"
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
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Channel(DiscordCoreInternal::ChannelData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->initialize(dataNew, guildNew, agentResourcesNew, channelsNew, threadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
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
		Channel oldChannel;
	};

	class ChannelManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static map<string, Channel> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::ChannelManager* pChannelManager;
		Guild* pGuild;

		ChannelManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ChannelManager* pChannelManagerNew, Guild* pGuildNew)
			:agent(*threadsNew->at(8).scheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pChannelManager = pChannelManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			co_return;
		}

		task<Channel> getChannelAsync(GetChannelData getData) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + getData.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(3).scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::ChannelData channelData = getData.oldChannel.data;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			Channel channelNew(channelData, this->pGuild, this->agentResources, this->pChannelManager, this->threads);
			co_return channelNew;
		}

		void run() {
			string channelId;
			try {
				channelId = receive(ChannelManagerAgent::requestGetBuffer, 1U);
				if (ChannelManagerAgent::cache.contains(channelId)) {
					Channel channel = ChannelManagerAgent::cache.at(channelId);
					send(ChannelManagerAgent::outBuffer, channel);
				}
				else {
					Channel channel;
					send(ChannelManagerAgent::outBuffer, channel);
				}
			}
			catch (exception error) {}
			try {
				channelId = receive(ChannelManagerAgent::requestFetchBuffer, 1U);
				if (ChannelManagerAgent::cache.contains(channelId)) {
					GetChannelData getData;
					getData.channelId = channelId;
					getData.oldChannel = ChannelManagerAgent::cache.at(channelId);
					ChannelManagerAgent::cache.erase(channelId);
					Channel channel = getChannelAsync(getData).get();
					ChannelManagerAgent::cache.insert(make_pair(channelId, channel));
					send(ChannelManagerAgent::outBuffer, channel);
				}
				else {
					GetChannelData getData;
					getData.channelId = channelId;
					Channel Channel = getChannelAsync(getData).get();
					ChannelManagerAgent::cache.insert(make_pair(channelId, Channel));
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

		task<Channel> fetchAsync(string channelId) {
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this, this->guild);
			send(ChannelManagerAgent::requestFetchBuffer, channelId);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			co_return channel;
		}

		task<Channel> getChannelAsync(string channelId) {
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this, this->guild);
			send(ChannelManagerAgent::requestGetBuffer, channelId);
			channelManagerAgent.start();
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			agent::wait(&channelManagerAgent);
			co_return channel;
		}

	protected:
		friend class Guild;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		ChannelManager(Guild* guild, DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->guild = guild;
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}

	};
	map<string, Channel> ChannelManagerAgent::cache;
	unbounded_buffer<string>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<string>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif