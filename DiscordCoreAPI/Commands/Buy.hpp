// Buy.hpp - Header for the "Buy" command.
// June 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BUY_
#define _BUY_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Buy : public BaseFunction {
	public:
		Buy() {
			this->commandName = "buy";
			this->helpDescription = "__**Buy**__ !buy = ITEMNAME/ROLENAME";
		}
		virtual task<void> execute(BaseFunctionArguments* args) {

			Channel channel = args->coreClient->channels->getChannelAsync({ .channelId = args->message.data.channelId }).get();

			bool areWeInADm = areWeInADM(args->message, channel);

			if (areWeInADm == true) {
				co_return;
			}

			if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::INTERACTION) {
				args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
			}

			Guild guild = args->coreClient->guilds->getGuildAsync({ .guildId = args->message.data.guildId }).get();
			DiscordGuild discordGuild(guild.data);

			if (args->argumentsArray.at(0) == "") {
				string msgString = "------\n**Please enter an item name!(!buy = ITEMNAME)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
				co_return;
			}

			string objectName = args->argumentsArray.at(0);
			unsigned int objectShopIndex = 0;
			string objectType;

			GuildMember guildMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);

			bool isFoundInShop = false;
			for (unsigned int x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
				if (objectName == discordGuild.data.guildShop.items.at(x).itemName) {
					isFoundInShop = true;
					objectShopIndex = x;
					objectType = "item";
					break;
				}
			}
			for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
				if (objectName == discordGuild.data.guildShop.roles.at(x).roleName){
					isFoundInShop = true;
					objectShopIndex = x;
					objectType = "role";
					break;
				}
			}

			if (isFoundInShop == false) {
				string msgString = "------\n**Sorry, but we could not find that object in the shop!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Object:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
				co_return;
			}

			bool isFoundInInventory = false;

			if (discordGuildMember.data.items.contains(objectName)) {
				isFoundInInventory = true;
			}

			if (discordGuildMember.data.roles.contains(objectName)) {
				isFoundInInventory = true;
			}

			if (isFoundInInventory == true) {
				string msgString = "------\n**Sorry, but you already have one of those ${ objectType }s.** \n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Duplicate Object:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
				co_return;
			}

			if (objectType == "role") {
				unsigned int  roleCost = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
				unsigned int userBalance = discordGuildMember.data.currency.wallet;

				if (roleCost > userBalance) {
					string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}
				InventoryRole newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
				discordGuildMember.data.roles.insert(make_pair(newRole.roleName, newRole));
				discordGuildMember.data.currency.wallet -= roleCost;
				discordGuildMember.writeDataToDB().get();

				unsigned int newBalance = discordGuildMember.data.currency.wallet;

				string roleID = discordGuild.data.guildShop.roles.at(objectShopIndex).roleId;
				/*
				const guildMemberRoleManager = new Discord.GuildMemberRoleManager(commandData.guildMember as Discord.GuildMember);

				guildMemberRoleManager.add(roleID);

				msgString = `----- - \nCongratulations!You've just purchased a new ${objectType}.\n------\n__**It is as follows:**__ <@&${newRole.roleID}> (${newRole.roleName})\n------\n__**Your new wallet balance:**__ ${newBalance} ${discordUser.userData.currencyName}\n------`;
					messageEmbed.setTitle('__**New Role Purchased:**__');

				let maxIdx = 0;
				let tempRole : FoundationClasses.InventoryRole;
				const len = guildMemberData.roles.length;
				for (let x = 0; x < len; x += 1) {
					maxIdx = x;
					for (let y = x + 1; y < len; y += 1) {
						if (guildMemberData.roles[y]!.roleCost > guildMemberData.roles[maxIdx]!.roleCost) {
							maxIdx = y;
						}
					}
					tempRole = guildMemberData.roles[x]!;
					guildMemberData.roles[x] = guildMemberData.roles[maxIdx]!;
					guildMemberData.roles[maxIdx] = tempRole;
				}
				await guildData.writeToDataBase();
			}
			else if (objectType == = 'item') {
				const { itemCost } = guildData.guildShop.items[objectShopIndex]!;
				const userBalance = guildMemberData.currency.wallet;

				if (itemCost > userBalance) {
					const msgString = `----- - \n * Sorry, but you have insufficient funds in your wallet to purchase that!**\n------`;
						let msgEmbed = new Discord.MessageEmbed()
						.setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL()!)
						.setColor(guildData.borderColor as[number, number, number])
						.setDescription(msgString)
						.setTimestamp(Date() as unknown as Date)
						.setTitle('__**Insufficient Funds:**__')
						let msg = await HelperFunctions.sendMessageWithCorrectChannel(commandData, msgEmbed);
					if (commandData.toTextChannel instanceof Discord.WebhookClient) {
						msg = new Discord.Message(commandData.guild!.client, msg, commandData.fromTextChannel!);
					}
					await msg.delete({ timeout: 20000 });
					return commandReturnData;
				}

				const newItem = guildData.guildShop.items[objectShopIndex]!;
				guildMemberData.items.push(newItem);
				guildMemberData.currency.wallet -= itemCost;
				await guildMemberData.writeToDataBase();

				const itemEmoji = guildData.guildShop.items[objectShopIndex]!.emoji;
				const { itemName } = guildData.guildShop.items[objectShopIndex]!;
				const newBalance = guildMemberData.currency.wallet;
				msgString = `----- - \nCongratulations!You've just purchased a new ${objectType}.\n------\n__**It is as follows:**__ ${itemEmoji}${itemName}\n------\n__**Your new wallet balance:**__ ${newBalance} ${discordUser.userData.currencyName}\n------`;
					messageEmbed.setTitle('__**New Item Purchased:**__');

				let maxIdx = 0;
				let tempItem : FoundationClasses.InventoryItem;
				const len = guildMemberData.items.length;
				for (let x = 0; x < len; x += 1) {
					maxIdx = x;
					for (let y = x + 1; y < len; y += 1) {
						if (guildMemberData.items[y]!.itemCost > guildMemberData.items[maxIdx]!.itemCost) {
							maxIdx = y;
						}
					}
					tempItem = guildMemberData.items[x]!;
					guildMemberData.items[x] = guildMemberData.items[maxIdx]!;
					guildMemberData.items[maxIdx] = tempItem;
				}
				await guildMemberData.writeToDataBase();
			}
			messageEmbed
				.setTimestamp(Date.now())
				.setDescription(msgString)
				.setAuthor((commandData.guildMember as Discord.GuildMember).user.username, (commandData.guildMember as Discord.GuildMember).user.avatarURL()!)
				.setColor(guildData.borderColor as[number, number, number]);
			await HelperFunctions.sendMessageWithCorrectChannel(commandData, messageEmbed);*/
				co_return;
			}
		};
	};
	Buy buy{};
}
#endif
