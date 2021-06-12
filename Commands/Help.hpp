// Help.hpp - Header for the "help" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELP_
#define _HELP_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Help : public  BaseFunction {
	public:
		Help() {
			this->commandName = "help";
			this->helpDescription = "__**Help:**__ Enter !help or /help for help!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

				GetAuditLogData dataPackage;
				dataPackage.actionType = DiscordCoreAPI::AuditLogEvent::ROLE_CREATE;
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 25;
				dataPackage.userId = args->eventData.getAuthorId();
				AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();
				InputEventResponseData dataPackage02(InputEventResponseType::INTERACTION_RESPONSE_DEFERRED);
				dataPackage02.applicationId = args->eventData.getApplicationId();
				dataPackage02.channelId = args->eventData.getChannelId();
				dataPackage02.interactionToken = args->eventData.getInteractionToken();
				dataPackage02.interactionId = args->eventData.getInteractionId();
				dataPackage02.type = InteractionCallbackType::DeferredChannelMessageWithSource;
				EmbedData embed01;
				embed01.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				embed01.setColor("FEFEFE");
				embed01.setDescription("TEST DESCRIPTION!");
				embed01.setTimeStamp(getTimeAndDate());
				embed01.setFooter("HERE IT IS");
				dataPackage02.embeds.push_back(embed01);
				InputEventData newData = InputEventHandler::respondToEvent(dataPackage02).get();
				dataPackage02.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
				dataPackage02.type = InteractionCallbackType::ChannelMessageWithSource;
				InputEventData newData2 = InputEventHandler::respondToEvent(dataPackage02).get();

				InputEventHandler::deleteInputEventResponse(newData2, 5000).get();
				for (auto value : auditLogData.auditLogEntries) {
					cout << "ID: " << value.id << endl;
					cout << "TARGET ID: " << value.targetId << endl;
					for (auto value2 : value.changes) {
						cout << value2.newValueString << endl;
						cout << value2.oldValueString << endl;
					}
				}
				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync({ .deletePinned = false,.channelId = args->eventData.getChannelId(),.limit = 25, .beforeThisId = args->eventData.getMessageId() }).get();

				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Help help{};
}
#endif
