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
		DiscordCoreInternal::MessageData data;
		DiscordCoreClient* coreClient{ nullptr };

		Message() {};

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;
		friend class InteractionManager;

		Message(DiscordCoreInternal::MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->initialize(dataNew, coreClientNew).get();
		}

		task<void> initialize(DiscordCoreInternal::MessageData dataNew, DiscordCoreClient* coreClientNew) {
			this->data = dataNew;
			this->coreClient = coreClientNew;
			this->data.interactionId = this->data.interaction.id;
			co_return;
		}

	};

	struct AttachmentData {
		string id = "";
		string filename = "";
		string contentType = "";
		int size = 0;
		string url = "";
		string proxyUrl = "";
		int height = 0;
		int width = 0;
	};

	struct EmbedFooterData {
		string iconUrl;
		string text;
		string proxyIconUrl;
	};

	struct EmbedImageData {
		string url;
		string proxyUrl;
		int height;
		int width;
	};

	struct EmbedThumbnailData {
		string url;
		string proxyUrl;
		int height;
		int width;
	};

	struct EmbedVideoData {
		string url;
		string proxyUrl;
		int height;
		int width;
	};

	struct EmbedProviderData {
		string name;
		string url;
	};

	struct EmbedAuthorData {
		string name;
		string url;
		string iconUrl;
		string proxyIconUrl;
	};

	struct EmbedFieldData {
		string name;
		string value;
		bool Inline;
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
		EmbedData setColor(int colorRed, int colorGreen, int colorBlue) {
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
			EmbedFieldData embedFieldData;
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
		int color[3] = { 255, 255, 255 };
		EmbedFooterData footer;
		EmbedImageData image;
		EmbedThumbnailData thumbnail;
		EmbedVideoData video;
		EmbedProviderData provider;
		EmbedAuthorData author;
		vector<EmbedFieldData> fields;
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

	struct AllowedMentionsData {
		vector<string> parse;
		vector<string> roles;
		vector<string> users;
		bool repliedUser;
	};

	struct MessageReferenceData {
		string messageId;
		string channelId;
		string guildId;
		bool failIfNotExists;
	};

	enum class ComponentType {
		ActionRow = 1,
		Button = 2
	};

	enum class ButtonStyle {
		Primary = 1,
		Secondary = 2,
		Success = 3,
		Danger = 4,
		Link = 5
	};

	struct ComponentData {
		ComponentType type;
		ButtonStyle style;
		string label = "";
		DiscordCoreInternal::EmojiData emoji;
		string customId = "";
		string url = "";
		bool disabled = false;
	};

	struct ActionRowData {
		vector<ComponentData> components;
	};

	struct EditMessageData {
		string content = "";
		EmbedData embed;
		int flags = 0;
		vector<AttachmentData> attachments;
		AllowedMentionsData allowedMentions;
		DiscordCoreInternal::MessageData originalMessageData;
		vector<ActionRowData> components;
	};

	struct CreateMessageData {
		DiscordCoreInternal::AllowedMentionsData allowedMentions;
		string content = "";
		string channelId;
		string requesterId;
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
		AllowedMentionsData allowedMentions;
		MessageReferenceData messageReference;
		int nonce;
		vector<ActionRowData> components;
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
	
	class MessageManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class InteractionManager;

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteBuffer;
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
			MessageManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestPatchBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
			MessageManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
			MessageManagerAgent::messagesToInsert = new concurrent_queue<Message>;
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
			Message messageNew(messageData, this->coreClient);
			messageNew.data.requesterId = dataPackage.requesterId;
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
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->coreClient);
			messageNew.data.requesterId = dataPackage.requesterId;
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
			messageNew.data.requesterId = dataPackage.requesterId;
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
			if (replyMessageData.replyingToMessageData.messageType == DiscordCoreInternal::MessageTypeReal::INTERACTION) {
				InteractionResponseData editInteractionResponseData;
				editInteractionResponseData.data.content = replyMessageData.content;
				if (replyMessageData.embed.description != ""|| replyMessageData.embed.fields.size() > 0) {
					DiscordCoreInternal::EmbedData embedData;
					embedData.actualColorVal = replyMessageData.embed.actualColorVal;
					embedData.author.iconUrl = replyMessageData.embed.author.iconUrl;
					embedData.author.name = replyMessageData.embed.author.name;
					embedData.author.proxyIconUrl = replyMessageData.embed.author.proxyIconUrl;
					embedData.author.url = replyMessageData.embed.author.url;
					embedData.color[0] = replyMessageData.embed.color[0];
					embedData.color[1] = replyMessageData.embed.color[1];
					embedData.color[2] = replyMessageData.embed.color[2];
					embedData.description = replyMessageData.embed.description;
					for (auto& value : replyMessageData.embed.fields) {
						DiscordCoreInternal::EmbedFieldData field;
						field.value = value.value;
						field.name = value.name;
						field.Inline = value.Inline;
						embedData.fields.push_back(field);
					}
					embedData.footer.iconUrl = replyMessageData.embed.footer.iconUrl;
					embedData.footer.proxyIconUrl = replyMessageData.embed.footer.proxyIconUrl;
					embedData.footer.text = replyMessageData.embed.footer.text;
					embedData.image.height = replyMessageData.embed.image.height;
					embedData.image.proxyUrl = replyMessageData.embed.image.proxyUrl;
					embedData.image.url = replyMessageData.embed.image.url;
					embedData.image.width = replyMessageData.embed.image.width;
					embedData.provider.name = replyMessageData.embed.provider.name;
					embedData.provider.url = replyMessageData.embed.provider.url;
					embedData.thumbnail.height = replyMessageData.embed.thumbnail.height;
					embedData.thumbnail.proxyUrl = replyMessageData.embed.thumbnail.proxyUrl;
					embedData.thumbnail.url = replyMessageData.embed.thumbnail.url;
					embedData.thumbnail.width = replyMessageData.embed.thumbnail.width;
					embedData.timestamp = replyMessageData.embed.timestamp;
					embedData.title = replyMessageData.embed.title;
					embedData.type = replyMessageData.embed.type;
					embedData.url = replyMessageData.embed.url;
					embedData.video.height = replyMessageData.embed.video.height;
					embedData.video.proxyUrl = replyMessageData.embed.video.proxyUrl;
					embedData.video.url = replyMessageData.embed.video.url;
					embedData.video.width = replyMessageData.embed.video.width;
					editInteractionResponseData.data.embeds.push_back(embedData);
				}
				editInteractionResponseData.data.allowedMentions.parse = replyMessageData.allowedMentions.parse;
				editInteractionResponseData.data.allowedMentions.repliedUser = replyMessageData.allowedMentions.repliedUser;
				editInteractionResponseData.data.allowedMentions.roles = replyMessageData.allowedMentions.roles;
				editInteractionResponseData.data.allowedMentions.users = replyMessageData.allowedMentions.users;
				for (auto& value : replyMessageData.components) {
					DiscordCoreInternal::ActionRowData componentDatas;
					for (auto& valueNew :value.components) {
						DiscordCoreInternal::ComponentData componentData;
						componentData.customId = valueNew.customId;
						componentData.disabled = valueNew.disabled;
						componentData.emoji = valueNew.emoji;
						componentData.label = valueNew.label;
						componentData.style = (DiscordCoreInternal::ButtonStyle)valueNew.style;
						componentData.type = (DiscordCoreInternal::ComponentType)valueNew.type;
						componentData.url = valueNew.url;
						componentDatas.components.push_back(componentData);
					}
					editInteractionResponseData.data.components.push_back(componentDatas);
				}
				editInteractionResponseData.channelId = replyMessageData.replyingToMessageData.channelId;
				editInteractionResponseData.applicationId = replyMessageData.replyingToMessageData.applicationId;
				editInteractionResponseData.token = replyMessageData.replyingToMessageData.interactionToken;
				DiscordCoreInternal::MessageData messageData = InteractionManager::editInteractionResponseAsync(editInteractionResponseData).get();
				Message messageNew(messageData, this->coreClient);
				messageNew.data.messageType = DiscordCoreInternal::MessageTypeReal::INTERACTION;
				messageNew.data.applicationId = replyMessageData.replyingToMessageData.applicationId;
				messageNew.data.interactionToken = replyMessageData.replyingToMessageData.interactionToken;
				messageNew.data.requesterId = replyMessageData.replyingToMessageData.requesterId;
				co_await mainThread;
				co_return messageNew;
			}
			else {
				DiscordCoreInternal::PostMessageData dataPackage;
				dataPackage.agentResources = this->agentResources;
				dataPackage.threadContext = this->threads->at(3);
				dataPackage.channelId = replyMessageData.replyingToMessageData.channelId;
				DiscordCoreInternal::CreateMessageData createMessageDataNew;
				createMessageDataNew.allowedMentions.parse = replyMessageData.allowedMentions.parse;
				createMessageDataNew.allowedMentions.repliedUser = replyMessageData.allowedMentions.repliedUser;
				createMessageDataNew.allowedMentions.roles = replyMessageData.allowedMentions.roles;
				createMessageDataNew.allowedMentions.users = replyMessageData.allowedMentions.users;
				createMessageDataNew.content = replyMessageData.content;
				DiscordCoreInternal::EmbedData embedData;
				embedData.actualColorVal = replyMessageData.embed.actualColorVal;
				embedData.actualColorVal = replyMessageData.embed.actualColorVal;
				embedData.author.iconUrl = replyMessageData.embed.author.iconUrl;
				embedData.author.name = replyMessageData.embed.author.name;
				embedData.author.proxyIconUrl = replyMessageData.embed.author.proxyIconUrl;
				embedData.author.url = replyMessageData.embed.author.url;
				embedData.color[0] = replyMessageData.embed.color[0];
				embedData.color[1] = replyMessageData.embed.color[1];
				embedData.color[2] = replyMessageData.embed.color[2];
				embedData.description = replyMessageData.embed.description;
				for (auto& value : replyMessageData.embed.fields) {
					DiscordCoreInternal::EmbedFieldData field;
					field.value = value.value;
					field.name = value.name;
					field.Inline = value.Inline;
					embedData.fields.push_back(field);
				}
				embedData.footer.iconUrl = replyMessageData.embed.footer.iconUrl;
				embedData.footer.proxyIconUrl = replyMessageData.embed.footer.proxyIconUrl;
				embedData.footer.text = replyMessageData.embed.footer.text;
				embedData.image.height = replyMessageData.embed.image.height;
				embedData.image.proxyUrl = replyMessageData.embed.image.proxyUrl;
				embedData.image.url = replyMessageData.embed.image.url;
				embedData.image.width = replyMessageData.embed.image.width;
				embedData.provider.name = replyMessageData.embed.provider.name;
				embedData.provider.url = replyMessageData.embed.provider.url;
				embedData.thumbnail.height = replyMessageData.embed.thumbnail.height;
				embedData.thumbnail.proxyUrl = replyMessageData.embed.thumbnail.proxyUrl;
				embedData.thumbnail.url = replyMessageData.embed.thumbnail.url;
				embedData.thumbnail.width = replyMessageData.embed.thumbnail.width;
				embedData.timestamp = replyMessageData.embed.timestamp;
				embedData.title = replyMessageData.embed.title;
				embedData.type = replyMessageData.embed.type;
				embedData.url = replyMessageData.embed.url;
				embedData.video.height = replyMessageData.embed.video.height;
				embedData.video.proxyUrl = replyMessageData.embed.video.proxyUrl;
				embedData.video.url = replyMessageData.embed.video.url;
				embedData.video.width = replyMessageData.embed.video.width;
				createMessageDataNew.embed = embedData;
				createMessageDataNew.messageReference.channelId = replyMessageData.messageReference.channelId;
				createMessageDataNew.messageReference.failIfNotExists = replyMessageData.messageReference.failIfNotExists;
				createMessageDataNew.messageReference.guildId = replyMessageData.messageReference.guildId;
				createMessageDataNew.messageReference.messageId = replyMessageData.messageReference.messageId;
				createMessageDataNew.nonce = replyMessageData.nonce;
				createMessageDataNew.tts = replyMessageData.tts;
				createMessageDataNew.messageReference.channelId = replyMessageData.replyingToMessageData.channelId;
				createMessageDataNew.messageReference.failIfNotExists = false;
				createMessageDataNew.messageReference.messageId = replyMessageData.replyingToMessageData.id;
				createMessageDataNew.messageReference.guildId = replyMessageData.replyingToMessageData.guildId;
				createMessageDataNew.allowedMentions.repliedUser = true;
				for (auto& value : replyMessageData.components) {
					DiscordCoreInternal::ActionRowData componentDatas;
					for (auto& valueNew : value.components) {
						DiscordCoreInternal::ComponentData componentData;
						componentData.customId = valueNew.customId;
						componentData.disabled = valueNew.disabled;
						componentData.emoji = valueNew.emoji;
						componentData.label = valueNew.label;
						componentData.style = (DiscordCoreInternal::ButtonStyle)valueNew.style;
						componentData.type = (DiscordCoreInternal::ComponentType)valueNew.type;
						componentData.url = valueNew.url;
						componentDatas.components.push_back(componentData);
					}
					createMessageDataNew.components.push_back(componentDatas);
				}
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
				messageNew.data.requesterId = replyMessageData.replyingToMessageData.author.id;
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
			embedData.actualColorVal = sendDMData.messageData.embed.actualColorVal;
			embedData.actualColorVal = sendDMData.messageData.embed.actualColorVal;
			embedData.author.iconUrl = sendDMData.messageData.embed.author.iconUrl;
			embedData.author.name = sendDMData.messageData.embed.author.name;
			embedData.author.proxyIconUrl = sendDMData.messageData.embed.author.proxyIconUrl;
			embedData.author.url = sendDMData.messageData.embed.author.url;
			embedData.color[0] = sendDMData.messageData.embed.color[0];
			embedData.color[1] = sendDMData.messageData.embed.color[1];
			embedData.color[2] = sendDMData.messageData.embed.color[2];
			embedData.description = sendDMData.messageData.embed.description;
			for (auto& value : sendDMData.messageData.embed.fields) {
				DiscordCoreInternal::EmbedFieldData field;
				field.value = value.value;
				field.name = value.name;
				field.Inline = value.Inline;
				embedData.fields.push_back(field);
			}
			embedData.footer.iconUrl = sendDMData.messageData.embed.footer.iconUrl;
			embedData.footer.proxyIconUrl = sendDMData.messageData.embed.footer.proxyIconUrl;
			embedData.footer.text = sendDMData.messageData.embed.footer.text;
			embedData.image.height = sendDMData.messageData.embed.image.height;
			embedData.image.proxyUrl = sendDMData.messageData.embed.image.proxyUrl;
			embedData.image.url = sendDMData.messageData.embed.image.url;
			embedData.image.width = sendDMData.messageData.embed.image.width;
			embedData.provider.name = sendDMData.messageData.embed.provider.name;
			embedData.provider.url = sendDMData.messageData.embed.provider.url;
			embedData.thumbnail.height = sendDMData.messageData.embed.thumbnail.height;
			embedData.thumbnail.proxyUrl = sendDMData.messageData.embed.thumbnail.proxyUrl;
			embedData.thumbnail.url = sendDMData.messageData.embed.thumbnail.url;
			embedData.thumbnail.width = sendDMData.messageData.embed.thumbnail.width;
			embedData.video.height = sendDMData.messageData.embed.video.height;
			embedData.video.proxyUrl = sendDMData.messageData.embed.video.proxyUrl;
			embedData.video.url = sendDMData.messageData.embed.video.url;
			embedData.video.width = sendDMData.messageData.embed.video.width;
			embedData.timestamp = sendDMData.messageData.embed.timestamp;
			embedData.title = sendDMData.messageData.embed.title;
			embedData.type = sendDMData.messageData.embed.type;
			embedData.url = sendDMData.messageData.embed.url;
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
			embedData.author.iconUrl = createMessageData.embed.author.iconUrl;
			embedData.author.name = createMessageData.embed.author.name;
			embedData.author.proxyIconUrl = createMessageData.embed.author.proxyIconUrl;
			embedData.author.url = createMessageData.embed.author.url;
			embedData.color[0] = createMessageData.embed.color[0];
			embedData.color[1] = createMessageData.embed.color[1];
			embedData.color[2] = createMessageData.embed.color[2];
			embedData.description = createMessageData.embed.description;
			for (auto& value : createMessageData.embed.fields) {
				DiscordCoreInternal::EmbedFieldData field;
				field.value = value.value;
				field.name = value.name;
				field.Inline = value.Inline;
				embedData.fields.push_back(field);
			}
			embedData.footer.iconUrl = createMessageData.embed.footer.iconUrl;
			embedData.footer.proxyIconUrl = createMessageData.embed.footer.proxyIconUrl;
			embedData.footer.text = createMessageData.embed.footer.text;
			embedData.image.height = createMessageData.embed.image.height;
			embedData.image.proxyUrl = createMessageData.embed.image.proxyUrl;
			embedData.image.url = createMessageData.embed.image.url;
			embedData.image.width = createMessageData.embed.image.width;
			embedData.provider.name = createMessageData.embed.provider.name;
			embedData.provider.url = createMessageData.embed.provider.url;
			embedData.thumbnail.height = createMessageData.embed.thumbnail.height;
			embedData.thumbnail.proxyUrl = createMessageData.embed.thumbnail.proxyUrl;
			embedData.thumbnail.url = createMessageData.embed.thumbnail.url;
			embedData.thumbnail.width = createMessageData.embed.thumbnail.width;
			embedData.video.height = createMessageData.embed.video.height;
			embedData.video.proxyUrl = createMessageData.embed.video.proxyUrl;
			embedData.video.url = createMessageData.embed.video.url;
			embedData.video.width = createMessageData.embed.video.width;
			embedData.timestamp = createMessageData.embed.timestamp;
			embedData.title = createMessageData.embed.title;
			embedData.type = createMessageData.embed.type;
			embedData.url = createMessageData.embed.url;
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
			messageNew.data.requesterId = createMessageData.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		task<Message> editMessageAsync(EditMessageData editMessageData, string channelId, string messageId) {
			apartment_context mainThread;
			co_await resume_background();
			if (editMessageData.originalMessageData.messageType == DiscordCoreInternal::MessageTypeReal::INTERACTION) {
				InteractionResponseData editInteractionData;
				editInteractionData.type = DiscordCoreInternal::InteractionCallbackType::UpdateMessage;
				editInteractionData.data.allowedMentions.parse = editMessageData.allowedMentions.parse;
				editInteractionData.data.allowedMentions.repliedUser= editMessageData.allowedMentions.repliedUser;
				editInteractionData.data.allowedMentions.roles= editMessageData.allowedMentions.roles;
				editInteractionData.data.allowedMentions.users = editMessageData.allowedMentions.users;
				editInteractionData.applicationId = editMessageData.originalMessageData.applicationId;
				for (auto& value : editMessageData.components) {
					DiscordCoreInternal::ActionRowData rowData;
					for (auto& value2 : value.components) {
						DiscordCoreInternal::ComponentData component;
						component.customId = value2.customId;
						component.disabled = value2.disabled;
						component.emoji = value2.emoji;
						component.label = value2.label;
						component.style = (DiscordCoreInternal::ButtonStyle)value2.style;
						component.type = (DiscordCoreInternal::ComponentType)value2.type;
						component.url = value2.url;
						rowData.components.push_back(component);
					}
					editInteractionData.data.components.push_back(rowData);
				}
				editInteractionData.data.content = editMessageData.content;
				vector<DiscordCoreInternal::EmbedData> embeds;
				DiscordCoreInternal::EmbedData embed;
				for (auto& value : editMessageData.embed.fields) {
					DiscordCoreInternal::EmbedFieldData field;
					field.value = value.value;
					field.name = value.name;
					field.Inline = value.Inline;
					embed.fields.push_back(field);
				}
				embed.actualColorVal = editMessageData.embed.actualColorVal;
				embed.footer.iconUrl = editMessageData.embed.footer.iconUrl;
				embed.color[0] = editMessageData.embed.color[0];
				embed.color[1] = editMessageData.embed.color[1];
				embed.color[2] = editMessageData.embed.color[2];
				embed.author.iconUrl = editMessageData.embed.author.iconUrl;
				embed.author.name = editMessageData.embed.author.name;
				embed.author.proxyIconUrl = editMessageData.embed.author.proxyIconUrl;
				embed.author.url = editMessageData.embed.author.url;
				embed.footer.proxyIconUrl = editMessageData.embed.footer.proxyIconUrl;
				embed.footer.text = editMessageData.embed.footer.text;
				embed.image.height = editMessageData.embed.image.height;
				embed.image.proxyUrl = editMessageData.embed.image.proxyUrl;
				embed.image.url = editMessageData.embed.image.url;
				embed.image.width = editMessageData.embed.image.width;
				embed.provider.name = editMessageData.embed.provider.name;
				embed.provider.url = editMessageData.embed.provider.url;
				embed.thumbnail.height = editMessageData.embed.thumbnail.height;
				embed.thumbnail.proxyUrl = editMessageData.embed.thumbnail.proxyUrl;
				embed.thumbnail.url = editMessageData.embed.thumbnail.url;
				embed.thumbnail.width = editMessageData.embed.thumbnail.width;
				embed.video.height = editMessageData.embed.video.height;
				embed.video.proxyUrl = editMessageData.embed.video.proxyUrl;
				embed.video.url = editMessageData.embed.video.url;
				embed.video.width = editMessageData.embed.video.width;
				embed.timestamp = editMessageData.embed.timestamp;
				embed.title = editMessageData.embed.title;
				embed.type = editMessageData.embed.type;
				embed.url = editMessageData.embed.url;
				embed.description = editMessageData.embed.description;
				embeds.push_back(embed);
				editInteractionData.data.embeds = embeds;
				editInteractionData.userId = editMessageData.originalMessageData.author.id;
				editInteractionData.token = editMessageData.originalMessageData.interactionToken;
				editInteractionData.channelId = editMessageData.originalMessageData.channelId;
				editInteractionData.interactionId = editMessageData.originalMessageData.interaction.id;
				DiscordCoreInternal::MessageData messageData = InteractionManager::editInteractionResponseAsync(editInteractionData).get();
				Message message(messageData, this->coreClient);
				message.data.interactionToken = editMessageData.originalMessageData.interactionToken;
				message.data.applicationId = editMessageData.originalMessageData.applicationId;
				message.data.requesterId = editMessageData.originalMessageData.requesterId;
				co_return message;
			}
			DiscordCoreInternal::PatchMessageData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.channelId = channelId;
			dataPackage.messageId = messageId;
			DiscordCoreInternal::EditMessageData editMessageDataNew;
			editMessageDataNew.allowedMentions.parse = editMessageData.allowedMentions.parse;
			editMessageDataNew.allowedMentions.repliedUser = editMessageData.allowedMentions.repliedUser;
			editMessageDataNew.allowedMentions.roles = editMessageData.allowedMentions.roles;
			editMessageDataNew.allowedMentions.users = editMessageData.allowedMentions.users;
			for (auto& value : editMessageData.attachments) {
				DiscordCoreInternal::AttachmentData attachment;
				attachment.contentType = value.contentType;
				attachment.filename = value.filename;
				attachment.height = value.height;
				attachment.id = value.id;
				attachment.proxyUrl = value.proxyUrl;
				attachment.size = value.size;
				attachment.url = value.url;
				attachment.width = value.width;
				editMessageDataNew.attachments.push_back(attachment);
			}
			for (auto& value : editMessageData.components) {
				DiscordCoreInternal::ActionRowData componentDatas;
				for (auto& valueNew : value.components) {
					DiscordCoreInternal::ComponentData componentData;
					componentData.customId = valueNew.customId;
					componentData.disabled = valueNew.disabled;
					componentData.emoji = valueNew.emoji;
					componentData.label = valueNew.label;
					componentData.style = (DiscordCoreInternal::ButtonStyle)valueNew.style;
					componentData.type = (DiscordCoreInternal::ComponentType)valueNew.type;
					componentData.url = valueNew.url;
					componentDatas.components.push_back(componentData);
				}
				editMessageDataNew.components.push_back(componentDatas);
			}
			DiscordCoreInternal::EmbedData embedData;
			for (auto& value : editMessageData.embed.fields) {
				DiscordCoreInternal::EmbedFieldData field;
				field.value = value.value;
				field.name = value.name;
				field.Inline = value.Inline;
				embedData.fields.push_back(field);
			}
			embedData.actualColorVal = editMessageData.embed.actualColorVal;
			embedData.color[0] = editMessageData.embed.color[0];
			embedData.color[1] = editMessageData.embed.color[1];
			embedData.color[2] = editMessageData.embed.color[2];
			embedData.author.iconUrl = editMessageData.embed.author.iconUrl;
			embedData.author.name = editMessageData.embed.author.name;
			embedData.author.proxyIconUrl = editMessageData.embed.author.proxyIconUrl;
			embedData.author.url = editMessageData.embed.author.url;
			embedData.footer.iconUrl = editMessageData.embed.footer.iconUrl;
			embedData.footer.proxyIconUrl = editMessageData.embed.footer.proxyIconUrl;
			embedData.footer.text = editMessageData.embed.footer.text;
			embedData.image.height = editMessageData.embed.image.height;
			embedData.image.proxyUrl = editMessageData.embed.image.proxyUrl;
			embedData.image.url = editMessageData.embed.image.url;
			embedData.image.width = editMessageData.embed.image.width;
			embedData.provider.name = editMessageData.embed.provider.name;
			embedData.provider.url = editMessageData.embed.provider.url;
			embedData.thumbnail.height = editMessageData.embed.thumbnail.height;
			embedData.thumbnail.proxyUrl = editMessageData.embed.thumbnail.proxyUrl;
			embedData.thumbnail.url = editMessageData.embed.thumbnail.url;
			embedData.thumbnail.width = editMessageData.embed.thumbnail.width;
			embedData.video.height = editMessageData.embed.video.height;
			embedData.video.proxyUrl = editMessageData.embed.video.proxyUrl;
			embedData.video.url = editMessageData.embed.video.url;
			embedData.video.width = editMessageData.embed.video.width;
			embedData.timestamp = editMessageData.embed.timestamp;
			embedData.title = editMessageData.embed.title;
			embedData.type = editMessageData.embed.type;
			embedData.url = editMessageData.embed.url;
			embedData.description = editMessageData.embed.description;
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
			messageNew.data.requesterId = editMessageData.originalMessageData.requesterId;
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
			messageNew.data.requesterId = getMessageData.requesterId;
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
			messageNew.data.requesterId = fetchMessageData.requesterId;
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
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outBuffer;
	concurrent_queue<Message>* MessageManagerAgent::messagesToInsert;
}
#endif


