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
		friend class UserManagerAgent;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Channel(DiscordCoreInternal::ChannelData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, MessageManager* pMessageManager, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->initialize(dataNew, agentResourcesNew, channelsNew, pMessageManager, threadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::ChannelData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew, MessageManager* pMessageManager, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->channels = channelsNew;
			this->threads = threadsNew;
			this->messages = new MessageManager(this->agentResources, this->threads);
			pMessageManager = this->messages;
			co_return;
		}
	};

	struct GetChannelData {
		string channelId;
	};

	struct SendDMData {
		string userId;
		CreateMessageData messageData;
	};

	class ChannelManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestSendDMBuffer;
		static unbounded_buffer<Channel>* outBuffer;
		static unbounded_buffer<Message>* outMessageBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;
		DiscordCoreInternal::HttpAgentResources agentResources;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::ChannelManager* pChannelManager;
		MessageManager* messageManager;

		ChannelManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ChannelManager* pChannelManagerNew,  Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pChannelManager = pChannelManagerNew;
		}

		static task<void> initialize() {
			ChannelManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestSendDMBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			ChannelManagerAgent::outMessageBuffer = new unbounded_buffer<Message>;
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
			Channel channelNew(channelData, this->agentResources, this->pChannelManager,this->messageManager, this->threads);
			co_return channelNew;
		}

		task<Channel> postObjectAsync(DiscordCoreInternal::SendDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.relativePath = "/users/@me/channels";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::ChannelData channelData;
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			Channel channelNew(channelData, this->agentResources, this->pChannelManager, this->messageManager, this->threads);
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
			DiscordCoreInternal::SendDMData sendDMData;
			SendDMData dataPackage;
			dataPackage.messageData.allowedMentions = sendDMData.messageData.allowedMentions;
			dataPackage.messageData.content = sendDMData.messageData.content;
			dataPackage.messageData.embed = sendDMData.messageData.embed;
			dataPackage.messageData.messageReference = sendDMData.messageData.messageReference;
			dataPackage.messageData.nonce = sendDMData.messageData.nonce;
			dataPackage.messageData.tts = sendDMData.messageData.tts;
			if (try_receive(ChannelManagerAgent::requestSendDMBuffer, sendDMData)) {
				map<string, Channel> cacheTemp;
				if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
					bool isItFound = false;
					for (auto const& [key, value] : cacheTemp) {
						if (value.data.recipients.at(0).id == sendDMData.userId) {
							isItFound = true;
							Message message = this->messageManager->createMessageAsync(dataPackage.messageData, value.data.id).get();
							send(ChannelManagerAgent::outMessageBuffer, message);
						}
					}
					if (isItFound == false) {
						Channel channel = postObjectAsync(sendDMData).get();
						cacheTemp.insert(make_pair(channel.data.id, channel));
						Message message = this->messageManager->createMessageAsync(dataPackage.messageData, channel.data.id).get();
						send(ChannelManagerAgent::outMessageBuffer, message);
						asend(ChannelManagerAgent::cache, cacheTemp);
					}
				}
				else {
					Channel channel = postObjectAsync(sendDMData).get();
					cacheTemp.insert(make_pair(channel.data.id, channel));
					Message message = this->messageManager->createMessageAsync(dataPackage.messageData, channel.data.id).get();
					send(ChannelManagerAgent::outMessageBuffer, message);
					asend(ChannelManagerAgent::cache, cacheTemp);
				}
			}
			Channel channelNew;
			while (ChannelManagerAgent::channelsToInsert.try_pop(channelNew)) {
				map<string, Channel> cacheTemp;
				if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(channelNew.data.id)) {
						cacheTemp.erase(channelNew.data.id);
					}
				}				
				cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
				asend(cache, cacheTemp);
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
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			Channel channel;
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			Channel channel;
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Message> sendDM(SendDMData sendDMData) {
			DiscordCoreInternal::SendDMData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = sendDMData.userId;
			dataPackage.messageData.allowedMentions = sendDMData.messageData.allowedMentions;
			dataPackage.messageData.content = sendDMData.messageData.content;
			dataPackage.messageData.embed = sendDMData.messageData.embed;
			dataPackage.messageData.messageReference = sendDMData.messageData.messageReference;
			dataPackage.messageData.nonce = sendDMData.messageData.nonce;
			dataPackage.messageData.tts = sendDMData.messageData.tts;
			ChannelManagerAgent channelManagerAgent(this->threads, dataPackage.agentResources, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestSendDMBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			Message message;
			try_receive(ChannelManagerAgent::outMessageBuffer, message);
			co_return message;
		}

		task<void> insertChannel(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this, this->threads->at(2).scheduler);
			channelManagerAgent.start();
			ChannelManagerAgent::channelsToInsert.push(channel);
			channelManagerAgent.wait(&channelManagerAgent);
			co_return;
		}

	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;
		MessageManager* messageManager;

		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, MessageManager* messageManagerNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->messageManager = messageManagerNew;
		}

	};
	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* ChannelManagerAgent::requestSendDMBuffer;
	unbounded_buffer<Message>* ChannelManagerAgent::outMessageBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif
