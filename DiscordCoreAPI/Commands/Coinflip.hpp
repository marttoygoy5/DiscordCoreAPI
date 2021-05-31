// Coinflip.hpp - Header for the "coinflip" game/command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COIN_FLIP_
#define _COIN_FLIP_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Coinflip :public BaseFunction {
	public:
		Coinflip() {
			this->commandName = "coinflip";
			this->helpDescription = "";
		}
			virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();

				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm == true) {
					return;
				}

				if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::INTERACTION) {
					args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				Guild guild = args->coreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
				DiscordGuild discordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->message, discordGuild.data);

				if (areWeAllowed == false) {
					return;
				}
				GuildMember guildMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();

				if (!DiscordCoreAPI::PermissionsConverter::checkForPermission(guildMember, channel, DiscordCoreInternal::Permissions::MANAGE_MESSAGES)) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					return;
				}

				regex betAmountRegExp("\\d{1,18}");

				if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), betAmountRegExp) || stoll(args->argumentsArray.at(0)) < 1) {
					string msgString = "------\n**Please enter a valid amount to bet! 1 " + args->coreClient->discordUser->data.currencyName + " or more! (!coinflip = BETAMOUNT)**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					return;
				}

				DiscordGuildMember discordGuildMember(guildMember.data);

				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, betAmountRegExp);
				unsigned int betAmount = (unsigned int)stoll(matchResults.str());

				unsigned int currencyAmount = discordGuildMember.data.currency.wallet;

				if (betAmount > currencyAmount) {
					string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to place that wager!**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					return;
				}

				string newBetString = "";
				newBetString += "Welcome, <@!" + guildMember.data.user.id + "> , you have placed a bet of **" + to_string(betAmount) + " " + args->coreClient->discordUser->data.currencyName + "**.\n";
				newBetString += "React with :exploding_head: to choose heads, or with :snake: to choose tails!";

				DiscordCoreInternal::EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(0, 0, 255);
				msgEmbed.setDescription(newBetString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Heads, or Tails!?**__");
				ReplyMessageData replyMessageData;
				replyMessageData.replyingToMessageData = args->message.data;
				replyMessageData.embed = msgEmbed;
				DiscordCoreInternal::ActionRowData actionRowData;
				DiscordCoreInternal::ComponentData componentData01;
				componentData01.customId = "Heads";
				componentData01.disabled = false;
				componentData01.emoji.name = "🤯";
				componentData01.label = "Heads";
				componentData01.style = DiscordCoreInternal::ButtonStyle::Success;
				componentData01.type = DiscordCoreInternal::ComponentType::Button;
				DiscordCoreInternal::ComponentData componentData02;
				actionRowData.components.push_back(componentData01);
				componentData02.disabled = false;
				componentData02.customId = "Tails";
				componentData02.emoji.name = "🐍";
				componentData02.label = "Tails";
				componentData02.style = DiscordCoreInternal::ButtonStyle::Success;
				componentData02.type = DiscordCoreInternal::ComponentType::Button;
				actionRowData.components.push_back(componentData02);
				replyMessageData.components.push_back(actionRowData);
				Message message = args->coreClient->messages->replyAsync(replyMessageData).get();
				cout << message.data.channelId << "  " << message.data.author.id << "MSG ID: " << message.data.id << "  " << endl;
				vector<string> buttonIds;
				buttonIds.push_back("Heads");
				ButtonRequest buttonRequest;
				buttonIds.push_back("Tails");
				buttonRequest.buttonIds = buttonIds;
				buttonRequest.channelId = args->message.data.channelId;
				buttonRequest.userId = args->message.data.author.id;
				InteractionManager::areWeRunning = true;
				send(InteractionManager::buttonRequestBuffer, buttonRequest);
				ButtonResponse buttonResponse;
				try {
					buttonResponse = receive(InteractionManager::buttonResponseBuffer, 5000);
				}
				catch (exception& e) {
					string timeOutString = "------\nSorry, but you ran out of time to select heads or tails.\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setColor(255, 0, 0);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Heads, or Tails:**__");
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setDescription(timeOutString);
					message.data.components.resize(0);
					vector<DiscordCoreInternal::EmbedData>embeds;
					embeds.push_back(msgEmbed);
					args->coreClient->messages->editMessageAsync({ .embed = msgEmbed,.originalMessage = message, .components = vector<DiscordCoreInternal::ActionRowData>() }, args->message.data.channelId, message.data.id).get();
					return;
				}
				srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				float number = ((float)rand() / (float)RAND_MAX);
				unsigned int newBalance = 0;

				discordGuildMember.getDataFromDB();
				currencyAmount = discordGuildMember.data.currency.wallet;

				if (betAmount > currencyAmount) {
					string completionString = "------\nSorry, but you have insufficient funds in your wallet to place that wager.\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setColor(255, 0, 0);
					msgEmbed.setDescription(completionString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Heads, or Tails**__");
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					args->coreClient->messages->editMessageAsync({ .embed = msgEmbed, .originalMessage = message }, args->message.data.channelId, message.data.id);
					return;
				}

				DiscordCoreInternal::EmbedData msgEmbed2;
				if (buttonResponse.buttonId == "Heads" && number > 0.50 || buttonResponse.buttonId == "Tails" && number < 0.50) {
					discordGuildMember.data.currency.wallet += betAmount;
					discordGuildMember.writeDataToDB().get();
					discordGuild.data.casinoStats.totalCoinFlipPayout += betAmount;
					discordGuild.data.casinoStats.totalPayout += betAmount;
					if (betAmount > discordGuild.data.casinoStats.largestCoinFlipPayout.amount) {
						discordGuild.data.casinoStats.largestCoinFlipPayout.amount = betAmount;
						discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestCoinFlipPayout.userId = args->message.data.author.id;
						discordGuild.data.casinoStats.largestCoinFlipPayout.userName = args->message.data.author.username;
						discordGuild.writeDataToDB();
					}
					newBalance = discordGuildMember.data.currency.wallet;
					string completionString = "------\nNICELY DONE FAGGOT! YOU WON!\nYour new wallet balance is: " + to_string(newBalance) + " " + args->coreClient->discordUser->data.currencyName + ".\n------";
					msgEmbed2.setColor(0, 255, 0);
					msgEmbed2.setDescription(completionString);
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed2.setTitle("__**Heads, or Tails?**__");
				}
				else if (buttonResponse.buttonId == "Heads" && number <= 0.50 || buttonResponse.buttonId == "Tails" && number >= 0.50) {
					discordGuildMember.data.currency.wallet -= betAmount;
					discordGuildMember.writeDataToDB();
					discordGuild.data.casinoStats.totalCoinFlipPayout -= betAmount;
					discordGuild.data.casinoStats.totalPayout -= betAmount;
					discordGuild.writeDataToDB();
					newBalance = discordGuildMember.data.currency.wallet;
					string completionString = "------\nOWNED FUCK FACE! YOU LOST!\nYour new wallet balance is: " + to_string(newBalance) + " " + args->coreClient->discordUser->data.currencyName + ".\n------";
					msgEmbed2.setColor(255, 0, 0);
					msgEmbed2.setDescription(completionString);
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed2.setTitle("__**Heads, or Tails?**__");
				}
				message.data.components.resize(0);
				args->coreClient->messages->editMessageAsync({ .embed = msgEmbed2, .originalMessage = message }, args->message.data.channelId, message.data.id).get();
				return;
			}
			catch (exception& e) {
				cout << "Coinflip::execute() Error: " << e.what() << endl;
			}
		}

					};
					Coinflip coinFlip{};
				};
#endif
