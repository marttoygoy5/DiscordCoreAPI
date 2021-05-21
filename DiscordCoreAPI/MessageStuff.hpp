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

	class MessageManager;

	class Message {
	public:
		DiscordCoreInternal::MessageData data;
		MessageManager* messages;
		ReactionManager* reactions;

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;

		Message(DiscordCoreInternal::MessageData dataNew,  MessageManager* messagesNew, ReactionManager* reactionsNew) {
			this->initialize(dataNew, messagesNew, reactionsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::MessageData dataNew,  MessageManager* messagesNew, ReactionManager* reactionsNew) {
			this->data = dataNew;
			this->messages = messagesNew;
			this->reactions = reactionsNew;
			co_return;
		}

	};

	struct EditMessageData {
		string content;
		DiscordCoreInternal::EmbedData embed;
		int flags = 0;
		vector<DiscordCoreInternal::AttachmentData> attachments;
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
	};

	struct CreateMessageData {
		string content;
		int nonce;
		bool tts = false;
		DiscordCoreInternal::EmbedData embed;
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
		DiscordCoreInternal::MessageReferenceData messageReference;
	};

	struct SendDMData {
		string channelId;
		string userId;
		CreateMessageData messageData;
	};

	struct GetMessageData {
		string channelId;
		string id;
	};

	struct DeleteMessageData {
		string channelId;
		string messageId;
		unsigned int timeDelay = 0;
	};

	class MessageManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class Message;

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message> messagesToInsert;
		static overwrite_buffer<map<string, Message>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreAPI::MessageManager* messages{ nullptr };
		ReactionManager* reactions{ nullptr };

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreAPI::MessageManager* messagesNew, ReactionManager* reactionsNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->messages = messagesNew;
			this->reactions = reactionsNew;
		}

		static task<void> initialize() {
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestPatchBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
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
			Message messageNew(messageData, this->messages, this->reactions);
			co_return messageNew;
		}

		task<Message> patchObjectAsync(DiscordCoreInternal::PatchMessageData getObjectData) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + getObjectData.channelId + "/messages/" + getObjectData.messageId;
			workload.content = getObjectData.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(getObjectData.agentResources, getObjectData.threadContext.scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->messages, this->reactions);
			co_return messageNew;
		}

		task<Message> postObjectAsync(DiscordCoreInternal::PostMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.content;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			json jsonValue; 
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->messages, this->reactions);
			co_return messageNew;
		}

		task<Message> postObjectAsync(DiscordCoreInternal::SendDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.content;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message messageNew(messageData, this->messages, this->reactions);
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
			agent::wait(&requestAgent);
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
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
			DiscordCoreInternal::PostMessageData dataPackage;
			if (try_receive(MessageManagerAgent::requestPostBuffer, dataPackage)) {
				Message message = this->postObjectAsync(dataPackage).get();
				map<string, Message> cacheTemp;
				try_receive(MessageManagerAgent::cache, cacheTemp);
				cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
				send(MessageManagerAgent::outBuffer, message);
				asend(MessageManagerAgent::cache, cacheTemp);
			};
			DiscordCoreInternal::GetMessageData dataPackageNew;;
			if (try_receive(MessageManagerAgent::requestGetBuffer, dataPackageNew)) {
				map<string, Message> cacheTemp;
				if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackageNew.channelId + dataPackageNew.id)) {
						Message message = cacheTemp.at(dataPackageNew.channelId + dataPackageNew.id);
						send(MessageManagerAgent::outBuffer, message);
					}
				}
			};
			DiscordCoreInternal::SendDMData dataPackageNewer;
			if (try_receive(MessageManagerAgent::requestPostDMBuffer, dataPackageNewer)) {
				Message message = this->postObjectAsync(dataPackage).get();
				map<string, Message> cacheTemp;
				try_receive(MessageManagerAgent::cache, cacheTemp);
				cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
				send(MessageManagerAgent::outBuffer, message);
				asend(MessageManagerAgent::cache, cacheTemp);
			};
			DiscordCoreInternal::GetMessageData dataPackageGet;
			if (try_receive(MessageManagerAgent::requestFetchBuffer, dataPackageGet)) {
				map<string, Message> cacheTemp;
				if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackageGet.channelId + dataPackageGet.id)) {
						cacheTemp.erase(dataPackageGet.channelId + dataPackageGet.id);
					}
				}
				Message message = getObjectAsync(dataPackageGet).get();
				cacheTemp.insert(make_pair(dataPackageGet.channelId + dataPackageGet.id, message));
				send(MessageManagerAgent::outBuffer, message);
				asend(MessageManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::PatchMessageData dataPackageGetNew;
			if (try_receive(MessageManagerAgent::requestPatchBuffer, dataPackageGetNew)) {
				map<string, Message> cacheTemp;
				if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackageGet.channelId + dataPackageGet.id)) {
						cacheTemp.erase(dataPackageGet.channelId + dataPackageGet.id);
					}
				}
				Message message = patchObjectAsync(dataPackageGetNew).get();
				cacheTemp.insert(make_pair(dataPackageGet.channelId + dataPackageGet.id, message));
				send(MessageManagerAgent::outBuffer, message);
				asend(MessageManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::DeleteMessageData dataPackageDelete;
			if (try_receive(MessageManagerAgent::requestDeleteBuffer, dataPackageDelete)) {
				map<string, Message> cacheTemp;
				if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackageDelete.channelId + dataPackageDelete.messageId)) {
						cacheTemp.erase(dataPackageDelete.channelId + dataPackageDelete.messageId);
					}
				}				
				deleteObjectAsync(dataPackageDelete).get();
				asend(MessageManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this->messages, this->reactions);
			while (MessageManagerAgent::messagesToInsert.try_pop(message)) {
				map<string, Message> cacheTemp;
				if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(message.data.channelId + message.data.id)) {
						cacheTemp.erase(message.data.channelId + message.data.id);
					}
				}
				cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
				asend(MessageManagerAgent::cache, cacheTemp);
			}
			done();
		}
	};

	class MessageManager {
	public:

		task<Message> sendDMAsync(SendDMData sendDMData) {
			DiscordCoreInternal::SendDMData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = sendDMData.userId;
			dataPackage.channelId = sendDMData.channelId;
			DiscordCoreInternal::CreateMessageData createMessageDataNew;
			createMessageDataNew.allowedMentions = sendDMData.messageData.allowedMentions;
			createMessageDataNew.content = sendDMData.messageData.content;
			createMessageDataNew.embed = sendDMData.messageData.embed;
			createMessageDataNew.messageReference = sendDMData.messageData.messageReference;
			createMessageDataNew.nonce = sendDMData.messageData.nonce;
			createMessageDataNew.tts = sendDMData.messageData.tts;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostDMBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return message;
		}

		task<Message> createMessageAsync(CreateMessageData createMessageData, string channelId) {
			DiscordCoreInternal::PostMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.channelId = channelId;
			DiscordCoreInternal::CreateMessageData createMessageDataNew;
			createMessageDataNew.allowedMentions = createMessageData.allowedMentions;
			createMessageDataNew.content = createMessageData.content;
			createMessageDataNew.embed = createMessageData.embed;
			createMessageDataNew.messageReference = createMessageData.messageReference;
			createMessageDataNew.nonce = createMessageData.nonce;
			createMessageDataNew.tts = createMessageData.tts;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(6).scheduler);
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return message;
		}

		task<Message> editMessageAsync(EditMessageData editMessageData, string channelId, string messageId) {
			DiscordCoreInternal::PatchMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.channelId = channelId;
			dataPackage.messageId = messageId;
			DiscordCoreInternal::EditMessageData editMessageDataNew;
			editMessageDataNew.allowedMentions = editMessageData.allowedMentions;
			editMessageDataNew.attachments = editMessageData.attachments;
			editMessageDataNew.content = editMessageData.content;
			editMessageDataNew.embed = editMessageData.embed;
			editMessageDataNew.flags = editMessageData.flags;
			dataPackage.content = DiscordCoreInternal::getEditMessagePayload(editMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(6).scheduler);
			send(MessageManagerAgent::requestPatchBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return message;
		}

		task<void> deleteMessageAsync(DeleteMessageData deleteMessageData) {
			DiscordCoreInternal::DeleteMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(9);
			dataPackage.channelId = deleteMessageData.channelId;
			dataPackage.messageId = deleteMessageData.messageId;
			dataPackage.timeDelay = deleteMessageData.timeDelay;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return;
		}

		task<Message> getMessageAsync(GetMessageData getMessageData) {
			DiscordCoreInternal::GetMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getMessageData.channelId;
			dataPackage.id = getMessageData.id;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestGetBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return message;
		}

		task<Message> fetchAsync(GetMessageData getMessageData) {
			DiscordCoreInternal::GetMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getMessageData.channelId;
			dataPackage.id = getMessageData.id;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this, this->reactions, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestFetchBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			DiscordCoreInternal::MessageData messageData;
			Message message(messageData, this, this->reactions);
			try_receive(MessageManagerAgent::outBuffer, message);
			co_return message;
		}

	protected:
		friend class Channel;
		friend class Guild;
		friend class DiscordCoreClient;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		ReactionManager* reactions{ nullptr };

		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, ReactionManager*reactionsNew ) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->reactions = reactionsNew;
		}
	};
	overwrite_buffer<map<string, Message>> MessageManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* MessageManagerAgent::requestPostDMBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message> MessageManagerAgent::messagesToInsert;
}
#endif
