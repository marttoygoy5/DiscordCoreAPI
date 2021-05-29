// AddShopItem.hpp - Header for the "add shop item" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ADD_SHOP_ITEM_
#define _ADD_SHOP_ITEM_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class AddShopItem :public BaseFunction {
	public:
		AddShopItem() {
			this->commandName = "addshopitem";
			this->helpDescription = "__**Add Shop Item:**__ Enter !addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI, or /addshopitem ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI.";
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			Channel channel = args->coreClient->channels->getChannelAsync({ .channelId = args->message.data.channelId }).get();
			bool areWeInADm = areWeInADM(args->message, channel);

			if (areWeInADm ==  true) {
				return;
			}

			if (args->message.data.type != DiscordCoreInternal::MessageType::INTERACTION) {
				args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
			}

			bool doWeHaveAdmin = doWeHaveAdminPermissions(args->message);

			if (doWeHaveAdmin == false) {
				return;
			}
			Guild guild = args->coreClient->guilds->getGuildAsync({ .guildId = args->message.data.guildId }).get();

			DiscordGuild discordGuild(guild.data);

			regex itemNameRegExp("\.{1,32}");
			regex selfModRegExp("\\d{1,5}");
			regex oppModRegExp("-{0,1}\\d{1,5}");
			regex itemCostRegExp("\\d{1,6}");
			regex emojiRegExp("\.{1,32}");
			if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), itemNameRegExp)) {
				string msgString = "------\n**Please enter a valid item name! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
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
			if (args->argumentsArray.size() < 2 || !regex_search(args->argumentsArray.at(1), selfModRegExp) || stoll(args->argumentsArray.at(1)) > 100 || stoll(args->argumentsArray.at(1)) < 0) {
				string msgString = "------\n**Please enter a valid self - mod value, between 0 and 100! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
				return;
			}
			if (args->argumentsArray.size() <3|| !regex_search(args->argumentsArray.at(2), oppModRegExp) || stoll(args->argumentsArray.at(2)) < -100 || stoll(args->argumentsArray.at(2))> 0) {
				string msgString = "------\n**Please enter a valid opp - mod value between - 100 and 0! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
				return;
			}
			if (args->argumentsArray.size() <4 ||!regex_search(args->argumentsArray.at(3), itemCostRegExp) || stoll(args->argumentsArray.at(3)) < 1) {
				string msgString = "------\n**Please enter a valid item cost! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
				return;
			}
			if (args->argumentsArray.size() < 5|| !regex_search(args->argumentsArray.at(4), emojiRegExp)) {
				string msgString = "------\n**Please enter a valid emoji! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
				return;
			}

			string itemName = args->argumentsArray.at(0);
			cmatch matchResults;
			regex_search(args->argumentsArray.at(1).c_str(), matchResults, selfModRegExp);
			unsigned int selfMod = (unsigned int)stoll(matchResults.str());
			regex_search(args->argumentsArray.at(2).c_str(), matchResults, oppModRegExp);
			int oppMod = (int)stoll(matchResults.str());
			regex_search(args->argumentsArray.at(3).c_str(), matchResults, itemCostRegExp);
			unsigned int itemCost = (unsigned int)stoll(matchResults.str());
			string emoji = args->argumentsArray.at(4);

			for (auto& value: discordGuild.data.guildShop.items) {
				if (itemName == value.itemName) {
					string msgString = "------\n**Sorry, but an item by that name already exists!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Item Issue:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
					return;
				}
			}

			InventoryItem newItem;
			newItem.emoji = emoji;
			newItem.itemCost = itemCost;
			newItem.itemName = itemName;
			newItem.oppMod = oppMod;
			newItem.selfMod = selfMod;

			discordGuild.data.guildShop.items.push_back(newItem);
			discordGuild.writeDataToDB();

			string msgString = "";
			msgString = "Good job! You've added a new item to the shop, making it available for purchase by the members of this server!\n\
				The item's stats are as follows:\n__Item Name__: " + itemName + "\n__Self-Mod Value__: " + to_string(selfMod) + "\n__Opp-Mod Value__: " + to_string(oppMod) + "\n\
				__Item Cost__: " + to_string(itemCost) + " " + args->coreClient->discordUser->data.currencyName + "\n__Emoji__: " + emoji;
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**New Shop Item Added:**__");
			args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
			return;
		}
	};

	AddShopItem addShopItem{};
}
#endif