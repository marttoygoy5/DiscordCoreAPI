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
				regex userIDRegExp("<@!\\d{1,18}>");
				if (args->argumentsArray.size() > 0) {
					if (args->argumentsArray.at(0) == "testing") {
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor("FEFE00");
						msgEmbed.setDescription("__**READY TO GO!**__");
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Welcome:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData01(args->eventData);
							responseData01.embed = msgEmbed;
							InputEventData event01 = InputEventHandler::respondToEvent(responseData01).get();
							EditMessageData responseData(event01);
							msgEmbed.setDescription("Empty Description");
							responseData.embed = msgEmbed;
							InputEventData event03 = InputEventHandler::respondToEvent(responseData).get();
							InputEventHandler::deleteInputEventResponse(event03, 5000).get();
						}
						else {
							CreateEphemeralInteractionResponseData newData(args->eventData);
							newData.data.embeds.push_back(msgEmbed);
							auto eventNew = InputEventHandler::respondToEvent(newData).get();
							EditInteractionResponseData editData(eventNew);
							msgEmbed.setDescription("EDITED DESCRIPTION!");
							editData.embeds.push_back(msgEmbed);
							auto eventNew02 = InputEventHandler::respondToEvent(editData).get();
							InputEventHandler::deleteInputEventResponse(eventNew02, 5000).get();
						}
					}
					else if (regex_search(args->argumentsArray.at(0), userIDRegExp)){
						cmatch matchResults;
						regex_search(args->argumentsArray.at(0).c_str(), matchResults, userIDRegExp);
						string userID = matchResults.str();
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor("FE00FE");
						msgEmbed.setDescription("__**READY TO GO:**__\n" + userID);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Welcome:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData01(args->eventData);
							responseData01.embed = msgEmbed;
							InputEventData event01 = InputEventHandler::respondToEvent(responseData01).get();
							EditMessageData responseData(event01);
							msgEmbed.setDescription("Edited description!");
							responseData.embed = msgEmbed;
							responseData.embed.setDescription(userID);
							InputEventData event03 = InputEventHandler::respondToEvent(responseData).get();
							InputEventHandler::deleteInputEventResponse(event03, 5000).get();
						}
						else {
							CreateInteractionResponseData newData(args->eventData);
							newData.data.embeds.push_back(msgEmbed);
							auto eventNew = InputEventHandler::respondToEvent(newData).get();
							EditInteractionResponseData editData(eventNew);
							msgEmbed.setColor("00FEFE");
							msgEmbed.setDescription(userID);
							editData.embeds.push_back(msgEmbed);
							auto eventNew02 = InputEventHandler::respondToEvent(editData).get();
							InputEventHandler::deleteInputEventResponse(eventNew02, 5000).get();
						}
					}
					else if(args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION){
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor("FEFEFE");
						msgEmbed.setDescription(args->argumentsArray.at(0));
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Welcome to &:**__");
						CreateDeferredInteractionResponseData newData01(args->eventData);
						auto newEvent = InputEventHandler::respondToEvent(newData01).get();
						CreateFollowUpMessageData newData(newEvent);
						newData.embeds.push_back(msgEmbed);
						newEvent = InputEventHandler::respondToEvent(newData).get();
						vector<EmbedData> embeds;
						EditFollowUpMessageData newData02(newEvent);
						msgEmbed.setColor("000000");
						embeds.push_back(msgEmbed);
						newData02.embeds = embeds;
						//newEvent = InputEventHandler::respondToEvent(newData02).get();
						///InputEventHandler::deleteInputEventResponse(newEvent, 5000).get();
					}
					else if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor("FEFEFE");
						msgEmbed.setDescription(args->argumentsArray.at(0));
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Welcome to &:**__");
						ReplyMessageData newData01(args->eventData);
						newData01.embed = msgEmbed;
						auto newEvent = InputEventHandler::respondToEvent(newData01).get();
						/*
						CreateFollowUpMessageData newData(newEvent);
						newData.embeds.push_back(msgEmbed);
						newEvent = InputEventHandler::respondToEvent(newData).get();
						vector<EmbedData> embeds;
						EditFollowUpMessageData newData02(newEvent);
						msgEmbed.setColor("000000");
						embeds.push_back(msgEmbed);
						newData02.embeds = embeds;
						*/
					}
				}
				string permissionString;
				vector<Permissions> permisVector;
				permisVector.push_back(Permissions::ADD_REACTIONS);
				permisVector.push_back(Permissions::CONNECT);
				permisVector.push_back(Permissions::ATTACH_FILES);
				permisVector.push_back(Permissions::ADMINISTRATOR);
				permissionString = DiscordCoreAPI::PermissionsConverter::addPermissionsToString(permissionString, permisVector);
				DiscordCoreAPI::PermissionsConverter::displayPermissions(permissionString);
				permisVector.erase(permisVector.begin(), permisVector.begin() + 2);
				for (auto value : permisVector) {
					cout << "VALUE: " << to_string((int)value) << endl;
				}
				permissionString = DiscordCoreAPI::PermissionsConverter::removePermissionsFromString(permissionString, permisVector);
				DiscordCoreAPI::PermissionsConverter::displayPermissions(permissionString);
				DeleteMessagesBulkData deleteData;
				deleteData.beforeThisId = args->eventData.getMessageId();
				deleteData.channelId = args->eventData.getChannelId();
				deleteData.limit = 100;
				deleteData.deletePinned = false;
				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync(deleteData).get();

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
