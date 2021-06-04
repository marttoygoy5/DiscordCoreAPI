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
			virtual task<void> execute(BaseFunctionArguments* args) {
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();

				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::INTERACTION) {
					args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				Guild guild = args->coreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
				DiscordGuild discordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->message, discordGuild.data);

				if (areWeAllowed == false) {
					co_return;
				}
				GuildMember guildMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
				GuildMember botMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->coreClient->discordUser->data.userId }).get();
				if (!DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, DiscordCoreInternal::Permissions::MANAGE_MESSAGES)) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}

				regex betAmountRegExp("\\d{1,18}");

				if (args->argumentsArray.size() == 0 || !std::regex_search(args->argumentsArray.at(0), betAmountRegExp) || stoll(args->argumentsArray.at(0)) < 1) {
					string msgString = "------\n**Please enter a valid amount to bet! 1 " + args->coreClient->discordUser->data.currencyName + " or more! (!coinflip = BETAMOUNT)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}

				DiscordGuildMember discordGuildMember(guildMember.data);

				cmatch matchResults;
				std::regex_search(args->argumentsArray.at(0).c_str(), matchResults, betAmountRegExp);
				unsigned int betAmount = (unsigned int)stoll(matchResults.str());
				unsigned int currencyAmount = discordGuildMember.data.currency.wallet;

				if (betAmount > currencyAmount) {
					string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to place that wager!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}

				string newBetString = "";
				newBetString += "Welcome, <@!" + guildMember.data.user.id + "> , you have placed a bet of **" + to_string(betAmount) + " " + args->coreClient->discordUser->data.currencyName + "**.\n";
				newBetString += "React with :exploding_head: to choose heads, or with :snake: to choose tails!";

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(0, 0, 255);
				msgEmbed.setDescription(newBetString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Heads, or Tails!?**__");
				ReplyMessageData replyMessageData;
				replyMessageData.replyingToMessageData = args->message.data;
				replyMessageData.embed = msgEmbed;
				ActionRowData actionRowData;
				ComponentData componentData01;
				componentData01.disabled = false;
				componentData01.customId = "Heads";
				componentData01.emoji.name = "🤯";
				componentData01.label = "Heads";
				componentData01.style = ButtonStyle::Success;
				componentData01.type = ComponentType::Button;
				ComponentData componentData02;
				actionRowData.components.push_back(componentData01);
				componentData02.disabled = false;
				componentData02.customId = "Tails";
				componentData02.emoji.name = "🐍";
				componentData02.label = "Tails";
				componentData02.style = ButtonStyle::Success;
				componentData02.type = ComponentType::Button;
				actionRowData.components.push_back(componentData02);
				replyMessageData.components.push_back(actionRowData);
				Message message = args->coreClient->messages->replyAsync(replyMessageData).get();
				vector<string> buttonIds;
				buttonIds.push_back("Heads");
				buttonIds.push_back("Tails");
				InteractionData interactionData;
				Button button2(message.data);
				ButtonInteractionData buttonInteractionData = button2.getOurButtonData(60000);
				if (buttonInteractionData.customId == ""){
					string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					EmbedData msgEmbed2;
					msgEmbed2.setColor(255, 0, 0);
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setTitle("__**Blackjack Game:**__");
					msgEmbed2.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed2.setDescription(timeOutString);
					vector<EmbedData> embeds;
					embeds.push_back(msgEmbed2);
					message.data.author = args->message.data.author;
					DiscordCoreAPI::Message newerMessage = args->coreClient->messages->editMessageAsync({ .embed = msgEmbed2,.originalMessageData = message.data, .components = vector<DiscordCoreAPI::ActionRowData>() }, args->message.data.channelId, message.data.id).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = newerMessage.data.channelId, .messageId = newerMessage.data.id, .timeDelay = 20000 }).get();
					co_return;

				}
				
				std::srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				float number = ((float)rand() / (float)RAND_MAX);
				unsigned int newBalance = 0;

				discordGuildMember.getDataFromDB();
				currencyAmount = discordGuildMember.data.currency.wallet;

				if (betAmount > currencyAmount) {
					string completionString = "------\nSorry, but you have insufficient funds in your wallet to place that wager.\n------";
					EmbedData msgEmbed3;
					msgEmbed3.setColor(255, 0, 0);
					msgEmbed3.setDescription(completionString);
					msgEmbed3.setTimeStamp(getTimeAndDate());
					msgEmbed3.setTitle("__**Heads, or Tails**__");
					msgEmbed3.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					args->coreClient->messages->editMessageAsync({ .embed = msgEmbed3, .originalMessageData = message.data, }, args->message.data.channelId, message.data.id).get();
					co_return;
				}

				EmbedData msgEmbed4;
				if (button2.getButtonId() == "Heads" && number > 0.50 || button2.getButtonId() == "Tails" && number < 0.50) {
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
					msgEmbed4.setColor(0, 255, 0);
					msgEmbed4.setDescription(completionString);
					msgEmbed4.setTimeStamp(getTimeAndDate());
					msgEmbed4.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed4.setTitle("__**Heads, or Tails?**__");
					args->coreClient->messages->editMessageAsync({ .embed = msgEmbed4, .originalMessageData = message.data }, args->message.data.channelId, message.data.id).get();
				}
				else if (button2.getButtonId() == "Heads" && number <= 0.50 || button2.getButtonId() == "Tails" && number >= 0.50) {
					discordGuildMember.data.currency.wallet -= betAmount;
					discordGuildMember.writeDataToDB();
					discordGuild.data.casinoStats.totalCoinFlipPayout -= betAmount;
					discordGuild.data.casinoStats.totalPayout -= betAmount;
					discordGuild.writeDataToDB();
					newBalance = discordGuildMember.data.currency.wallet;
					string completionString = "------\nOWNED FUCK FACE! YOU LOST!\nYour new wallet balance is: " + to_string(newBalance) + " " + args->coreClient->discordUser->data.currencyName + ".\n------";
					msgEmbed4.setColor(255, 0, 0);
					msgEmbed4.setDescription(completionString);
					msgEmbed4.setTimeStamp(getTimeAndDate());
					msgEmbed4.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed4.setTitle("__**Heads, or Tails?**__");
					args->coreClient->messages->editMessageAsync({ .embed = msgEmbed4, .originalMessageData = message.data }, args->message.data.channelId, message.data.id).get();
				}
				co_return;
			}
			catch (exception& e) {
				cout << "Coinflip::execute() Error: " << e.what() << endl;
			}
		}

					};
					Coinflip coinFlip{};
				};
#endif
