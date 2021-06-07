// InputEventHandler.hpp - Header for the "input event handlermanager" class.
// June 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENT_HANDLER_
#define _INPUT_EVENT_HANDLER_

#include "pch.h"
#include "MessageStuff.hpp"
#include "InteractionManager.hpp"

namespace DiscordCoreAPI {

	class InputEventHandler {
	public:
		InputEventHandler() {}

		static void initialize(MessageManager* messagesNew,  DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ThreadContext threadContextNew){
			InputEventHandler::messages = messagesNew;
			InputEventHandler::agentResources = agentResourcesNew;
			InputEventHandler::discordCoreClient = discordCoreClientNew;
			InputEventHandler::threadContext = threadContextNew;
		}

		static task<InputEventData> respondToEvent(InputEventResponseData eventResponseData) {
			InputEventData eventData;
			if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE) {
				CreateFollowUpMessageData data;
				data.allowedMentions = eventResponseData.allowedMentions;
				data.components = eventResponseData.components;
				data.content = eventResponseData.content;
				data.embeds = eventResponseData.embeds;
				data.tts = eventResponseData.tts;
				data.applicationId = eventResponseData.applicationId;
				data.flags = eventResponseData.flags;
				data.interactionToken = eventResponseData.interactionToken;
				data.tts = eventResponseData.tts;
				MessageData messageData = InputEventHandler::interactions->createFollowUpMessageAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.messageData = messageData;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
				EditFollowUpMessageData data;
				data.allowedMentions = eventResponseData.allowedMentions;
				data.components = eventResponseData.components;
				data.content = eventResponseData.content;
				data.embeds = eventResponseData.embeds;
				data.tts = eventResponseData.tts;
				data.applicationId = eventResponseData.applicationId;
				data.flags = eventResponseData.flags;
				data.interactionToken = eventResponseData.interactionToken;
				data.tts = eventResponseData.tts;
				data.messageId = eventResponseData.messageId;
				MessageData messageData = InputEventHandler::interactions->editFollowUpMessageAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.messageData = messageData;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
				CreateInteractionResponseData data;
				data.data.allowedMentions = eventResponseData.allowedMentions;
				data.data.components = eventResponseData.components;
				data.data.content = eventResponseData.content;
				data.data.embeds = eventResponseData.embeds;
				data.data.tts = eventResponseData.tts;
				data.data.flags = 64;
				data.interactionId = eventResponseData.interactionId;
				data.type = eventResponseData.type;
				data.interactionToken = eventResponseData.interactionToken;
				InputEventHandler::interactions->createInteractionResponseAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT) {
				EditInteractionResponseData data;
				data.content = eventResponseData.content;
				data.allowedMentions = eventResponseData.allowedMentions;
				data.components = eventResponseData.components;
				data.embeds = eventResponseData.embeds;
				data.applicationId = eventResponseData.applicationId;
				data.interactionToken = eventResponseData.interactionToken;
				data.type = InteractionCallbackType::UpdateMessage;
				MessageData messageData = InputEventHandler::interactions->editInteractionResponseAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.messageData = messageData;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE) {
				CreateInteractionResponseData data;
				data.data.allowedMentions = eventResponseData.allowedMentions;
				data.data.components = eventResponseData.components;
				data.data.content = eventResponseData.content;
				data.data.embeds = eventResponseData.embeds;
				data.data.flags = eventResponseData.flags;
				data.data.tts = eventResponseData.tts;
				data.interactionId = eventResponseData.interactionId;
				data.interactionToken = eventResponseData.interactionToken;
				data.type = InteractionCallbackType::ChannelMessageWithSource;
				data.interactionToken = eventResponseData.interactionToken;
				MessageData messageData = InputEventHandler::interactions->createInteractionResponseAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.messageData = messageData;
				eventData.interactionData.channelId = eventResponseData.channelId;
				eventData.interactionData.message.id = eventResponseData.messageId;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED) {
				CreateDeferredInteractionResponseData data;
				data.interactionId = eventResponseData.interactionId;
				data.interactionToken = eventResponseData.interactionToken;
				data.type = InteractionCallbackType::DeferredChannelMessageWithSource;
				MessageData messageData = InputEventHandler::interactions->createDeferredInteractionResponseAsync(data).get();
				eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				eventData.messageData = messageData;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE) {
				ReplyMessageData data;
				data.allowedMentions = eventResponseData.allowedMentions;
				data.replyingToMessageData = InputEventHandler::messages->getMessageAsync({ .channelId = eventResponseData.channelId, .id = eventResponseData.messageId }).get().data;
				data.components = eventResponseData.components;
				data.content = eventResponseData.content;
				if (eventResponseData.embeds.size() > 0) {
					data.embed = eventResponseData.embeds.at(0);
				}
				data.channelId = eventResponseData.channelId;
				data.messageReference = eventResponseData.messageReference;
				data.tts = eventResponseData.tts;
				Message message = InputEventHandler::messages->replyAsync(data).get();
				eventData.eventType = InputEventType::REGULAR_MESSAGE;
				eventData.messageData = message.data;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
				EditMessageData data;
				data.allowedMentions = eventResponseData.allowedMentions;
				data.originalMessageData = InputEventHandler::messages->fetchAsync({ .channelId = eventResponseData.channelId, .id = eventResponseData.messageId }).get().data;
				for (auto value : eventResponseData.components) {
					data.components.push_back(value);
				}
				if (eventResponseData.embeds.size() > 0) {
					data.embed = eventResponseData.embeds.at(0);
				}
				data.content = eventResponseData.content;
				Message message = InputEventHandler::messages->editMessageAsync(data, data.originalMessageData.channelId, data.originalMessageData.id).get();
				eventData.eventType = InputEventType::REGULAR_MESSAGE;
				eventData.messageData = message.data;
				eventData.requesterId = eventResponseData.requesterId;
				eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (eventResponseData.inputEventResponseType == InputEventResponseType::DEFER_BUTTON_RESPONSE) {
			CreateInteractionResponseData createData;
			createData.interactionId = eventResponseData.interactionId;
			createData.interactionToken = eventResponseData.interactionToken;
			createData.type = InteractionCallbackType::DeferredUpdateMessage;
			InputEventHandler::interactions->createInteractionResponseAsync(createData).get();
			eventData.eventType = InputEventType::REGULAR_MESSAGE;
			eventData.requesterId = eventResponseData.requesterId;
			eventData.pDiscordCoreClient = InputEventHandler::discordCoreClient;
			}
			co_return eventData;
		}

