// SellDrugs.hpp - Header for the "sell drugs" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SELL_DRUGS_
#define _SELL_DRUGS_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class SellDrugs :public BaseFunction {
	public:
		SellDrugs() {
			this->commandName = "selldrugs";
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();
				
				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm == true) {
					return;
				}

				if (args->message.data.type != DiscordCoreInternal::MessageType::INTERACTION) {
					args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				Guild guild = args->coreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
				DiscordGuild discordGuild = args->coreClient->getDiscordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->message, discordGuild.data);

				if (areWeAllowed ==  false) {
					return;
				}
				
				unsigned long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				unsigned int msPerWorkCycle = args->coreClient->discordUser->data.hoursOfDrugSaleCooldown * 60 * 60 * 1000;
				unsigned int msPerSecond = 1000;
				unsigned int msPerMinute = 60 * msPerSecond;
				unsigned int msPerHour = 60 * msPerMinute;

				GuildMember guildMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
				DiscordGuildMember discordGuildMember(guildMember.data);

				unsigned int lastTimeWorked = discordGuildMember.data.lastTimeWorked;

				unsigned int timeDifference = (unsigned int)currentTime - lastTimeWorked;

				if (timeDifference >= msPerWorkCycle) {
					srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					unsigned int amountEarned = (unsigned int)trunc(((float)rand() / (float)RAND_MAX) * 5000.0f);

					discordGuildMember.data.currency.wallet += amountEarned;
					discordGuildMember.writeDataToDB().get();

					string msgString = "";
					msgString += "You've been busy dealing drugs... and you've earned " + to_string(amountEarned) + " " + args->coreClient->discordUser->data.currencyName + "\nNice job and watch out for cops!\nYour new wallet balance is: ";
					msgString += to_string(discordGuildMember.data.currency.wallet) + " " + args->coreClient->discordUser->data.currencyName;
					EmbedData messageEmbed;
					messageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Drug Dealing:**__");
					messageEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					messageEmbed.setTimeStamp(getTimeAndDate());
					ReplyMessageData replyMessageData;
					replyMessageData.embed = messageEmbed;
					replyMessageData.replyingToMessageData = args->message.data;
					args->coreClient->messages->replyAsync(replyMessageData).get();

					discordGuildMember.data.lastTimeWorked = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB().get();
				}
				else {
					unsigned int timeLeft = msPerWorkCycle - timeDifference;
					unsigned int hoursLeft = (unsigned int)trunc(timeLeft / msPerHour);
					unsigned int minutesLeft = (unsigned int)trunc((timeLeft % msPerHour) / msPerMinute);
					unsigned int secondsLeft = (unsigned int)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					string msgString = "";
				if (hoursLeft > 0) {
					msgString += "Sorry, but you need to wait " + to_string(hoursLeft) + " hours, " + to_string(minutesLeft) + " minutes, and " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				else if (minutesLeft > 0) {
					msgString += "Sorry, but you need to wait " + to_string(minutesLeft) + " minutes, and " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				else {
					msgString += "Sorry, but you need to wait " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				EmbedData  messageEmbed;
				messageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Drug Dealing:**__");
				messageEmbed.setColor(254, 254, 254);
				messageEmbed.setTimeStamp(getTimeAndDate());
				ReplyMessageData replyMessageData;
				replyMessageData.embed = messageEmbed;
				replyMessageData.replyingToMessageData = args->message.data;
				args->coreClient->messages->replyAsync(replyMessageData).get();
				}
			}
			catch (exception& e) {
				cout << "SellDrugs::execute() Error: " << e.what() << endl << endl;
			}
		}
	};
	SellDrugs sellDrugs{};
}
#endif
