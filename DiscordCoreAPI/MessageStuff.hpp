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
		com_ptr<MessageManager> messages{ nullptr };
		com_ptr<ReactionManager> reactions{ nullptr };
		Guild* guild{ nullptr };
		Message() {}
		Message(DiscordCoreInternal::MessageData dataNew, Guild* guildNew,  DiscordCoreInternal::HttpAgentPointers pointersNew, com_ptr<MessageManager> pMessageManagerNew) {
			this->guild = guildNew;
			this->data = dataNew;
			this->pointers = pointersNew;
			this->messages = pMessageManagerNew;
			this->reactions = make_self<ReactionManager>(this->pointers, this->guild);
		}

		task<Message> editMessageAsync(DiscordCoreInternal::EditMessageData editMessageData) {
			Message message;
			co_return message;
		}

		task<void> deleteMessageAsync(unsigned int timeDelay = 0) {
			co_return;
		}

		~Message() {}

	protected:

		DiscordCoreInternal::HttpAgentPointers pointers;

	};

	class MessageManager: concurrent_unordered_map<string, Message>, public implements<MessageManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };
		MessageManager() {}
		MessageManager(DiscordCoreInternal::HttpAgentPointers pointersNew, Guild* guildNew) {
			this->pointers = pointersNew;
			this->guild = guildNew;
		}

		task<Message> createMessageAsync(DiscordCoreInternal::CreateMessageData createMessageData, string channelId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.relativePath = "/channels/" + channelId + "/messages";
			workload.content = DiscordCoreInternal::getCreateMessagePayload(createMessageData);
			send(&pointers.pPOSTAgent->workSubmissionBuffer, workload);
			json jsonValue = receive(pointers.pPOSTAgent->workReturnBuffer);
			DiscordCoreInternal::MessageData messageData;
			DiscordCoreInternal::parseObject(jsonValue, &messageData);
			com_ptr<MessageManager> pMessageManager;
			pMessageManager.attach(this);
			Message message(messageData, this->guild, this->pointers, pMessageManager);
			this->insert(make_pair(message.data.id, message));
			co_return message;
		}

		~MessageManager() {}
	protected:
		DiscordCoreInternal::HttpAgentPointers pointers;
	};
}
#endif
