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
		DiscordCoreClient* discordCoreClient{ nullptr };

		Message() {};

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;
		friend class InteractionManager;

		Message(MessageData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->initialize(dataNew, discordCoreClientNew).get();
		}

		task<void> initialize(MessageData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
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
		string channelId;
		string messageId;
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
		bool deletePinned;
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

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* requestGetMultMessagesBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* requestDeleteMultMessagesBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteMessageBuffer;
		static unbounded_buffer<vector<Message>>* outMultMessagesBuffer;
		static unbounded_buffer<Message>* outMessageBuffer;
		static concurrent_queue<Message>* messagesToInsert;
		static overwrite_buffer<map<string, Message>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			MessageManagerAgent::requestGetMultMessagesBuffer = new unbounded_buffer<DiscordCoreInternal::FetchMessagesData>;
			MessageManagerAgent::requestFetchMessageBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetMessageBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostMessageBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMMessageBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestDeleteMultMessagesBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>;
			MessageManagerAgent::requestPatchMessageBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
			MessageManagerAgent::requestDeleteMessageBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
			MessageManagerAgent::messagesToInsert = new concurrent_queue<Message>;
			MessageManagerAgent::outMultMessagesBuffer = new unbounded_buffer<vector<Message>>;
			MessageManagerAgent::outMessageBuffer = new unbounded_buffer<Message>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Message> getObjectDataAsync(DiscordCoreInternal::GetMessageData dataPackage) {
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
			Message messageNew(messageData, this->discordCoreClient);
			co_return messageNew;
		}

		task<vector<Message>> getObjectDataAsync(DiscordCoreInternal::FetchMessagesData dataPackage) {
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
				Message messageNew(messageData, this->discordCoreClient);
				messagesVector.push_back(messageNew);
			}
			co_return messagesVector;
		}

		task<Message> patchObjectDataAsync(DiscordCoreInternal::PatchMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = dataPackage.finalContent;
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
			Message messageNew(messageData, this->discordCoreClient);
			co_return messageNew;
		}

		task<Message> postObjectDataAsync(DiscordCoreInternal::PostMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.finalContent;
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
			Message messageNew(messageData, this->discordCoreClient);
			co_return messageNew;
		}

		task<Message> postObjectDataAsync(DiscordCoreInternal::SendDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.finalContent;
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
			Message messageNew(messageData, this->discordCoreClient);
			co_return messageNew;
		}

		task<void> onDeleteDataAsync(DiscordCoreInternal::DeleteMessageData dataPackage) {
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

		task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteMessageData dataPackage) {
			if (dataPackage.timeDelay > 0) {
				DispatcherQueueTimer timer = this->threads->at(10).threadQueue.get()->CreateTimer();
				timer.Interval(chrono::milliseconds(dataPackage.timeDelay));
				timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
					onDeleteDataAsync(dataPackage).get();
					timer.Stop();
					});
				timer.Start();
			}
			else {
				onDeleteDataAsync(dataPackage).get();
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
				DiscordCoreInternal::PostMessageData dataPackage01;
				if (try_receive(MessageManagerAgent::requestPostMessageBuffer, dataPackage01)) {
					Message message = this->postObjectDataAsync(dataPackage01).get();
					map<string, Message> cacheTemp;
					try_receive(MessageManagerAgent::cache, cacheTemp);
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				};
				DiscordCoreInternal::GetMessageData dataPackage02;
				if (try_receive(MessageManagerAgent::requestGetMessageBuffer, dataPackage02)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId + dataPackage02.messageId)) {
							Message message = cacheTemp.at(dataPackage02.channelId + dataPackage02.messageId);
							send(MessageManagerAgent::outMessageBuffer, message);
						}
					}
				};
				DiscordCoreInternal::SendDMData dataPackage03;
				if (try_receive(MessageManagerAgent::requestPostDMMessageBuffer, dataPackage03)) {
					Message message = this->postObjectDataAsync(dataPackage03).get();
					map<string, Message> cacheTemp;
					try_receive(MessageManagerAgent::cache, cacheTemp);
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				};
				DiscordCoreInternal::GetMessageData dataPackage04;
				if (try_receive(MessageManagerAgent::requestFetchMessageBuffer, dataPackage04)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage04.channelId + dataPackage04.messageId)) {
							cacheTemp.erase(dataPackage04.channelId + dataPackage04.messageId);
						}
					}
					Message message = getObjectDataAsync(dataPackage04).get();
					cacheTemp.insert(make_pair(dataPackage04.channelId + dataPackage04.messageId, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::PatchMessageData dataPackage05;
				if (try_receive(MessageManagerAgent::requestPatchMessageBuffer, dataPackage05)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage05.channelId + dataPackage05.messageId)) {
							cacheTemp.erase(dataPackage05.channelId + dataPackage05.messageId);
						}
					}
					Message message = patchObjectDataAsync(dataPackage05).get();
					cacheTemp.insert(make_pair(dataPackage05.channelId + dataPackage05.messageId, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::FetchMessagesData dataPackage06;
				if (try_receive(MessageManagerAgent::requestGetMultMessagesBuffer, dataPackage06)) {
					vector<Message> messages = getObjectDataAsync(dataPackage06).get();
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						for (auto value : messages) {
							if (cacheTemp.contains(value.data.channelId + value.data.id)) {
								cacheTemp.erase(value.data.channelId + value.data.id);
							}
							cacheTemp.insert(make_pair(value.data.channelId + value.data.id, value));
						}
					}
					send(MessageManagerAgent::outMultMessagesBuffer, messages);
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::DeleteMessageData dataPackage07;
				if (try_receive(MessageManagerAgent::requestDeleteMessageBuffer, dataPackage07)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage07.channelId + dataPackage07.messageId)) {
							cacheTemp.erase(dataPackage07.channelId + dataPackage07.messageId);
						}
					}
					deleteObjectDataAsync(dataPackage07).get();
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::DeleteMessagesBulkData dataPackage08;
				if (try_receive(MessageManagerAgent::requestDeleteMultMessagesBuffer, dataPackage08)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						for (auto value : dataPackage08.messageIds) {
							if (cacheTemp.contains(dataPackage08.channelId + value)) {
								cacheTemp.erase(dataPackage08.channelId + value);
							}
						}						
					}
					postObjectAsync(dataPackage08).get();
					asend(MessageManagerAgent::cache, cacheTemp);
				}
				MessageData messageData;
				Message message(messageData, this->discordCoreClient);
				while (MessageManagerAgent::messagesToInsert->try_pop(message)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.size() >= 1000) {
							cacheTemp.erase(cacheTemp.end());
						}
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

		task<Message> replyAsync(ReplyMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackage.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			dataPackageNew.channelId = dataPackage.channelId;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			dataPackageNew.embed = dataPackage.embed;
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.replyingToMessageData = dataPackage.replyingToMessageData;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.content = dataPackage.content;
			dataPackageNew.finalContent = DiscordCoreInternal::getReplyMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::replyAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> sendDMAsync(SendDMData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::SendDMData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions = dataPackage.messageData.allowedMentions;
			dataPackageNew.content = dataPackage.messageData.content;
			for (auto value : dataPackage.messageData.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.embed = dataPackage.messageData.embed;
			dataPackageNew.messageReference = dataPackage.messageData.messageReference;
			dataPackageNew.nonce = dataPackage.messageData.nonce;
			dataPackageNew.tts = dataPackage.messageData.tts;
			dataPackageNew.finalContent = DiscordCoreInternal::getCreateMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostDMMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::sendDMAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> createMessageAsync(CreateMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.embed = dataPackage.embed;
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.finalContent = DiscordCoreInternal::getCreateMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::createMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> editMessageAsync(EditMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PatchMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(7);
			dataPackageNew.channelId = dataPackage.originalMessageData.channelId;
			dataPackageNew.messageId = dataPackage.originalMessageData.id;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			for (auto value : dataPackage.attachments) {
				dataPackageNew.attachments.push_back(value);
			}
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			dataPackageNew.embed = dataPackage.embed;
			dataPackageNew.flags = dataPackage.flags;
			dataPackageNew.finalContent = DiscordCoreInternal::getEditMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(6).scheduler);
			send(MessageManagerAgent::requestPatchMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::editMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<void> deleteMessageAsync(DeleteMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.timeDelay = dataPackage.timeDelay;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::deleteMessageAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteMessasgeBulkAsync(DeleteMessagesBulkData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			vector<Message> messages = this->fetchMessagesAsync({ .channelId = dataPackage.channelId, .limit = dataPackage.limit, .beforeThisId = dataPackage.beforeThisId, .afterThisId = dataPackage.afterThisId, .aroundThisId = dataPackage.aroundThisId, }).get();
			vector<string> messageIds;
			for (auto value : messages) {
				if (dataPackage.deletePinned) {
					messageIds.push_back(value.data.id);
				}
				else {
					if (!value.data.pinned) {
						messageIds.push_back(value.data.id);
					}
				}
			}
			DiscordCoreInternal::DeleteMessagesBulkData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageIds = messageIds;
			dataPackageNew.content = DiscordCoreInternal::getDeleteMessagesBulkPayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(MessageManagerAgent::requestDeleteMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::deleteMessagesBulkAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<Message> getMessageAsync(GetMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestGetMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::getMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<vector<Message>> fetchMessagesAsync(FetchMessagesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.afterThisId = dataPackage.afterThisId;
			dataPackageNew.aroundThisId = dataPackage.aroundThisId;
			dataPackageNew.beforeThisId = dataPackage.beforeThisId;
			dataPackageNew.limit = dataPackage.limit;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestGetMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::fetchMessagesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Message> messageVector;
			try_receive(MessageManagerAgent::outMultMessagesBuffer, messageVector);
			co_return messageVector;
		}

		task<Message> fetchAsync(FetchMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(MessageManagerAgent::requestFetchMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<void> insertMessageAsync(Message message) {
			MessageManagerAgent messageManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			MessageManagerAgent::messagesToInsert->push(message);
			messageManagerAgent.start();
			agent::wait(&messageManagerAgent);
			exception error;
			while (messageManagerAgent.getError(error)) {
				cout << "MessageManager::insertMessageAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeMessageAsync(string channelId, string messageId) {
			apartment_context mainThread;
			co_await resume_background();
			map<string, Message> cache;;
			try_receive(MessageManagerAgent::cache, cache);
			if (cache.contains(channelId + messageId)) {
				cache.erase(channelId + messageId);
			}
			asend(MessageManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

	protected:
		friend class Channel;
		friend class Guild;
		friend class DiscordCoreClient;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}
	};
	overwrite_buffer<map<string, Message>> MessageManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestFetchMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* MessageManagerAgent::requestPostDMMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* MessageManagerAgent::requestDeleteMultMessagesBuffer;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* MessageManagerAgent::requestGetMultMessagesBuffer;
	unbounded_buffer<vector<Message>>* MessageManagerAgent::outMultMessagesBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outMessageBuffer;
	concurrent_queue<Message>* MessageManagerAgent::messagesToInsert;
}
#endif