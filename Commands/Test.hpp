// Test.hpp - Header for the "test" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor("000000");
				msgEmbed.setDescription(args->argumentsArray.at(0));
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Welcome:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
					responseData.channelId = args->eventData.messageData.channelId;
					responseData.messageId = args->eventData.messageData.id;
					responseData.embeds.push_back(msgEmbed);
					InputEventHandler::respondToEvent(responseData).get();
				}

				task<AuditLogData> auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync({ .actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE, .guildId = args->eventData.getGuildId(), .limit = 25,   .userId = args->eventData.getAuthorId() });

				for (auto value : auditLogData.get().auditLogEntries) {
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
	Test test{};
}
#endif