		static task<void> deleteInputEventResponse(DiscordCoreInternal::InputEventData eventToDelete, unsigned int timeDelayNew = 0) {
			if (eventToDelete.eventType == DiscordCoreInternal::InputEventType::REGULAR_MESSAGE) {
				InputEventHandler::messages->deleteMessageAsync({ .channelId = eventToDelete.getChannelId(), .messageId = eventToDelete.getMessageId(), .timeDelay = timeDelayNew }).get();
			}
			else if (eventToDelete.eventType == DiscordCoreInternal::InputEventType::SLASH_COMMAND_INTERACTION) {
				DeleteInteractionData deleteInteractionData;
				deleteInteractionData.applicationId = eventToDelete.interactionData.applicationId;
				deleteInteractionData.interactionToken = eventToDelete.interactionData.token;
				InputEventHandler::interactions->deleteInteractionResponseAsync(deleteInteractionData, timeDelayNew).get();
			}
			co_return;
		}

		static task<void> deleteInputEventResponse(InputEventData eventToDelete, unsigned int timeDelayNew = 0) {
			if (eventToDelete.eventType == InputEventType::REGULAR_MESSAGE) {
				InputEventHandler::messages->deleteMessageAsync({ .channelId = eventToDelete.getChannelId(), .messageId = eventToDelete.getMessageId(), .timeDelay = timeDelayNew }).get();
			}
			else if (eventToDelete.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				DeleteInteractionData deleteInteractionData;
				deleteInteractionData.applicationId = eventToDelete.interactionData.applicationId;
				deleteInteractionData.interactionToken = eventToDelete.interactionData.token;
				InputEventHandler::interactions->deleteInteractionResponseAsync(deleteInteractionData, timeDelayNew).get();
			}
			co_return;
		}

	protected:
		static MessageManager* messages;
		static InteractionManager* interactions;
		static DiscordCoreClient* discordCoreClient;
		static DiscordCoreInternal::HttpAgentResources agentResources;
		static DiscordCoreInternal::ThreadContext threadContext;
	};
	MessageManager* InputEventHandler::messages;
	InteractionManager* InputEventHandler::interactions;
	DiscordCoreClient* InputEventHandler::discordCoreClient;
	DiscordCoreInternal::HttpAgentResources InputEventHandler::agentResources;
	DiscordCoreInternal::ThreadContext InputEventHandler::threadContext;


}
#endif
