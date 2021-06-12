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
					ReplyMessageData responseData01(args->eventData);
					responseData01.embed = msgEmbed;
					InputEventData event01 = InputEventHandler::respondToEvent(responseData01).get();
					cout << event01.getChannelId() << " " << event01.getMessageId() << endl;
					EditMessageData responseData(event01);
					msgEmbed.setDescription("NEW DESCRIPTION");
					responseData.embed = msgEmbed;

					InputEventData event03 = InputEventHandler::respondToEvent(responseData).get();
					cout << "APP ID: " << event03.getApplicationId() << " INT TOKEN: " << event03.getInteractionToken() << endl;
					InputEventHandler::deleteInputEventResponse(event03, 5000).get();
				}
				else {
					
					
				}
				GetAuditLogData dataPackage;
				dataPackage.actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE;
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 25;
				dataPackage.userId = args->eventData.getAuthorId();
				AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();

				EmbedData embed01;
				embed01.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				embed01.setColor("FEFEFE");
				embed01.setDescription("TEST DESCRIPTION!");
				embed01.setTimeStamp(getTimeAndDate());
				embed01.setFooter("HERE IT IS");
				//InputEventData newData = InputEventHandler::respondToEvent(dataPackage02).get();
				//InputEventData newData2 = InputEventHandler::respondToEvent(dataPackage02).get();
				//newData2.interactionData.token = args->eventData.getInteractionToken();
				//newData2.interactionData.id = args->eventData.getInteractionId();
				//InputEventHandler::deleteInputEventResponse(newData2, 5000).get();

				for (auto value : auditLogData.auditLogEntries) {
					for (auto value2 : value.changes) {
						cout << value2.newValueString << endl;
						cout << value2.oldValueString << endl;
					}
				}
				DeleteMessagesBulkData dataPackage2;
				dataPackage2.deletePinned = true;
				dataPackage2.limit = 25;
				dataPackage2.beforeThisId = args->eventData.getMessageId();
				dataPackage2.channelId = args->eventData.getChannelId();
				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync(dataPackage2).get();

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
