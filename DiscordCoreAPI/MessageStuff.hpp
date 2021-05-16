// MessageStuff.hpp - Header for Message classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _MESSAGE_STUFF_
#define _MESSAGE_STUFF_

#include "pch.h"
#include "ReactionStuff.hpp"
#include "JSONifier.hpp"

namespace DiscordCoreAPI {

	class Guild;

	class MessageManager;

	class Message {
	public:
		DiscordCoreInternal::MessageData data;
		MessageManager* messages{ nullptr };
		ReactionManager* reactions{ nullptr };
		Guild* guild{ nullptr };
		Message() {}
		
		task<Message> editMessageAsync(DiscordCoreInternal::EditMessageData editMessageData) {
			Message message;
			co_return message;
		}

		task<void> deleteMessageAsync(unsigned int timeDelay = 0) {
			co_return;
		}

		~Message() {}

	protected:
		friend struct DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		Message(DiscordCoreInternal::MessageData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, MessageManager* pMessageManagerNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->guild = guildNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->messages = pMessageManagerNew;
			this->threads = threadsNew;
			this->reactions = new ReactionManager(this->agentResources, this->guild);
		}
		

	};

	struct MessageManagerDataPackage {
		string messageId;
		string channelId;
		string content;
	};

	class MessageManagerAgent : public agent {
	public:
		static unbounded_buffer<MessageManagerDataPackage>* requestFetchBuffer;
		static unbounded_buffer<MessageManagerDataPackage>* requestGetBuffer;
		static unbounded_buffer<MessageManagerDataPackage>* requestPostBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message> messagesToInsert;
		static map<string, Message> cache;

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::MessageManager* pMessageManagerNew, Guild* pGuildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew)
			:agent(*threadsNew->at(0).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pMessageManager = pMessageManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<MessageManagerDataPackage>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<MessageManagerDataPackage>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<MessageManagerDataPackage>;
			MessageManagerAgent::outBuffer = new unbounded_buffer<Message>;
			co_return;
		}

		~MessageManagerAgent() {}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::MessageManager* pMessageManager;
		Guild* pGuild;

		task<Message> fetchAsync(string messageId, string channelId, Message message) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + channelId + "/messages/" + messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(3).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::MessageData messageData = message.data;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->pGuild, this->agentResources, this->pMessageManager, this->threads);
			co_return messageNew;
		}

		task<Message> postAsync(string channelId, string content) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = content;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(5).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->pGuild, this->agentResources, this->pMessageManager, this->threads);
			co_return messageNew;
		}

		void run() {
			MessageManagerDataPackage dataPackage;
			try {
				dataPackage = receive(MessageManagerAgent::requestPostBuffer, 50U);
				Message message = this->postAsync(dataPackage.channelId, dataPackage.content).get();
				send(MessageManagerAgent::outBuffer, message);
			}
			catch (exception error) {}
			try {
				dataPackage = receive(MessageManagerAgent::requestGetBuffer, 50U);
				if (MessageManagerAgent::cache.contains(dataPackage.channelId + dataPackage.messageId)) {
					Message message = MessageManagerAgent::cache.at(dataPackage.channelId + dataPackage.messageId);
					cout << "GUILD NAME111: " << message.data.id << endl;
					send(MessageManagerAgent::outBuffer, message);
				}
				else {
					Message message;
					send(MessageManagerAgent::outBuffer, message);
					cout << "GUILD NAME2222: " << message.data.id << endl;
				}
			}
			catch (exception error) {}
			try {
				dataPackage = receive(MessageManagerAgent::requestFetchBuffer, 50U);
				if (MessageManagerAgent::cache.contains(dataPackage.channelId + dataPackage.messageId)) {
					Message message = MessageManagerAgent::cache.at(dataPackage.channelId + dataPackage.messageId);
					MessageManagerAgent::cache.erase(dataPackage.channelId + dataPackage.messageId);
					message = fetchAsync(dataPackage.messageId, dataPackage.channelId, message).get();
					MessageManagerAgent::cache.insert(make_pair(dataPackage.channelId + dataPackage.messageId, message));
					send(MessageManagerAgent::outBuffer, message);
				}
				else {
					Message message = fetchAsync(dataPackage.messageId, dataPackage.channelId, message).get();
					MessageManagerAgent::cache.insert(make_pair(dataPackage.channelId + dataPackage.messageId, message));
					send(MessageManagerAgent::outBuffer, message);
				}
			}
			catch (exception error) {}
			Message message;
			while (MessageManagerAgent::messagesToInsert.try_pop(message)) {
				if (MessageManagerAgent::cache.contains(message.data.channelId + message.data.id)) {
					MessageManagerAgent::cache.erase(message.data.channelId + message.data.id);
					cout << message.data.id << endl;
				}
				MessageManagerAgent::cache.insert(make_pair(message.data.channelId + message.data.id, message));
			}
			done();
		}

	};

	class MessageManager: concurrent_unordered_map<string, Message>, public implements<MessageManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };
		MessageManager() {}
		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->threads = threadsNew;
		}

		task<Message> createMessageAsync(DiscordCoreInternal::CreateMessageData createMessageData, string channelId) {
			co_await resume_foreground(*this->threads->at(5).threadQueue.get());
			MessageManagerDataPackage dataPackage;
			dataPackage.channelId = channelId;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageData);
			MessageManagerAgent messageManagerAgent(this->agentResources, this, this->guild, this->threads);
			messageManagerAgent.start();
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			cout << "WERE HERE WERE HERE WERE HERE" << endl;
			Message message = receive(MessageManagerAgent::outBuffer);
			agent::wait(&messageManagerAgent);
			co_return message;
		}

		~MessageManager() {}
	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
	};
	map<string, Message> MessageManagerAgent::cache;
	unbounded_buffer<MessageManagerDataPackage>* MessageManagerAgent::requestFetchBuffer;
	unbounded_buffer<MessageManagerDataPackage>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<MessageManagerDataPackage>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message> MessageManagerAgent::messagesToInsert;
}
#endif
