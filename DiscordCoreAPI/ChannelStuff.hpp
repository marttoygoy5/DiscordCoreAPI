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

	class Channel {
	public:
		DiscordCoreInternal::ChannelData data;
		MessageManager* messages{ nullptr };
		ChannelManager* channels{ nullptr };

		Channel() {};

	protected:
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Channel(DiscordCoreInternal::ChannelData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, ChannelManager* channelsNew,  concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
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

	struct GetDMChannelData {
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreInternal::ThreadContext threadContext;
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
		DiscordCoreAPI::ChannelManager* channels;
		MessageManager* messages;

		ChannelManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, MessageManager* messagesNew, DiscordCoreAPI::ChannelManager* channelsNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->channels = channelsNew;
			this->messages = messagesNew;
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
			Channel channelNew(channelData, this->agentResources, this->channels, this->threads);
			co_return channelNew;
		}

		task<Channel> postObjectAsync(DiscordCoreInternal::SendDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
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
			Channel channelNew(channelData, this->agentResources, this->channels, this->threads);
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
				channel = cacheTemp.at(getData.channelId);
				cacheTemp.insert(make_pair(getData.channelId, channel));
				send(ChannelManagerAgent::outBuffer, channel);
				asend(cache, cacheTemp);
			}
			DiscordCoreInternal::SendDMData sendDMData;
			if (try_receive(ChannelManagerAgent::requestSendDMBuffer, sendDMData)) {
				SendDMData dataPackage;
				dataPackage.messageData.allowedMentions = sendDMData.messageData.allowedMentions;
				dataPackage.messageData.content = sendDMData.messageData.content;
				dataPackage.messageData.embed = sendDMData.messageData.embed;
				dataPackage.messageData.messageReference = sendDMData.messageData.messageReference;
				dataPackage.messageData.nonce = sendDMData.messageData.nonce;
				dataPackage.messageData.tts = sendDMData.messageData.tts;
				map<string, Channel> cacheTemp;
				if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
					bool isItFound = false;
					for (auto const& [key, value] : cacheTemp) {
						if (value.data.recipients.size() > 0) {
							if (value.data.recipients.at(0).id == sendDMData.userId) {
								isItFound = true;
								Message message = this->messages->createMessageAsync(dataPackage.messageData, value.data.id).get();
								send(ChannelManagerAgent::outMessageBuffer, message);
							}
						}
					}
					if (isItFound == false) {
						Channel channel = postObjectAsync(sendDMData).get();
						cacheTemp.insert(make_pair(channel.data.id, channel));
						Message message = this->messages->createMessageAsync(dataPackage.messageData, channel.data.id).get();
						send(ChannelManagerAgent::outMessageBuffer, message);
						asend(ChannelManagerAgent::cache, cacheTemp);
					}
				}
				else {
					Channel channel = postObjectAsync(sendDMData).get();
					cacheTemp.insert(make_pair(channel.data.id, channel));
					Message message = this->messages->createMessageAsync(dataPackage.messageData, channel.data.id).get();
					send(ChannelManagerAgent::outMessageBuffer, message);
					asend(ChannelManagerAgent::cache, cacheTemp);
				}
			}
			DiscordCoreInternal::ChannelData channelData;
			Channel channelNew(channelData, this->agentResources, this->channels, this->threads);
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
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this->messages, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestFetchBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->agentResources, this, this->threads);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData getChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this->messages, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->agentResources, this, this->threads);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Channel> getDMChannelAsync(GetDMChannelData getDMChannelData) {
			DiscordCoreInternal::GetChannelData dataPackage;
			dataPackage.agentResources = getDMChannelData.agentResources;
			dataPackage.threadContext = getDMChannelData.threadContext;
			dataPackage.channelId = getDMChannelData.channelId;
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this->messages, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestGetBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->agentResources, this, this->threads);
			try_receive(ChannelManagerAgent::outBuffer, channel);
			co_return channel;
		}

		task<Message> sendDMAsync(SendDMData sendDMData) {
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
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this->messages, this, this->threads->at(2).scheduler);
			send(ChannelManagerAgent::requestSendDMBuffer, dataPackage);
			channelManagerAgent.start();
			agent::wait(&channelManagerAgent);
			Message message;
			try_receive(ChannelManagerAgent::outMessageBuffer, message);
			co_return message;
		}

		task<void> insertChannel(Channel channel) {
			ChannelManagerAgent channelManagerAgent(this->threads, this->agentResources, this->messages, this, this->threads->at(2).scheduler);
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
		MessageManager* messages;
		
		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, MessageManager* messagesNew) {
			this->threads = threadsNew;
			this->messages = messagesNew;
			this->agentResources = agentResourcesNew;
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
