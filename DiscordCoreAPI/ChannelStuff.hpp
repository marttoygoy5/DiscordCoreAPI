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
		Channel() {}
		DiscordCoreInternal::ChannelData data;
		com_ptr<MessageManager> messages{ nullptr };
		com_ptr<ChannelManager> channels{ nullptr };
		Guild* guild{ nullptr };
		Channel(DiscordCoreInternal::ChannelData channelData, DiscordCoreInternal::HttpAgentResources agentResourcesNew, com_ptr<ChannelManager> channelManager, Guild* guildNew) {
			data = channelData;
			this->agentResources = agentResourcesNew;
			this->channels = channelManager;
			this->guild = guildNew;
			//this->messages = make_self<MessageManager>(this->pointers, this->guild);
		}
	protected:
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;
	};

	class ChannelManagerAgent : public agent {
	public:
		Guild* guild;

		ChannelManagerAgent(Scheduler* pSchedulerNew, com_ptr<ChannelManager> pChannelManagerNew)
			:agent(*pSchedulerNew)
		{
			this->pChannelManager = pChannelManagerNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			ChannelManagerAgent::outBuffer = new unbounded_buffer<Channel>;
			co_return;
		}

		~ChannelManagerAgent() {}

	protected:
		friend class ChannelManager;
		static map<string, Channel> cache;
		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		com_ptr<ChannelManager> pChannelManager;
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads;
		DiscordCoreInternal::HttpAgentPointers pointers;

		task<Channel> fetchAsyncWrapped(string channelId, Channel channel) {
			/*
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + channelId;
			cout << "CHANNEL RESULTS 0000" << endl;
			DiscordCoreInternal::HttpRequestAgent pGetAgent(this->pointers.pGETAgent->botToken, this->pointers.pGETAgent->baseURL, this->pointers.pGETAgent->);
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			cout << "WEVE SEND IT WEVE SEND IT!" << endl;
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			DiscordCoreInternal::ChannelData channelData = channel.data;
			cout << "CHANNEL RESULTS 11111" << endl;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			cout << "CHANNEL RESULTS 222222" << endl;
			Channel channelNew(channelData, this->pointers, this->pChannelManager, this->guild);
			cout << "CHANNEL RESULTS 333333" << endl;
			ChannelManagerAgent::cache.insert(make_pair(channelId, channelNew));
			co_return channel;
			*/
		}

		task<Channel> getChannelAsyncWrapped(string channelId) {
			/*
			if (ChannelManagerAgent::cache.contains(channelId)) {
				Channel channel = ChannelManagerAgent::cache.at(channelId);
				ChannelManagerAgent::cache.erase(channel.data.id);
				co_return channel;
			}
			else {
				DiscordCoreInternal::ChannelData channelData;
				Channel channel(channelData, this->pointers, this->pChannelManager, this->guild);
				co_return channel;
			}
			*/
		}

		void run() {
			try {/*
				string channelId = receive(ChannelManagerAgent::requestFetchBuffer, 100U);
				cout << "CHANNEL ID: " << channelId << endl;
				if (ChannelManagerAgent::cache.contains(channelId)) {
					Channel channel = ChannelManagerAgent::cache.at(channelId);
					ChannelManagerAgent::cache.erase(channelId);
					channel = fetchAsyncWrapped(channelId, channel).get();
					send(ChannelManagerAgent::outBuffer, channel);
				}
				else {
					DiscordCoreInternal::ChannelData channelData;
					//Channel channel(channelData, this->pointers, this->pChannelManager, this->guild);
					channel = fetchAsyncWrapped(channelId, channel).get();
					send(ChannelManagerAgent::outBuffer, channel);
				}
			}
			catch (exception error) {
				//cout << "run() Error: " << error.what() << endl;
			}
			string channelId;
			if (try_receive(ChannelManagerAgent::requestGetBuffer, channelId)) {
				Channel channel = getChannelAsyncWrapped(channelId).get();
				send(ChannelManagerAgent::outBuffer, channel);
			}
			Channel channel;
			while (ChannelManagerAgent::channelsToInsert.try_pop(channel)) {
				if (ChannelManagerAgent::cache.contains(channel.data.id)) {
					ChannelManagerAgent::cache.erase(channel.data.id);
				}
				ChannelManagerAgent::cache.insert(make_pair(channel.data.id, channel));
				cout << ChannelManagerAgent::cache.at(channel.data.id).data.name << endl;
			}
			*/
				done();
			}
			catch (exception error) {

			}
		};
	};

	class ChannelManager : concurrent_unordered_map<string, Channel>, public implements<ChannelManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };
		ChannelManager() {}
		ChannelManager(Guild* guild, DiscordCoreInternal::HttpAgentResources agentResourcesNew, com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreadsNew) {
			this->guild = guild;
			this->agentResources = agentResourcesNew;
			this->pSystemThreads = pSystemThreadsNew;
		}

		task<Channel> fetchAsync(string channelId) {
			com_ptr<ChannelManager> pChannelManager;
			pChannelManager.attach(this);
			SchedulerPolicy policy;
			policy.SetConcurrencyLimits(1, 1);
			policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
			policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
			CurrentScheduler::Create(policy);
			Scheduler* pScheduler = CurrentScheduler::Get();
			ChannelManagerAgent channelManagerAgent(pScheduler, pChannelManager);
			channelManagerAgent.start();
			send(ChannelManagerAgent::requestFetchBuffer, channelId);
			Channel channel = receive(ChannelManagerAgent::outBuffer);
			cout << "CHANNEL NAME CHANNEL NAME: " << channel.data.name << endl;
			co_return channel;
		}

		task<Channel> getChannelAsync(string channelId) {
			com_ptr<ChannelManager> pChannelManager;
			pChannelManager.attach(this);
			ChannelManagerAgent channelManagerAgent(this->pSystemThreads->Threads.at(4).scheduler, pChannelManager);
			channelManagerAgent.start();
			send(ChannelManagerAgent::requestGetBuffer, channelId);
			co_return receive(ChannelManagerAgent::outBuffer);
		}
		~ChannelManager() {}

	protected:
		friend class Guild;
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads;
		DiscordCoreInternal::HttpAgentResources agentResources;
	};
	map<string, Channel> ChannelManagerAgent::cache;
	unbounded_buffer<string>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<string>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif