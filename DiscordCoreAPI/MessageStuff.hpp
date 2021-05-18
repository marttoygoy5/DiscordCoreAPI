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
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	class Guild;

	class MessageManager;

	class MessageManagerAgent;

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
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;

		vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		Message(DiscordCoreInternal::MessageData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, MessageManager* pMessageManagerNew, vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->guild = guildNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->messages = pMessageManagerNew;
			this->threads = threadsNew;
			this->reactions = new ReactionManager(this->agentResources, this->guild, this->threads, this->data.channelId, this->data.id);
		}

	};

	struct PostMessageData {
		string channelId;
		string content;
	};

	struct GetMessageData {
		string channelId;
		string id;
	};

	struct DeleteMessageData {
		unsigned int timeDelay = 0;
		string channelId;
		string messageId;
	};

	class MessageManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class Message;

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message> messagesToInsert;
		static map<string, Message> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::MessageManager* pMessageManager;
		Guild* pGuild;

		MessageManagerAgent(vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::MessageManager* pMessageManagerNew, Guild* pGuildNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pMessageManager = pMessageManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
			MessageManagerAgent::outBuffer = new unbounded_buffer<Message>;
			co_return;
		}

		task<Message> getObjectAsync(DiscordCoreInternal::GetMessageData getObjectData) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + getObjectData.channelId + "/messages/" + getObjectData.id;
			DiscordCoreInternal::HttpRequestAgent requestAgent(getObjectData.agentResources, getObjectData.threadContext.scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->pGuild, this->agentResources, this->pMessageManager, this->threads);
			co_return messageNew;
		}

		task<Message> postObjectAsync(DiscordCoreInternal::PostMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.content;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			cout << "TEST 1" << endl<<jsonValue << endl;
			agent::wait(&requestAgent);
			cout << "TEST 2" << endl << jsonValue << endl;
			DiscordCoreInternal::MessageData messageData;
			cout << "TEST 3" << endl << jsonValue << endl;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			cout << "TEST 4" << endl << jsonValue << endl;
			Message messageNew(messageData, this->pGuild, this->agentResources, this->pMessageManager, this->threads);
			cout << "TEST 5" << endl << jsonValue << endl;
			co_return messageNew;
		}

		task<void> onDelete(DiscordCoreInternal::DeleteMessageData dataPackage) {
			co_await resume_foreground(*dataPackage.threadContext.threadQueue.get());
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
		}

		task<void> deleteObjectAsync(DiscordCoreInternal::DeleteMessageData dataPackage) {
			DispatcherQueueTimer timer = this->threads->at(10).threadQueue.get()->CreateTimer();
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
				DiscordCoreInternal::PostMessageData dataPackage = receive(MessageManagerAgent::requestPostBuffer, 1U);
				Message message = this->postObjectAsync(dataPackage).get();
				cout << message.data.content << endl;
				send(MessageManagerAgent::outBuffer, message);
				cout << message.data.content << endl;
			}
			catch (exception error) {}
			try {
				DiscordCoreInternal::GetMessageData dataPackageNew = receive(MessageManagerAgent::requestGetBuffer, 1U);
				Message message = MessageManagerAgent::cache.at(dataPackageNew.channelId + dataPackageNew.id);
				send(MessageManagerAgent::outBuffer, message);
			}
			catch (exception error) {}
			try {
				DiscordCoreInternal::GetMessageData dataPackageGet = receive(MessageManagerAgent::requestFetchBuffer, 1U);
				if (MessageManagerAgent::cache.contains(dataPackageGet.channelId + dataPackageGet.id)) {
					MessageManagerAgent::cache.erase(dataPackageGet.channelId + dataPackageGet.id);
				}
				Message message = getObjectAsync(dataPackageGet).get();
				MessageManagerAgent::cache.insert(make_pair(dataPackageGet.channelId + dataPackageGet.id, message));
				send(MessageManagerAgent::outBuffer, message);
			}
			catch (exception error) {}
			try {
				DiscordCoreInternal::DeleteMessageData dataPackageDelete = receive(MessageManagerAgent::requestDeleteBuffer, 1U);
				MessageManagerAgent::cache.erase(dataPackageDelete.channelId + dataPackageDelete.messageId);
				deleteObjectAsync(dataPackageDelete).get();
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
			DiscordCoreInternal::PostMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(6);
			dataPackage.channelId = channelId;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageData);
			MessageManagerAgent messageManagerAgent(this->threads, dataPackage.agentResources, this, this->guild, this->threads->at(5).scheduler);
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			messageManagerAgent.start();
			Message message = receive(MessageManagerAgent::outBuffer);
			MessageManagerAgent::cache.insert(make_pair(message.data.channelId + message.data.id, message));
			cout << message.data.content << endl;
			agent::wait(&messageManagerAgent);
			cout << message.data.content << endl;
			co_return message;
		}

		task<void> deleteMessageAsync(DeleteMessageData deleteMessageData) {
			DiscordCoreInternal::DeleteMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(9);
			dataPackage.channelId = deleteMessageData.channelId;
			dataPackage.messageId = deleteMessageData.messageId;
			dataPackage.timeDelay = deleteMessageData.timeDelay;
			cout << "DELETING DELETING DELETING!" << endl;
			MessageManagerAgent messageManagerAgent(this->threads, dataPackage.agentResources, this, this->guild, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteBuffer, dataPackage);
			cout << "DELETING DELETING DELETING222!" << endl;
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			cout << "DELETING DELETING DELETING3333!" << endl;
			co_return;
		}

		task<Message> fetchAsync(GetMessageData getMessageData) {
			DiscordCoreInternal::GetMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(4);
			dataPackage.channelId = getMessageData.channelId;
			dataPackage.id = getMessageData.id;
			MessageManagerAgent messageManagerAgent(this->threads, dataPackage.agentResources, this, this->guild, this->threads->at(3).scheduler);
			send(MessageManagerAgent::requestFetchBuffer, dataPackage);
			messageManagerAgent.start();
			Message message = receive(MessageManagerAgent::outBuffer);
			agent::wait(&messageManagerAgent);
			co_return message;
		}

	protected:
		friend class Channel;

		DiscordCoreInternal::HttpAgentResources agentResources;
		vector<DiscordCoreInternal::ThreadContext>* threads;

		MessageManager() {}
		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew, vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->threads = threadsNew;
		}
	};
	map<string, Message> MessageManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message> MessageManagerAgent::messagesToInsert;
}
#endif
