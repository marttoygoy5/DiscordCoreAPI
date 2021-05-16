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
		Channel(DiscordCoreInternal::ChannelData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew) {
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->channels = channelsNew;
			this->guild = guildNew;
			this->messages = new MessageManager(this->agentResources, this->guild, this->pThreads);
		}
	protected:
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;
	};

	class ChannelManagerAgent : public agent {
	public:
		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static map<string, Channel> cache;

		ChannelManagerAgent(shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ChannelManager* pChannelManagerNew, Guild* pGuildNew)
			:agent(*pThreadsNew->at(8).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->pThreads = pThreadsNew;
			this->pChannelManager = pChannelManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			co_return;
		}

		~ChannelManagerAgent() {}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;
		DiscordCoreAPI::ChannelManager* pChannelManager;
		Guild* pGuild;

		task<Channel> fetchAsync(string channelId, Channel channel) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->pThreads->at(3).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::ChannelData channelData = channel.data;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			Channel channelNew(channelData, this->pGuild, this->agentResources, this->pChannelManager, this->pThreads);
			co_return channelNew;
		}

		void run() {
			string channelId;
			try {
				channelId = receive(ChannelManagerAgent::requestGetBuffer, 50U);
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
				channelId = receive(ChannelManagerAgent::requestFetchBuffer, 50U);
				if (ChannelManagerAgent::cache.contains(channelId)) {
					Channel Channel = ChannelManagerAgent::cache.at(channelId);
					ChannelManagerAgent::cache.erase(channelId);
					Channel = fetchAsync(channelId, Channel).get();
					ChannelManagerAgent::cache.insert(make_pair(channelId, Channel));
					send(ChannelManagerAgent::outBuffer, Channel);
				}
				else {
					Channel Channel = fetchAsync(channelId, Channel).get();
					ChannelManagerAgent::cache.insert(make_pair(channelId, Channel));
					send(ChannelManagerAgent::outBuffer, Channel);
				}
			}
			catch (exception error) {}
			Channel Channel;
			while (ChannelManagerAgent::channelsToInsert.try_pop(Channel)) {
				if (ChannelManagerAgent::cache.contains(Channel.data.id)) {
					ChannelManagerAgent::cache.erase(Channel.data.id);
					cout << Channel.data.name << endl;
				}
				ChannelManagerAgent::cache.insert(make_pair(Channel.data.id, Channel));
				cout << ChannelManagerAgent::cache.at(Channel.data.id).data.name << endl;
			}
			done();
		}
	};

	class ChannelManager : concurrent_unordered_map<string, Channel>, public implements<ChannelManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };
		ChannelManager() {}
		ChannelManager(Guild* guild, DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew) {
			this->guild = guild;
			this->agentResources = agentResourcesNew;
			this->pThreads = pThreadsNew;
		}

		task<Channel> fetchAsync(string channelId) {
			ChannelManagerAgent channelManagerAgent(this->pThreads, this->agentResources, this, this->guild);
			channelManagerAgent.start();
			send(ChannelManagerAgent::requestFetchBuffer, channelId);
			cout << "333WERE HERE WERE HERE WERE HERE" << endl;
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			cout << "222 WERE HERE WERE HERE WERE HERE" << endl;
			agent::wait(&channelManagerAgent);
			co_return channel;
		}

		task<Channel> getChannelAsync(string channelId) {
			ChannelManagerAgent channelManagerAgent(this->pThreads, this->agentResources, this, this->guild);
			channelManagerAgent.start();
			send(ChannelManagerAgent::requestGetBuffer, channelId);
			agent::wait(&channelManagerAgent);
			co_return receive(ChannelManagerAgent::outBuffer);
		}

		~ChannelManager() {}

	protected:
		friend class Guild;
		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;
		DiscordCoreInternal::HttpAgentResources agentResources;
	};
	map<string, Channel> ChannelManagerAgent::cache;
	unbounded_buffer<string>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<string>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif