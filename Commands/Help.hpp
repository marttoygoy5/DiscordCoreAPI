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
			this->helpDescription = "__**Help Usage:**__ Enter !help or /help for help!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				if (channel.data.type != ChannelType::DM) {
					InputEventHandler::deleteInputEventResponse(args->eventData).get();
				}

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild.data);

				EmbedData errorMsgEmbed;
				errorMsgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				errorMsgEmbed.setColor(discordGuild.data.borderColor);
				errorMsgEmbed.setTimeStamp(getTimeAndDate());

				if (args->argumentsArray.size() == 0){
					string msgString = "------\n**Please enter a proper bot-username! (!help = BOTUSERNAME)**\n------";
					errorMsgEmbed.setDescription(msgString);
					errorMsgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventData event;
						ReplyMessageData responseData(args->eventData);
						responseData.embed = errorMsgEmbed;
						event = InputEventHandler::respondToEvent(responseData).get();
						InputEventHandler::deleteInputEventResponse(event, 20000).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						InputEventData event;
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(errorMsgEmbed);
						event = InputEventHandler::respondToEvent(responseData).get();
						event.interactionData.applicationId = args->eventData.interactionData.applicationId;
						event.interactionData.token = args->eventData.interactionData.token;
						InputEventHandler::deleteInputEventResponse(event, 20000).get();
					}
					co_return;
				}
				else if (args->argumentsArray.size() == 1) {
					if (args->argumentsArray.at(0) != args->eventData.discordCoreClient->currentUser->data.username) {
						cout << args->eventData.discordCoreClient->currentUser->data.username << endl;
						co_return;
					}
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());

					msgEmbed.setTitle("__**" + args->eventData.discordCoreClient->currentUser->data.username + " Help:**__");

					string msgString = "__**Help Usage:**__ Enter !help = BOTUSERNAME, COMMANDNAME, where the command names are as follows:\n";
					for (auto value : CommandController::commands) {
						msgString += value.first;
						if (value != *CommandController::commands.end()) {
							msgString += ", ";
						}
					}
					msgEmbed.setDescription(msgString);

					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventData event;
						ReplyMessageData responseData(args->eventData);
						responseData.embed = msgEmbed;
						event = InputEventHandler::respondToEvent(responseData).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						InputEventData event;
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						event = InputEventHandler::respondToEvent(responseData).get();
						event.interactionData.applicationId = args->eventData.interactionData.applicationId;
						event.interactionData.token = args->eventData.interactionData.token;
					}
				}
				else if (args->argumentsArray.size() == 2) {
					if (args->argumentsArray.at(0) != args->eventData.discordCoreClient->currentUser->data.username) {
						cout << args->eventData.discordCoreClient->currentUser->data.username << endl;
						co_return;
					}
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());

					msgEmbed.setTitle("__**" + args->eventData.discordCoreClient->currentUser->data.username + " Help:**__");

					string msgString;
					if (CommandController::commands.contains(args->argumentsArray.at(1))) {
						msgString = CommandController::commands.at(args->argumentsArray.at(1))->helpDescription;
					}
					else {
						msgString = "Sorry, but that command was not found!";
					}
					msgEmbed.setDescription(msgString);

					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventData event;
						ReplyMessageData responseData(args->eventData);
						responseData.embed = msgEmbed;
						event = InputEventHandler::respondToEvent(responseData).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						InputEventData event;
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						event = InputEventHandler::respondToEvent(responseData).get();
						event.interactionData.applicationId = args->eventData.interactionData.applicationId;
						event.interactionData.token = args->eventData.interactionData.token;
					}
				}

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
