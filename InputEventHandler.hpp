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

		static void initialize(MessageManager* messagesNew, DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ThreadContext threadContextNew) {
			InputEventHandler::messages = messagesNew;
			InputEventHandler::agentResources = agentResourcesNew;
			InputEventHandler::discordCoreClient = discordCoreClientNew;
			InputEventHandler::threadContext = threadContextNew;
		}

		static task<InputEventData> respondToEvent(CreateFollowUpMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			MessageData messageData = InputEventHandler::interactions->createFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditFollowUpMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			MessageData messageData = InputEventHandler::interactions->editFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			co_await mainThread;
			co_return dataPackageNewer;
		}		

		static task<InputEventData> respondToEvent(CreateInteractionResponseData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			InputEventHandler::interactions->createInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			InputEventHandler::interactions->createDeferredInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_DEFERRED;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditInteractionResponseData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			MessageData messageData = InputEventHandler::interactions->editInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventHandler::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			InteractionPackageData interactionPackage;
			interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			CreateInteractionResponseData newData(interactionPackage);
			newData.data = dataPackage.data;
			newData.data.flags = 64;
			newData.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			newData.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			newData.type = dataPackage.type;
			InputEventHandler::interactions->createInteractionResponseAsync(newData).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(ReplyMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			Message message = InputEventHandler::messages->replyAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.requesterId = dataPackage.replyingToMessageData.author.id;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			Message message = InputEventHandler::messages->editMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
			co_await mainThread;
			co_return dataPackageNewer;
		}

		static task<void> respondToEvent(DeferButtonResponseData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			ButtonInteractionData newData;
			newData.token = dataPackage.interactionPackage.interactionToken;
			newData.id = dataPackage.interactionPackage.interactionId;
			newData.applicationId = dataPackage.interactionPackage.applicationId;
			newData.type = InteractionType::MessageComponent;
			CreateInteractionResponseData dataPackageNew(newData);
			dataPackageNew.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			dataPackageNew.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNew.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			dataPackageNew.type = InteractionCallbackType::DeferredUpdateMessage;
			InputEventHandler::interactions->createInteractionResponseAsync(dataPackageNew).get();
			co_await mainThread;
			co_return;
		}

		static task<void> deleteInputEventResponse(InputEventData dataPackage, unsigned int timeDelayNew = 0) {
			if ((dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE)|| (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT)) {
				InputEventHandler::messages->deleteMessageAsync({ .channelId = dataPackage.getChannelId(),.messageId = dataPackage.getMessageId(), .timeDelay = timeDelayNew }).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT){
				DeleteFollowUpMessageData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventHandler::interactions->deleteFollowUpMessageAsync(dataPackageNewer).get();
			}
			else {
				DeleteInteractionResponseData dataPackageNewer(dataPackage);
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
