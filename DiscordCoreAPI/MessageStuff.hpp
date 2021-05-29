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

	class DiscordCoreClient;

	class Message {
	public:
		DiscordCoreInternal::MessageData data;
		DiscordCoreClient* coreClient{ nullptr };

		Message() {};

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;

		Message(DiscordCoreInternal::MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->initialize(dataNew, coreClientNew).get();
		}

		task<void> initialize(DiscordCoreInternal::MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->data = dataNew;
			this->coreClient = coreClientNew;
			co_return;
		}

	};

	struct EmbedData {
		EmbedData setTitle(string titleNew) {
			this->title = titleNew;
			return *this;
		}
		EmbedData setAuthor(string authorName, string authorAvatarURL = "") {
			this->author.name = authorName;
			this->author.iconUrl = authorAvatarURL;
			return *this;
		}
		EmbedData setImage(string imageURL) {
			this->image.url = imageURL;
			return *this;
		}
		EmbedData setThumbnail(string thumbnailURL) {
			this->thumbnail.url = thumbnailURL;
			return *this;
		}
		EmbedData setColor(unsigned int colorRed, unsigned int colorGreen,unsigned int colorBlue) {
			this->color[0] = colorRed;
			this->color[1] = colorGreen;
			this->color[2] = colorBlue;
			return *this;
		}
		EmbedData setDescription(string descriptionNew) {
			this->description = descriptionNew;
			return *this;
		}
		EmbedData setFooter(string footerText, string footerIconURLText = "") {
			this->footer.text = footerText;
			this->footer.iconUrl = footerIconURLText;
			return *this;
		}
		EmbedData setTimeStamp(string timeStamp) {
			this->timestamp = timeStamp;
			return *this;
		}
		EmbedData addField(string name, string value, bool Inline = true) {
			DiscordCoreInternal::EmbedFieldData embedFieldData;
			embedFieldData.name = name;
			embedFieldData.Inline = Inline;
			embedFieldData.value = value;
			this->fields.push_back(embedFieldData);
			return *this;
		}
		string title;
		string type;
		string description;
		string url;
		string timestamp;
		unsigned int color[3] = { 255, 255, 255 };
		DiscordCoreInternal::EmbedFooterData footer;
		DiscordCoreInternal::EmbedImageData image;
		DiscordCoreInternal::EmbedThumbnailData thumbnail;
		DiscordCoreInternal::EmbedVideoData video;
		DiscordCoreInternal::EmbedProviderData provider;
		DiscordCoreInternal::EmbedAuthorData author;
		vector<DiscordCoreInternal::EmbedFieldData> fields;
		int actualColor() {
			if (this->color[0] > 255) {
				this->color[0] = 255;
			}
			else if (this->color[0] < 0) {
				this->color[0] = 0;
			}
			if (this->color[1] > 255) {
				this->color[1] = 255;
			}
			else if (this->color[1] < 0) {
				this->color[1] = 0;
			}
			if (this->color[2] > 255) {
				this->color[2] = 255;
			}
			else if (this->color[2] < 0) {
				this->color[2] = 0;
			}
			int colorValue = 65536 * this->color[0] + 256 * this->color[1] + this->color[2];
			return colorValue;
		};
		int actualColorVal;
	};

	struct EditMessageData {
		string content = "";
		EmbedData embed;
		int flags = 0;
		vector<DiscordCoreInternal::AttachmentData> attachments;
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
	};

	struct CreateMessageData {
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
		string content = "";
		string channelId;
		EmbedData embed;
		DiscordCoreInternal::MessageReferenceData messageReference;
		int nonce;
		bool tts = false;
	};

	struct ReplyMessageData {
		string content = "";
		bool tts = false;
		DiscordCoreInternal::MessageData replyingToMessageData;
		EmbedData embed;
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
		DiscordCoreInternal::MessageReferenceData messageReference;
		int nonce;
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

	struct FetchMessageData {
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

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchInteractionResponseData>* requestPatchInteractionBuffer;
		static unbounded_buffer<Message>* outBuffer;
		static concurrent_queue<Message> messagesToInsert;
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
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestPatchBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
			MessageManagerAgent::requestPatchInteractionBuffer = new unbounded_buffer<DiscordCoreInternal::PatchInteractionResponseData>;
			MessageManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
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
				cout << "MessageManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData,  this->coreClient);
			co_return messageNew;
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
				cout << "MessageManagerAgent::patchObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
			co_return messageNew;
		}

		task<Message> patchObjectAsync(DiscordCoreInternal::PatchInteractionResponseData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
			workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::patchObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::MessageData messageData;
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
			DiscordCoreInternal::MessageData messageData;
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
			DiscordCoreInternal::MessageData messageData;
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
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
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
				DiscordCoreInternal::PatchInteractionResponseData dataPackageGetNewInteraction;
				if (try_receive(MessageManagerAgent::requestPatchInteractionBuffer, dataPackageGetNewInteraction)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackageGetNewInteraction.channelId + dataPackageGetNewInteraction.messageId)) {
							cacheTemp.erase(dataPackageGetNewInteraction.channelId + dataPackageGetNewInteraction.messageId);
						}
					}
					Message message = patchObjectAsync(dataPackageGetNewInteraction).get();
					cacheTemp.insert(make_pair(dataPackageGetNewInteraction.channelId + dataPackageGetNewInteraction.messageId, message));
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
				Message message(messageData, this->coreClient);
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
			if (replyMessageData.replyingToMessageData.type == DiscordCoreInternal::MessageType::INTERACTION) {
				DiscordCoreInternal::EditInteractionResponseData editInteractionResponseData;
				editInteractionResponseData.content = replyMessageData.content;
				if (replyMessageData.embed.description != ""|| replyMessageData.embed.fields.at(0).value != "") {
					DiscordCoreInternal::EmbedData embedData;
					embedData.actualColorVal = replyMessageData.embed.actualColorVal;
					embedData.author = replyMessageData.embed.author;
					embedData.color[0] = replyMessageData.embed.color[0];
					embedData.color[1] = replyMessageData.embed.color[1];
					embedData.color[2] = replyMessageData.embed.color[2];
					embedData.description = replyMessageData.embed.description;
					embedData.fields = replyMessageData.embed.fields;
					embedData.fields = replyMessageData.embed.fields;
					embedData.footer= replyMessageData.embed.footer;
					embedData.image = replyMessageData.embed.image;
					embedData.provider= replyMessageData.embed.provider;
					embedData.thumbnail = replyMessageData.embed.thumbnail;
					embedData.timestamp = replyMessageData.embed.timestamp;
					embedData.title = replyMessageData.embed.title;
					embedData.type = replyMessageData.embed.type;
					embedData.url = replyMessageData.embed.url;
					embedData.video = replyMessageData.embed.video;
					editInteractionResponseData.embeds.push_back(embedData);
				}
				editInteractionResponseData.allowedMentions = replyMessageData.allowedMentions;
				DiscordCoreInternal::PatchInteractionResponseData dataPackage;
				dataPackage.agentResources = this->agentResources;
				dataPackage.channelId = replyMessageData.replyingToMessageData.channelId;
				dataPackage.messageId = replyMessageData.replyingToMessageData.interactionId;
				dataPackage.applicationId = replyMessageData.replyingToMessageData.applicationId;
				dataPackage.interactionToken = replyMessageData.replyingToMessageData.interactionToken;
				dataPackage.threadContext = this->threads->at(5);
				dataPackage.content = DiscordCoreInternal::getEditInteractionResponsePayload(editInteractionResponseData);
				MessageManagerAgent managerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(4).scheduler);
				send(managerAgent.requestPatchInteractionBuffer, dataPackage);
				managerAgent.start();
				agent::wait(&managerAgent);
				exception error;
				while (managerAgent.getError(error)) {
					cout << "MessageManager::replyAsync() Error: " << error.what() << endl << endl;
				}
				DiscordCoreInternal::MessageData messageData;
				Message messageNew(messageData, this->coreClient);
				try_receive(MessageManagerAgent::outBuffer, messageNew);
				co_await mainThread;
				co_return messageNew;
			}
			else {
				DiscordCoreInternal::PostMessageData dataPackage;
				dataPackage.agentResources = this->agentResources;
				dataPackage.threadContext = this->threads->at(3);
				dataPackage.channelId = replyMessageData.replyingToMessageData.channelId;
				DiscordCoreInternal::CreateMessageData createMessageDataNew;
				createMessageDataNew.allowedMentions = replyMessageData.allowedMentions;
				createMessageDataNew.content = replyMessageData.content;
				DiscordCoreInternal::EmbedData embedData;
				embedData.actualColorVal = replyMessageData.embed.actualColorVal;
				embedData.author = replyMessageData.embed.author;
				embedData.color[0] = replyMessageData.embed.color[0];
				embedData.color[1] = replyMessageData.embed.color[1];
				embedData.color[2] = replyMessageData.embed.color[2];
				embedData.description = replyMessageData.embed.description;
				embedData.fields = replyMessageData.embed.fields;
				embedData.fields = replyMessageData.embed.fields;
				embedData.footer = replyMessageData.embed.footer;
				embedData.image = replyMessageData.embed.image;
				embedData.provider = replyMessageData.embed.provider;
				embedData.thumbnail = replyMessageData.embed.thumbnail;
				embedData.timestamp = replyMessageData.embed.timestamp;
				embedData.title = replyMessageData.embed.title;
				embedData.type = replyMessageData.embed.type;
				embedData.url = replyMessageData.embed.url;
				embedData.video = replyMessageData.embed.video;
				createMessageDataNew.embed = embedData;
				createMessageDataNew.messageReference = replyMessageData.messageReference;
				createMessageDataNew.nonce = replyMessageData.nonce;
				createMessageDataNew.tts = replyMessageData.tts;
				createMessageDataNew.messageReference.channelId = replyMessageData.replyingToMessageData.channelId;
				createMessageDataNew.messageReference.failIfNotExists = false;
				createMessageDataNew.messageReference.messageId = replyMessageData.replyingToMessageData.id;
				createMessageDataNew.messageReference.guildId = replyMessageData.replyingToMessageData.guildId;
				createMessageDataNew.allowedMentions.repliedUser = true;
				dataPackage.content = DiscordCoreInternal::getCreateMessagePayload(createMessageDataNew);
				MessageManagerAgent messageManagerAgent(dataPackage.agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
				send(MessageManagerAgent::requestPostBuffer, dataPackage);
				messageManagerAgent.start();
				agent::wait(&messageManagerAgent);
				exception error;
				while (messageManagerAgent.getError(error)) {
					cout << "MessageManager::replyAsync() Error: " << error.what() << endl << endl;
				}
				DiscordCoreInternal::MessageData messageData;
				Message messageNew(messageData, this->coreClient);
				try_receive(MessageManagerAgent::outBuffer, messageNew);
				co_await mainThread;
				co_return messageNew;
			}
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
			DiscordCoreInternal::EmbedData embedData;
			embedData.actualColorVal = sendDMData.messageData.embed.actualColorVal;
			embedData.author = sendDMData.messageData.embed.author;
			embedData.color[0] = sendDMData.messageData.embed.color[0];
			embedData.color[1] = sendDMData.messageData.embed.color[1];
			embedData.color[2] = sendDMData.messageData.embed.color[2];
			embedData.description = sendDMData.messageData.embed.description;
			embedData.fields = sendDMData.messageData.embed.fields;
			embedData.fields = sendDMData.messageData.embed.fields;
			embedData.footer = sendDMData.messageData.embed.footer;
			embedData.image = sendDMData.messageData.embed.image;
			embedData.provider = sendDMData.messageData.embed.provider;
			embedData.thumbnail = sendDMData.messageData.embed.thumbnail;
			embedData.timestamp = sendDMData.messageData.embed.timestamp;
			embedData.title = sendDMData.messageData.embed.title;
			embedData.type = sendDMData.messageData.embed.type;
			embedData.url = sendDMData.messageData.embed.url;
			embedData.video = sendDMData.messageData.embed.video;
			createMessageDataNew.embed = embedData;
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
			DiscordCoreInternal::MessageData messageData;
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
			DiscordCoreInternal::EmbedData embedData;
			embedData.actualColorVal = createMessageData.embed.actualColorVal;
			embedData.author = createMessageData.embed.author;
			embedData.color[0] = createMessageData.embed.color[0];
			embedData.color[1] = createMessageData.embed.color[1];
			embedData.color[2] = createMessageData.embed.color[2];
			embedData.description = createMessageData.embed.description;
			embedData.fields = createMessageData.embed.fields;
			embedData.fields = createMessageData.embed.fields;
			embedData.footer = createMessageData.embed.footer;
			embedData.image = createMessageData.embed.image;
			embedData.provider = createMessageData.embed.provider;
			embedData.thumbnail = createMessageData.embed.thumbnail;
			embedData.timestamp = createMessageData.embed.timestamp;
			embedData.title = createMessageData.embed.title;
			embedData.type = createMessageData.embed.type;
			embedData.url = createMessageData.embed.url;
			embedData.video = createMessageData.embed.video;
			createMessageDataNew.embed = embedData;
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
			DiscordCoreInternal::MessageData messageData;
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
			editMessageDataNew.attachments = editMessageData.attachments;
			editMessageDataNew.content = editMessageData.content;
			DiscordCoreInternal::EmbedData embedData;
			embedData.actualColorVal = editMessageData.embed.actualColorVal;
			embedData.author = editMessageData.embed.author;
			embedData.color[0] = editMessageData.embed.color[0];
			embedData.color[1] = editMessageData.embed.color[1];
			embedData.color[2] = editMessageData.embed.color[2];
			embedData.description = editMessageData.embed.description;
			embedData.fields = editMessageData.embed.fields;
			embedData.fields = editMessageData.embed.fields;
			embedData.footer = editMessageData.embed.footer;
			embedData.image = editMessageData.embed.image;
			embedData.provider = editMessageData.embed.provider;
			embedData.thumbnail = editMessageData.embed.thumbnail;
			embedData.timestamp = editMessageData.embed.timestamp;
			embedData.title = editMessageData.embed.title;
			embedData.type = editMessageData.embed.type;
			embedData.url = editMessageData.embed.url;
			embedData.video = editMessageData.embed.video;
			editMessageDataNew.embed = embedData;
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
			DiscordCoreInternal::MessageData messageData;
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
			DiscordCoreInternal::MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
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
			DiscordCoreInternal::MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
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
			DiscordCoreInternal::MessageData messageData;
			Message messageNew(messageData, this->coreClient);
			try_receive(MessageManagerAgent::outBuffer, messageNew);
			co_await mainThread;
			co_return messageNew;
		}

		task<void> insertMessageAsync(Message message) {
			MessageManagerAgent messageManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			MessageManagerAgent::messagesToInsert.push(message);
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
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* MessageManagerAgent::requestPostDMBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchInteractionResponseData>* MessageManagerAgent::requestPatchInteractionBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message> MessageManagerAgent::messagesToInsert;
}
#endif


