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
#include "InteractionManager.hpp"

namespace DiscordCoreAPI {

	class MessageManager;

	class DiscordCoreClient;

	class Message {
	public:
		MessageData data;
		DiscordCoreClient* coreClient{ nullptr };

		Message() {};

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;
		friend class InteractionManager;

		Message(MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->initialize(dataNew, coreClientNew).get();
		}

		task<void> initialize(MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->data = dataNew;
			this->coreClient = coreClientNew;
			co_return;
		}

	};

	struct EditMessageData {
		string content = "";
		EmbedData embed;
		int flags = 0;
		vector<AttachmentData> attachments;
		AllowedMentionsData allowedMentions;
		MessageData originalMessageData;
		vector<ActionRowData> components;
	};

	struct CreateMessageData {
		AllowedMentionsData allowedMentions;
		string content = "";
		string channelId;
		string requesterId;
		EmbedData embed;
		MessageReferenceData messageReference;
		vector<ActionRowData> components;
		int nonce;
		bool tts = false;
	};

	struct ReplyMessageData {
		string content = "";
		bool tts = false;
		MessageData replyingToMessageData;
		EmbedData embed;
		AllowedMentionsData allowedMentions;
		MessageReferenceData messageReference;
		int nonce;
		string channelId;
		vector<ActionRowData> components;
		bool sendEphemeral;
	};

	struct SendDMData {
		string channelId;
		string userId;
		CreateMessageData messageData;
	};

	struct GetMessageData {
		string channelId;
		string id;
		string requesterId;
	};

	struct FetchMessagesData {
		string channelId;
		unsigned int limit;
		string beforeThisId;
		string afterThisId;
		string aroundThisId;
	};

	struct FetchMessageData {
		string requesterId;
		string channelId;
		string id;
	};

	struct DeleteMessageData {
		string channelId;
		string messageId;
		unsigned int timeDelay = 0;
	};

	struct DeleteMessagesBulkData {
		string channelId;
		unsigned int limit;
		string beforeThisId;
		string afterThisId;
		string aroundThisId;
	};
	
	class MessageManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class InteractionManager;

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* requestGetMultBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* requestDeleteMultBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteBuffer;
		static unbounded_buffer<vector<Message>>* outMultBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message>* messagesToInsert;
		static overwrite_buffer<map<string, Message>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			MessageManagerAgent::requestGetMultBuffer = new unbounded_buffer<DiscordCoreInternal::FetchMessagesData>;
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestDeleteMultBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>;
			MessageManagerAgent::requestPatchBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
			MessageManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
			MessageManagerAgent::messagesToInsert = new concurrent_queue<Message>;
			MessageManagerAgent::outMultBuffer = new unbounded_buffer<vector<Message>>;
			MessageManagerAgent::outBuffer = new unbounded_buffer<Message>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Message> getObjectAsync(DiscordCoreInternal::GetMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::getObjectAsync() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObjectAsync() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObjectAsync() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
			co_return messageNew;
		}

