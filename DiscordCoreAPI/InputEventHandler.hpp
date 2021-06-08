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

		static task<InputEventData> respondToEvent(InputEventResponseData dataPackage) {
			InputEventData dataPackageNewer;
			if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE) {
				CreateFollowUpMessageData dataPackageNew;
				dataPackageNew.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.components = dataPackage.components;
				dataPackageNew.content = dataPackage.content;
				dataPackageNew.embeds = dataPackage.embeds;
				dataPackageNew.tts = dataPackage.tts;
				dataPackageNew.applicationId = dataPackage.applicationId;
				dataPackageNew.flags = dataPackage.flags;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				MessageData messageData = InputEventHandler::interactions->createFollowUpMessageAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.messageData = messageData;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
				EditFollowUpMessageData dataPackageNew;
				dataPackageNew.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.components = dataPackage.components;
				dataPackageNew.content = dataPackage.content;
				dataPackageNew.embeds = dataPackage.embeds;
				dataPackageNew.tts = dataPackage.tts;
				dataPackageNew.applicationId = dataPackage.applicationId;
				dataPackageNew.flags = dataPackage.flags;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				dataPackageNew.messageId = dataPackage.messageId;
				MessageData messageData = InputEventHandler::interactions->editFollowUpMessageAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.messageData = messageData;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
				CreateInteractionResponseData dataPackageNew;
				dataPackageNew.data.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.data.components = dataPackage.components;
				dataPackageNew.data.content = dataPackage.content;
				dataPackageNew.data.embeds = dataPackage.embeds;
				dataPackageNew.data.tts = dataPackage.tts;
				dataPackageNew.data.flags = 64;
				dataPackageNew.interactionId = dataPackage.interactionId;
				dataPackageNew.type = dataPackage.type;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				InputEventHandler::interactions->createInteractionResponseAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED) {
				CreateDeferredInteractionResponseData dataPackageNew;
				dataPackageNew.interactionId = dataPackage.interactionId;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				dataPackageNew.type = InteractionCallbackType::DeferredChannelMessageWithSource;
				MessageData messageData = InputEventHandler::interactions->createDeferredInteractionResponseAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.messageData = messageData;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE) {
				CreateInteractionResponseData dataPackageNew;
				dataPackageNew.data.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.data.components = dataPackage.components;
				dataPackageNew.data.content = dataPackage.content;
				dataPackageNew.data.embeds = dataPackage.embeds;
				dataPackageNew.data.flags = dataPackage.flags;
				dataPackageNew.data.tts = dataPackage.tts;
				dataPackageNew.interactionId = dataPackage.interactionId;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				dataPackageNew.type = InteractionCallbackType::ChannelMessageWithSource;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				MessageData messageData = InputEventHandler::interactions->createInteractionResponseAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.messageData = messageData;
				dataPackageNewer.interactionData.channelId = dataPackage.channelId;
				dataPackageNewer.interactionData.message.id = dataPackage.messageId;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT) {
				EditInteractionResponseData dataPackageNew;
				dataPackageNew.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.applicationId = dataPackage.applicationId;
				dataPackageNew.components = dataPackage.components;
				dataPackageNew.content = dataPackage.finalContent;
				dataPackageNew.embeds = dataPackage.embeds;
				dataPackageNew.flags = dataPackage.flags;
				dataPackageNew.interactionToken = dataPackage.interactionToken;
				dataPackageNew.type = InteractionCallbackType::UpdateMessage;
				MessageData messageData = InputEventHandler::interactions->editInteractionResponseAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
				dataPackageNewer.messageData = messageData;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE) {
				ReplyMessageData dataPackageNew;
				dataPackageNew.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.replyingToMessageData = InputEventHandler::messages->getMessageAsync({ .channelId = dataPackage.channelId, .id = dataPackage.messageId }).get().data;
				dataPackageNew.components = dataPackage.components;
				if (dataPackage.embeds.size() > 0) {
					dataPackageNew.embed = dataPackage.embeds.at(0);
				}
				dataPackageNew.channelId = dataPackage.channelId;
				dataPackageNew.messageReference = dataPackage.messageReference;
				dataPackageNew.content = dataPackage.finalContent;
				dataPackageNew.tts = dataPackage.tts;
				Message message = InputEventHandler::messages->replyAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
				dataPackageNewer.messageData = message.data;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
				EditMessageData dataPackageNew;
				dataPackageNew.allowedMentions = dataPackage.allowedMentions;
				dataPackageNew.channelId = dataPackage.channelId;
				dataPackageNew.messageId = dataPackage.messageId;
				dataPackageNew.originalMessageData = InputEventHandler::messages->fetchAsync({ .channelId = dataPackage.channelId, .id = dataPackage.messageId }).get().data;
				dataPackageNew.content = dataPackage.content;
				dataPackageNew.flags = dataPackage.flags;
				for (auto value : dataPackage.components) {
					dataPackageNew.components.push_back(value);
				}
				if (dataPackage.embeds.size() > 0) {
					dataPackageNew.embed = dataPackage.embeds.at(0);
				}
				dataPackageNew.content = dataPackage.content;
				Message message = InputEventHandler::messages->editMessageAsync(dataPackageNew).get();
				dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
				dataPackageNewer.messageData = message.data;
				dataPackageNewer.requesterId = dataPackage.requesterId;
				dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::DEFER_BUTTON_RESPONSE) {
			CreateInteractionResponseData createData;
			createData.interactionId = dataPackage.interactionId;
			createData.data.allowedMentions = dataPackage.allowedMentions;
			createData.data.embeds = dataPackage.embeds;
			createData.data.content = dataPackage.finalContent;
			createData.data.components = dataPackage.components;
			createData.data.flags = dataPackage.flags;
			createData.data.tts = dataPackage.tts;
			createData.interactionToken = dataPackage.interactionToken;
			createData.type = InteractionCallbackType::DeferredUpdateMessage;
			InputEventHandler::interactions->createInteractionResponseAsync(createData).get();
			dataPackageNewer.eventType = InputEventType::BUTTON_INTERACTION;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			}
			co_return dataPackageNewer;
		}

		static task<void> deleteInputEventResponse(DiscordCoreInternal::InputEventData eventToDelete, unsigned int timeDelayNew = 0) {
			if (eventToDelete.eventType == DiscordCoreInternal::InputEventType::REGULAR_MESSAGE) {
				InputEventHandler::messages->deleteMessageAsync({ .channelId = eventToDelete.getChannelId(), .messageId = eventToDelete.getMessageId(), .timeDelay = timeDelayNew }).get();
			}
			else if (eventToDelete.eventType == DiscordCoreInternal::InputEventType::SLASH_COMMAND_INTERACTION) {
				DeleteInteractionResponseData dataPackageNewer;
				dataPackageNewer.applicationId = eventToDelete.interactionData.applicationId;
				dataPackageNewer.interactionToken = eventToDelete.interactionData.token;
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventHandler::interactions->deleteInteractionResponseAsync(dataPackageNewer).get();
			}
			co_return;
		}

		static task<void> deleteInputEventResponse(InputEventData eventToDelete, unsigned int timeDelayNew = 0) {
			if (eventToDelete.eventType == InputEventType::REGULAR_MESSAGE) {
				InputEventHandler::messages->deleteMessageAsync({ .channelId = eventToDelete.getChannelId(), .messageId = eventToDelete.getMessageId(), .timeDelay = timeDelayNew }).get();
			}
			else if (eventToDelete.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				DeleteInteractionResponseData dataPackageNewer;
				dataPackageNewer.applicationId = eventToDelete.interactionData.applicationId;
				dataPackageNewer.interactionToken = eventToDelete.interactionData.token;
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventHandler::interactions->deleteInteractionResponseAsync(dataPackageNewer).get();
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
