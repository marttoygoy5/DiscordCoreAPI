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
			Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();

			bool areWeInADm = areWeInADM(args->message, channel);

			if (areWeInADm == true) {
				co_return;
			}

			if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::INTERACTION) {
				args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
			}

			Guild guild = args->coreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
			DiscordGuild discordGuild = args->coreClient->getDiscordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->message, discordGuild.data);

			if (areWeAllowed == false) {
				co_return;
			}

			string userID = "";
			unsigned int bankAmount = 0;
			unsigned int walletAmount = 0;

			regex mentionRegExp("<@!\\d{18,}>");
			regex idRegExp("\\d{18,}");
			if (args->argumentsArray.size() == 0) {
				userID = args->message.data.author.id;
			}
			else if (args->argumentsArray.at(0) != "") {
				if (!regex_search(args->argumentsArray.at(0), mentionRegExp, regex_constants::match_flag_type::format_first_only) && !regex_search(args->argumentsArray.at(0), idRegExp, regex_constants::match_flag_type::format_first_only)) {
					string msgString = "------\n* *Please, enter a valid user mention, or enter none at all!(!balance = @USERMENTION)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data,.embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
					co_return;
				}
				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, idRegExp);
				userID = matchResults.str();
			}

			GuildMember guildMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = userID }).get();

			if (guildMember.data.user.id == "") {
				string msgString = "------\n**Sorry, but that user could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
				Message  msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
				co_return;
			}

			DiscordGuildMember discordGuildMember = args->coreClient->getDiscordGuildMember(guildMember.data);

			string msgString = "";
			bankAmount = discordGuildMember.data.currency.bank;
			walletAmount = discordGuildMember.data.currency.wallet;
			msgString = "<@!" + guildMember.data.user.id + "> 's balances are:\n------\n__**Bank Balance:**__ " + to_string(bankAmount) + " " + args->coreClient->discordUser->data.currencyName +
				"\n__**Wallet Balance:**__ " + to_string(walletAmount) + " " + args->coreClient->discordUser->data.currencyName + "\n------";

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
			msgEmbed.setDescription(msgString);
			msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Current Balances:**__");
			args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data,.embed = msgEmbed });
		}
	};
	Balance balance{};

}
#endif