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

				if (args->message.data.type != DiscordCoreInternal::MessageType::INTERACTION) {
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
					EmbedData msgEmbed;
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
					EmbedData msgEmbed;
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
					EmbedData msgEmbed;
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
				componentData01.customId = "Heads";
				componentData01.disabled = false;
				componentData01.emoji.name = "🤯";
				componentData01.label = "Heads";
				componentData01.style = ButtonStyle::Success;
				componentData01.type = ComponentType::Button;
				actionRowData.components.push_back(componentData01);
				ComponentData componentData02;
				componentData02.customId = "Tails";
				componentData02.disabled = false;
				componentData02.emoji.name = "🐍";
				componentData02.label = "Tails";
				componentData02.style = ButtonStyle::Success;
				componentData02.type = ComponentType::Button;
				actionRowData.components.push_back(componentData02);
				replyMessageData.components.push_back(actionRowData);
				Message newMessage = args->coreClient->messages->replyAsync(replyMessageData).get();

				//auto filter = +AFsAXQ-(DiscordCoreInternal::InteractionData interactionData) {interactionData; +AH0AOw-
				/*
				args->coreClient->reactions->createReactionAsync({.channelId = newMessage.data.channelId, .messageId = newMessage.data.id, .emojiName = +ACLYPt0vACI - } ).get();
				args->coreClient->reactions->createReactionAsync({.channelId = newMessage.data.channelId, .messageId = newMessage.data.id, .emojiName = +ACLYPdwNACI - } ).get();
				const filter = (reaction: Discord.MessageReaction, user : Discord.User) = +AD4 - (reaction.emoji.name == = +AHwAfA - reaction.emoji.name == = ) + ACYAJg - user.id == = commandData.guildMember + ACE - .id;
				await newMessage.awaitReactions(filter, { max: 1, time : 120000).then(async collected = +AD4 - {
					if (collected.size == = 0) {
						let timeOutString = '';
						timeOutString = newBetString;

						timeOutString + -= '+AFw-n+AFw-n+AF8AXwAqACo-Sorry, but you ran out of time to select heads or tails+ACEAKgAqAF8AXw-';
						const messageEmbed2 = new Discord.MessageEmbed();
						messageEmbed2.setColor(+AFs - 255, 0, 0 + AF0 - ).setTimestamp(Date.now()).setTitle('+AF8AXwAqACo-Heads, or Tails+ACE-?+ACoAKgBfAF8-').setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL() + ACE - )
							.setDescription(timeOutString);
						await newMessage.edit(messageEmbed2);
						newMessage.reactions.removeAll();
					}
					else if (collected.first() + ACE - .emoji.name == = '+2D7dLw-' || collected.first() + ACE - .emoji.name == = '+2D3cDQ-') {
						const number = Math.random() + ACo - 2;
						let completionString = '';
						completionString = newBetString;
						let newBalance = 0;
						const messageEmbed3 = new Discord.MessageEmbed();

						await guildMemberData.getFromDataBase();
						currencyAmount = guildMemberData.currency.wallet;

						if (betAmount > currencyAmount) {
							completionString + -= '+AFw-n+AFw-n+AF8AXwAqACo-Sorry, but you have insufficient funds in your wallet to place that wager+ACEAKgAqAF8AXw-';

							messageEmbed3.setColor(+AFs - 255, 0, 0 + AF0 - ).setDescription(completionString).setTimestamp(Date.now()).setTitle('+AF8AXwAqACo-Heads, or Tails+ACE-?+ACoAKgBfAF8-')
								.setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL() + ACE - );
							await newMessage.edit(messageEmbed3);
							newMessage.reactions.removeAll();
							return commandReturnData;
						}

						if ((number > 1 + ACYAJg - collected.first() + ACE - .emoji.name == = '+2D7dLw-') || (number <- 1 + ACYAJg - collected.first() + ACE - .emoji.name == = '+2D3cDQ-')) {
							await guildData.getFromDataBase();
							guildMemberData.currency.wallet + -= betAmount;
							await guildMemberData.writeToDataBase();
							guildData.casinoStats.totalPayout + -= betAmount;
							guildData.casinoStats.totalCoinFlipPayout + -= betAmount;
							if (betAmount > guildData.casinoStats.largestCoinFlipPayout.amount) {
								guildData.casinoStats.largestCoinFlipPayout.amount = betAmount;
								guildData.casinoStats.largestCoinFlipPayout.date = Date();
								guildData.casinoStats.largestCoinFlipPayout.userID = guildMemberData.id + ACEAOw -
								guildData.casinoStats.largestCoinFlipPayout.username = guildMemberData.userName + ACEAOw -
							}
							await guildData.writeToDataBase();
							newBalance = guildMemberData.currency.wallet;
							completionString + -= +AGAAXA - n + AFw - n + AF8AXw - +ACo - +ACo - NICELY DONE FAGGOT + ACE - YOU WON + ACEAKgAqAF8AXwBc - nYour new wallet balance is : +ACQAew - newBalance+ACQAew - discordUser.userData.currencyName + AH0AYAA7 -

								messageEmbed.setColor(+AFs - 0, 255, 0 + AF0 - ).setDescription(completionString).setTimestamp(Date.now()).setTitle('+AF8AXwAqACo-Heads, or Tails+ACE-?+ACoAKgBfAF8-')
								.setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL() + ACE - );
						}
						else if ((number <- 1 + ACYAJg - collected.first() + ACE - .emoji.name == = '+2D7dLw-') || (number > 1 + ACYAJg - collected.first() + ACE - .emoji.name == = '+2D3cDQ-')) {
							await guildData.getFromDataBase();
							guildMemberData.currency.wallet -= betAmount;
							await guildMemberData.writeToDataBase();
							guildData.casinoStats.totalPayout -= betAmount;
							guildData.casinoStats.totalCoinFlipPayout -= betAmount;
							await guildData.writeToDataBase();
							newBalance = guildMemberData.currency.wallet;
							completionString + -= +AGAAXA - n + AFw - n + AF8AXw - +ACo - +ACo - OWNED + ACE - YOU LOST, FUCKFACE + ACEAKgAqAF8AXwBc - nYour new wallet balance is : +ACQAew - newBalance+ACQAew - discordUser.userData.currencyName + AH0AYAA7 -

								messageEmbed.setColor(+AFs - 255, 0, 0 + AF0 - ).setDescription(completionString).setTimestamp(Date.now()).setTitle('+AF8AXwAqACo-Heads, or Tails+ACE-?+ACoAKgBfAF8-')
								.setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL() + ACE - );
						}
						await newMessage.edit(messageEmbed);
						newMessage.reactions.removeAll();
					}
					return commandReturnData;
					+ACo - */

			}
			catch (exception& e) {
				cout << "Coinflip::execute() Error: " << e.what() << endl;
			}

		}

					};
					Coinflip coinFlip{};
				};
#endif
