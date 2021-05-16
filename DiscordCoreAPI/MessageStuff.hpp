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
		Message(DiscordCoreInternal::MessageData dataNew, Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, MessageManager* pMessageManagerNew) {
			this->guild = guildNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->messages = pMessageManagerNew;
			this->reactions = new ReactionManager(this->agentResources, this->guild);
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

		DiscordCoreInternal::HttpAgentResources agentResources;

	};

	class MessageManager: concurrent_unordered_map<string, Message>, public implements<MessageManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild{ nullptr };
		MessageManager() {}
		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
		}

		task<Message> createMessageAsync(DiscordCoreInternal::CreateMessageData createMessageData, string channelId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.relativePath = "/channels/" + channelId + "/messages";
			workload.content = DiscordCoreInternal::getCreateMessagePayload(createMessageData);
			DiscordCoreInternal::MessageData messageData;
			//DiscordCoreInternal::parseObject(jsonValue, &messageData);
			Message message(messageData, this->guild, this->agentResources, this);
			this->insert(make_pair(message.data.id, message));
			co_return message;
		}

		~MessageManager() {}
	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
	};
}
#endif