		task<vector<Message>> getObjectAsync(DiscordCoreInternal::FetchMessagesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGES;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			if (dataPackage.aroundThisId != ""){
				workload.relativePath += "?around=" + dataPackage.aroundThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.beforeThisId!= "") {
				workload.relativePath += "?before=" + dataPackage.beforeThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.afterThisId!= "") {
				workload.relativePath += "?after=" + dataPackage.afterThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else {
				if (dataPackage.limit != 0) {
					workload.relativePath += "?limit=" + to_string(dataPackage.limit);
				}
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::getObjectAsync() Error 02: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObjectAsync() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObjectAsync() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<Message> messagesVector;
			for (auto value : returnData.data) {
				MessageData messageData;
				DiscordCoreInternal::parseObject(value, &messageData);
				Message messageNew(messageData, this->coreClient);
				messagesVector.push_back(messageNew);
			}
			co_return messagesVector;
		}

		task<Message> patchObjectAsync(DiscordCoreInternal::PatchMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::patchObjectAsync() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObjectAsync() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObjectAsync() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObjectAsync() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectAsync() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObjectAsync() Error 03: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectAsync() Error 04: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
			co_return messageNew;
		}

		task<void> onDeleteAsync(DiscordCoreInternal::DeleteMessageData dataPackage) {
			co_await resume_foreground(*dataPackage.threadContext.threadQueue.get());
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::onDeleteAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::onDeleteAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::onDeleteAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		task<void> deleteObjectAsync(DiscordCoreInternal::DeleteMessageData dataPackage) {
			if (dataPackage.timeDelay > 0) {
				DispatcherQueueTimer timer = this->threads->at(10).threadQueue.get()->CreateTimer();
				timer.Interval(chrono::milliseconds(dataPackage.timeDelay));
				timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
					onDeleteAsync(dataPackage).get();
					timer.Stop();
					});
				timer.Start();
			}
			else {
				onDeleteAsync(dataPackage).get();
			}
			co_return;
		}

		task<void> postObjectAsync(DiscordCoreInternal::DeleteMessagesBulkData dataPackage) {
			co_await resume_foreground(*dataPackage.threadContext.threadQueue.get());
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObjecAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjecAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjecAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		void run() {
			try {
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
						if (cacheTemp.contains(dataPackageNew.channelId + dataPackageNew.messageId)) {
							Message message = cacheTemp.at(dataPackageNew.channelId + dataPackageNew.messageId);
							send(MessageManagerAgent::outBuffer, message);
						}
					}
				};
				DiscordCoreInternal::SendDMData dataPackageNewer;
				if (try_receive(MessageManagerAgent::requestPostDMBuffer, dataPackageNewer)) {
					Message message = this->postObjectAsync(dataPackageNewer).get();
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
						if (cacheTemp.contains(dataPackageGet.channelId + dataPackageGet.messageId)) {
							cacheTemp.erase(dataPackageGet.channelId + dataPackageGet.messageId);
						}
					}
					Message message = getObjectAsync(dataPackageGet).get();
					cacheTemp.insert(make_pair(dataPackageGet.channelId + dataPackageGet.messageId, message));
					send(MessageManagerAgent::outBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::PatchMessageData dataPackageGetNew;
				if (try_receive(MessageManagerAgent::requestPatchBuffer, dataPackageGetNew)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackageGet.channelId + dataPackageGet.messageId)) {
							cacheTemp.erase(dataPackageGet.channelId + dataPackageGet.messageId);
						}
					}
					Message message = patchObjectAsync(dataPackageGetNew).get();
					cacheTemp.insert(make_pair(dataPackageGet.channelId + dataPackageGet.messageId, message));
					send(MessageManagerAgent::outBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::FetchMessagesData dataPackageGetMultNew;
				if (try_receive(MessageManagerAgent::requestGetMultBuffer, dataPackageGetMultNew)) {
					vector<Message> messages = getObjectAsync(dataPackageGetMultNew).get();
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						for (auto value : messages) {
							if (cacheTemp.contains(value.data.channelId + value.data.id)) {
								cacheTemp.erase(value.data.channelId + value.data.id);
							}
							cacheTemp.insert(make_pair(value.data.channelId + value.data.id, value));
						}
					}
					send(MessageManagerAgent::outMultBuffer, messages);
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
				DiscordCoreInternal::DeleteMessagesBulkData dataPackageDeleteMult;
				if (try_receive(MessageManagerAgent::requestDeleteMultBuffer, dataPackageDeleteMult)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						for (auto value : dataPackageDeleteMult.messageIds) {
							if (cacheTemp.contains(dataPackageDeleteMult.channelId + value)) {
								cacheTemp.erase(dataPackageDeleteMult.channelId + value);
							}
						}						
					}
					postObjectAsync(dataPackageDeleteMult).get();
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				MessageData messageData;
				Message message(messageData, this->coreClient);
				while (MessageManagerAgent::messagesToInsert->try_pop(message)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(message.data.channelId + message.data.id)) {
							cacheTemp.erase(message.data.channelId + message.data.id);
						}
					}
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					asend(MessageManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class MessageManager {
	public:

		task<Message> replyAsync(ReplyMessageData replyMessageData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = replyMessageData.channelId;
			DiscordCoreInternal::ReplyMessageData createMessageDataNew;
			createMessageDataNew.allowedMentions = replyMessageData.allowedMentions;
			createMessageDataNew.channelId = replyMessageData.channelId;
			for (auto value : replyMessageData.components) {
				createMessageDataNew.components.push_back(value);
			}
			createMessageDataNew.content = replyMessageData.content;
			createMessageDataNew.embed = replyMessageData.embed;
			createMessageDataNew.messageReference = replyMessageData.messageReference;
			createMessageDataNew.nonce = replyMessageData.nonce;
			createMessageDataNew.replyingToMessageData = replyMessageData.replyingToMessageData;
			createMessageDataNew.tts = replyMessageData.tts;
			createMessageDataNew.content = replyMessageData.content;
			dataPackage.content = DiscordCoreInternal::getReplyMessagePayload(createMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::replyAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> sendDMAsync(SendDMData sendDMData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::SendDMData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.userId = sendDMData.userId;
			dataPackage.channelId = sendDMData.channelId;
			DiscordCoreInternal::CreateMessageData createMessageDataNew;
			createMessageDataNew.allowedMentions = sendDMData.messageData.allowedMentions;
			createMessageDataNew.content = sendDMData.messageData.content;
			for (auto value : sendDMData.messageData.components) {
				createMessageDataNew.components.push_back(value);
			}
			createMessageDataNew.embed = sendDMData.messageData.embed;
			createMessageDataNew.messageReference = sendDMData.messageData.messageReference;
			createMessageDataNew.nonce = sendDMData.messageData.nonce;
			createMessageDataNew.tts = sendDMData.messageData.tts;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostDMBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::sendDMAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> createMessageAsync(CreateMessageData createMessageData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = createMessageData.channelId;
			DiscordCoreInternal::CreateMessageData createMessageDataNew;
			createMessageDataNew.allowedMentions = createMessageData.allowedMentions;
			createMessageDataNew.content = createMessageData.content;
			for (auto value : createMessageData.components) {
				createMessageDataNew.components.push_back(value);
			}
			createMessageDataNew.embed = createMessageData.embed;
			createMessageDataNew.messageReference = createMessageData.messageReference;
			createMessageDataNew.nonce = createMessageData.nonce;
			createMessageDataNew.tts = createMessageData.tts;
			dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::createMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> editMessageAsync(EditMessageData editMessageData, string channelId, string messageId) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PatchMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.channelId = channelId;
			dataPackage.messageId = messageId;
			DiscordCoreInternal::EditMessageData editMessageDataNew;
			editMessageDataNew.allowedMentions = editMessageData.allowedMentions;
			for (auto value : editMessageData.attachments) {
				editMessageDataNew.attachments.push_back(value);
			}
			for (auto value : editMessageData.components) {
				editMessageDataNew.components.push_back(value);
			}
			editMessageDataNew.content = editMessageData.content;
			editMessageDataNew.embed = editMessageData.embed;
			editMessageDataNew.flags = editMessageData.flags;
			dataPackage.content = DiscordCoreInternal::getEditMessagePayload(editMessageDataNew);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(6).scheduler);
			send(MessageManagerAgent::requestPatchBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::editMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<void> deleteMessageAsync(DeleteMessageData deleteMessageData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(9);
			dataPackage.channelId = deleteMessageData.channelId;
			dataPackage.messageId = deleteMessageData.messageId;
			dataPackage.timeDelay = deleteMessageData.timeDelay;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::deleteMessageAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteMessasgeBulkAsync(DeleteMessagesBulkData deleteMessagesBulkData) {
			vector<Message> messages = this->fetchMessagesAsync({ .channelId = deleteMessagesBulkData.channelId, .limit = deleteMessagesBulkData.limit, .beforeThisId = deleteMessagesBulkData.beforeThisId, .afterThisId = deleteMessagesBulkData.afterThisId, .aroundThisId = deleteMessagesBulkData.aroundThisId, }).get();
			vector<string> messageIds;
			for (auto value : messages) {
				messageIds.push_back(value.data.id);
			}
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteMessagesBulkData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(9);
			dataPackage.channelId = deleteMessagesBulkData.channelId;
			dataPackage.messageIds = messageIds;
			dataPackage.content = DiscordCoreInternal::getDeleteMessagesBulkPayload(dataPackage);
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteMultBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::deleteMessagesBulkAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<Message> getMessageAsync(GetMessageData getMessageData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = getMessageData.channelId;
			dataPackage.messageId = getMessageData.id;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestGetBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::getMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<vector<Message>> fetchMessagesAsync(FetchMessagesData fetchMessagesData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchMessagesData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = fetchMessagesData.channelId;
			dataPackage.afterThisId = fetchMessagesData.afterThisId;
			dataPackage.aroundThisId = fetchMessagesData.aroundThisId;
			dataPackage.beforeThisId = fetchMessagesData.beforeThisId;
			dataPackage.limit = fetchMessagesData.limit;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestGetMultBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::fetchMessagesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Message> messageVector;
			try_receive(MessageManagerAgent::outMultBuffer, messageVector);
			co_return messageVector;
		}

		task<Message> fetchAsync(FetchMessageData fetchMessageData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.channelId = fetchMessageData.channelId;
			dataPackage.messageId = fetchMessageData.id;
			MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestFetchBuffer, dataPackage);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<void> insertMessageAsync(Message message) {
			MessageManagerAgent messageManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			MessageManagerAgent::messagesToInsert->push(message);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::insertMessageAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class Channel;
		friend class Guild;
		friend class DiscordCoreClient;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}
	};
	overwrite_buffer<map<string, Message>> MessageManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* MessageManagerAgent::requestPostDMBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* MessageManagerAgent::requestDeleteMultBuffer;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* MessageManagerAgent::requestGetMultBuffer;
	unbounded_buffer<vector<Message>>* MessageManagerAgent::outMultBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message>* MessageManagerAgent::messagesToInsert;
}
#endif