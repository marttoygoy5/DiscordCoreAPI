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

	class MessageManagerAgent;

	struct DeleteMessageData {
		string channelId;
		string messageId;
		unsigned int timeDelay = 0;
		DiscordCoreInternal::ThreadContext threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
	};

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
			this->reactions = new ReactionManager(this->agentResources, this->guild, this->threads, this->data.channelId, this->data.id);
		}

	};

	struct PostMessageData {
		string content;
		string channelId;
	};

	struct GetMessageData {
		string channelId;
		string id;
		DiscordCoreInternal::ThreadContext threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
	};

	class MessageManagerAgent : public agent {
	protected:
		friend struct DiscordCoreClient;
		friend class MessageManager;
		friend class Message;

		static unbounded_buffer<GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<GetMessageData>* requestGetBuffer;
		static unbounded_buffer<PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DeleteMessageData>* requestDeleteBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message> messagesToInsert;
		static map<string, Message> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::MessageManager* pMessageManager;
		Guild* pGuild;

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::MessageManager* pMessageManagerNew, Guild* pGuildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew)
			:agent(*threadsNew->at(0).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pMessageManager = pMessageManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<PostMessageData>;
			MessageManagerAgent::requestDeleteBuffer = new unbounded_buffer<DeleteMessageData>;
			MessageManagerAgent::outBuffer = new unbounded_buffer<Message>;
			co_return;
		}

		task<Message> getObjectAsync(GetMessageData getObjectData, Message message) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + getObjectData.channelId + "/messages/" + getObjectData.id;
			DiscordCoreInternal::HttpRequestAgent requestAgent(getObjectData.agentResources, getObjectData.threadContext.scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::MessageData messageData = message.data;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->pGuild, this->agentResources, this->pMessageManager, this->threads);
			co_return messageNew;
		}

		task<Message> postObjectAsync(string channelId, string content) {
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

		task<void> onDelete(DeleteMessageData dataPackage) {
			co_await resume_foreground(*dataPackage.threadContext.threadQueue.get());
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent  requestAgent = DiscordCoreInternal::HttpRequestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
		}

		task<void> deleteObjectAsync(DeleteMessageData dataPackage) {
			DispatcherQueueTimer timer = this->threads->at(0).threadQueue->CreateTimer();
			timer.Interval(chrono::milliseconds(dataPackage.timeDelay));
			timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
				onDelete(dataPackage).get();
				timer.Stop();
				});
			timer.Start();

			co_return;
		}

		void run() {
			try {
				PostMessageData dataPackage;
				dataPackage = receive(MessageManagerAgent::requestPostBuffer, 1U);
				Message message = this->postObjectAsync(dataPackage.channelId, dataPackage.content).get();
				send(MessageManagerAgent::outBuffer, message);
			}
			catch (exception error) {}
			try {
				GetMessageData dataPackage = receive(MessageManagerAgent::requestGetBuffer, 1U);
				if (MessageManagerAgent::cache.contains(dataPackage.channelId + dataPackage.id)) {
					Message message = MessageManagerAgent::cache.at(dataPackage.channelId + dataPackage.id);
					send(MessageManagerAgent::outBuffer, message);
				}
				else {
					Message message;
					send(MessageManagerAgent::outBuffer, message);
				}
			}
			catch (exception error) {}
			try {
				GetMessageData dataPackage = receive(MessageManagerAgent::requestFetchBuffer, 1U);
				if (MessageManagerAgent::cache.contains(dataPackage.channelId + dataPackage.id)) {
					Message message = MessageManagerAgent::cache.at(dataPackage.channelId + dataPackage.id);
					MessageManagerAgent::cache.erase(dataPackage.channelId + dataPackage.id);
					message = getObjectAsync(dataPackage, message).get();
					MessageManagerAgent::cache.insert(make_pair(dataPackage.channelId + dataPackage.id, message));
					send(MessageManagerAgent::outBuffer, message);
				}
				else {
					Message message = getObjectAsync(dataPackage, message).get();
					MessageManagerAgent::cache.insert(make_pair(dataPackage.channelId + dataPackage.id, message));
					send(MessageManagerAgent::outBuffer, message);
				}
			}
			catch (exception error) {}
			try {
				DeleteMessageData dataPackage = receive(MessageManagerAgent::requestDeleteBuffer, 1U);
				if (MessageManagerAgent::cache.contains(dataPackage.channelId + dataPackage.messageId)) {
					MessageManagerAgent::cache.erase(dataPackage.channelId + dataPackage.messageId);
				}
				deleteObjectAsync(dataPackage).get();
			}
			catch (exception error) {}
			Message message;
			while (MessageManagerAgent::messagesToInsert.try_pop(message)) {
				if (MessageManagerAgent::cache.contains(message.data.channelId + message.data.id)) {
					MessageManagerAgent::cache.erase(message.data.channelId + message.data.id);
				}
				MessageManagerAgent::cache.insert(make_pair(message.data.channelId + message.data.id, message));
			}
			done();
		}

	};

	class MessageManager : concurrent_unordered_map<string, Message>, public implements<MessageManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };

		task<Message> createMessageAsync(DiscordCoreInternal::CreateMessageData createMessageData, string channelId) {
			co_await resume_foreground(*this->threads->at(5).threadQueue.get());
			PostMessageData dataPackage;
			dataPackage.channelId = channelId;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageData);
			MessageManagerAgent messageManagerAgent(this->agentResources, this, this->guild, this->threads);
			messageManagerAgent.start();
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			Message message = receive(MessageManagerAgent::outBuffer);
			MessageManagerAgent::cache.insert(make_pair(message.data.channelId + message.data.id, message));
			agent::wait(&messageManagerAgent);
			co_return message;
		}

		task<void> deleteMessageAsync(DeleteMessageData deleteMessageData) {
			co_await resume_foreground(*this->threads->at(7).threadQueue.get());
			DeleteMessageData dataPackage;
			dataPackage.channelId = deleteMessageData.channelId;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.messageId = deleteMessageData.messageId;
			dataPackage.timeDelay = deleteMessageData.timeDelay;
			MessageManagerAgent messageManagerAgent(this->agentResources, this, this->guild, this->threads);
			messageManagerAgent.start();
			send(MessageManagerAgent::requestDeleteBuffer, dataPackage);
			agent::wait(&messageManagerAgent);
			co_return;
		}

		task<Message> fetchMessageAsync(GetMessageData getMessageData) {
			co_await resume_foreground(*this->threads->at(3).threadQueue.get());
			getMessageData.agentResources = this->agentResources;
			getMessageData.threadContext = this->threads->at(3);
			GetMessageData dataPackage = getMessageData;
			MessageManagerAgent messageManagerAgent(this->agentResources, this, this->guild, this->threads);
			messageManagerAgent.start();
			send(MessageManagerAgent::requestFetchBuffer, dataPackage);
			Message message = receive(MessageManagerAgent::outBuffer);
			agent::wait(&messageManagerAgent);
			co_return message;
		}

	protected:
		friend class Channel;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		MessageManager() {}
		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->threads = threadsNew;
		}
	};
	map<string, Message> MessageManagerAgent::cache;
	unbounded_buffer<GetMessageData>* MessageManagerAgent::requestFetchBuffer;
	unbounded_buffer<GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<PostMessageData>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<DeleteMessageData>* MessageManagerAgent::requestDeleteBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message> MessageManagerAgent::messagesToInsert;
}
#endif
