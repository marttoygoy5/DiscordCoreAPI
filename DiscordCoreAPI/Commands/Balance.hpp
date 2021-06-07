// Balance.hpp - Header for the "balance" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BALANCE_
#define _BALANCE_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Balance : public BaseFunction {
	public:
		Balance() {
			this->commandName = "balance";
			this->helpDescription = "__**Balance:**__ Enter !balance or /balance to view your own balance, or !balance = @USERMENTION /balance @USERMENTION to view someone else's balances.";
		}
		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			Channel channel = args->eventData.pDiscordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

			Guild guild = args->eventData.pDiscordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild = args->eventData.pDiscordCoreClient->getDiscordGuild(guild.data);

			bool areWeInADm = areWeInADM(args->eventData, channel, discordGuild);

			if (areWeInADm == true) {
				co_return;
			}

			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				InputEventHandler::deleteInputEventResponse(args->eventData).get();
			}

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			string userID = "";
			unsigned int bankAmount = 0;
			unsigned int walletAmount = 0;

			regex mentionRegExp("<@!\\d{18,}>");
			regex idRegExp("\\d{18,}");
			if (args->argumentsArray.size() == 0) {
				userID = args->eventData.pDiscordCoreClient->currentUser->data.id;
			}
			else if (args->argumentsArray.at(0) != "") {
				if (!regex_search(args->argumentsArray.at(0), mentionRegExp, regex_constants::match_flag_type::format_first_only) && !regex_search(args->argumentsArray.at(0), idRegExp, regex_constants::match_flag_type::format_first_only)) {
					string msgString = "------\n* *Please, enter a valid user mention, or enter none at all!(!balance = @USERMENTION)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
						responseData.channelId = args->eventData.messageData.channelId;
						responseData.messageId = args->eventData.messageData.id;
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
						InputEventHandler::deleteInputEventResponse(event01, 20000).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						InputEventData event;
						InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
						responseData.applicationId = args->eventData.interactionData.applicationId;
						responseData.embeds.push_back(msgEmbed);
						responseData.interactionId = args->eventData.interactionData.id;
						responseData.interactionToken = args->eventData.interactionData.token;
						responseData.type = InteractionCallbackType::ChannelMessage;
						event = InputEventHandler::respondToEvent(responseData).get();
						event.interactionData.applicationId = args->eventData.interactionData.applicationId;
						event.interactionData.token = args->eventData.interactionData.token;
						InputEventHandler::deleteInputEventResponse(event, 20000).get();
					}
					co_return;
				}
				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, idRegExp);
				userID = matchResults.str();
			}

			GuildMember guildMember = args->eventData.pDiscordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = userID }).get();

			if (guildMember.data.user.id == "") {
				string msgString = "------\n**Sorry, but that user could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
					responseData.channelId = args->eventData.messageData.channelId;
					responseData.messageId = args->eventData.messageData.id;
					responseData.embeds.push_back(msgEmbed);
					InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
					InputEventHandler::deleteInputEventResponse(event01, 20000).get();
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					InputEventData event;
					InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
					responseData.applicationId = args->eventData.interactionData.applicationId;
					responseData.embeds.push_back(msgEmbed);
					responseData.interactionId = args->eventData.interactionData.id;
					responseData.interactionToken = args->eventData.interactionData.token;
					responseData.type = InteractionCallbackType::ChannelMessage;
					event = InputEventHandler::respondToEvent(responseData).get();
					event.interactionData.applicationId = args->eventData.interactionData.applicationId;
					event.interactionData.token = args->eventData.interactionData.token;
					InputEventHandler::deleteInputEventResponse(event, 20000).get();
				}
				co_return;
			}

			DiscordGuildMember discordGuildMember = args->eventData.pDiscordCoreClient->getDiscordGuildMember(guildMember.data);

			string msgString = "";
			bankAmount = discordGuildMember.data.currency.bank;
			walletAmount = discordGuildMember.data.currency.wallet;
			msgString = "<@!" + guildMember.data.user.id + "> 's balances are:\n------\n__**Bank Balance:**__ " + to_string(bankAmount) + " " + args->eventData.pDiscordCoreClient->discordUser->data.currencyName +
				"\n__**Wallet Balance:**__ " + to_string(walletAmount) + " " + args->eventData.pDiscordCoreClient->discordUser->data.currencyName + "\n------";

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
			msgEmbed.setDescription(msgString);
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Current Balances:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
				responseData.channelId = args->eventData.messageData.channelId;
				responseData.messageId = args->eventData.messageData.id;
				responseData.embeds.push_back(msgEmbed);
				InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				InputEventData event;
				InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
				responseData.applicationId = args->eventData.interactionData.applicationId;
				responseData.embeds.push_back(msgEmbed);
				responseData.interactionId = args->eventData.interactionData.id;
				responseData.interactionToken = args->eventData.interactionData.token;
				responseData.type = InteractionCallbackType::ChannelMessage;
				event = InputEventHandler::respondToEvent(responseData).get();
				event.interactionData.applicationId = args->eventData.interactionData.applicationId;
				event.interactionData.token = args->eventData.interactionData.token;
			}
		}
	};
	Balance balance{};

}
#endif