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
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ .channelId = args->message.data.channelId }).get();

				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->message.data. != DiscordCoreInternal::EventType::SLASH_COMMAND_INTERACTION) {
					args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				Guild guild = args->coreClient->guilds->getGuildAsync({ .guildId = args->message.data.guildId }).get();
				DiscordGuild discordGuild(guild.data);

				if (args->argumentsArray.size() == 0) {
					string msgString = "------\n**Please enter an item name!(!buy = ITEMNAME)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
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

				GuildMember guildMember = args->coreClient->guildMembers->fetchAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
				DiscordGuildMember discordGuildMember(guildMember.data);

				vector<Role> rolesArray = args->coreClient->roles->getGuildMemberRolesAsync({ .guildId = args->message.data.guildId, .guildMember = guildMember }).get();
				for (unsigned int x = 0; x < rolesArray.size(); x += 1) {
					cout << rolesArray.at(x).data.name << endl;
				}
				
				for (unsigned int x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuildMember.data.roles.at(x);
					for (unsigned int y = 0; y < rolesArray.size(); y += 1) {
						if (rolesArray.at(y).data.id == shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						for (unsigned int z = 0; z < discordGuildMember.data.roles.size(); z += 1) {
							if (shopRole.roleId == discordGuildMember.data.roles.at(z).roleId) {
								discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + z);
							}
						}
						string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild member cache!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Removed Guild Member Role:**__");
						Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
						args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
						x -= 1;
					}
					discordGuildMember.writeDataToDB();
				}

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
					if (objectName == discordGuild.data.guildShop.roles.at(x).roleName) {
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
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Object:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}

				bool isFoundInInventory = false;

				for (unsigned int x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (objectName == discordGuildMember.data.items.at(x).itemName) {
						isFoundInInventory = true;
						break;
					}
				}

				for (unsigned int x = 0; x < discordGuildMember.data.roles .size(); x += 1) {
					if (objectName == discordGuildMember.data.roles.at(x).roleName) {
						isFoundInInventory = true;
						break;
					}
				}

				if (isFoundInInventory == true) {
					string msgString = "------\n**Sorry, but you already have one of those " + objectType + "s.** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Duplicate Object:**__");
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
					args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
					co_return;
				}

				if (objectType == "role") {
					unsigned int roleCost = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
					unsigned int userBalance = discordGuildMember.data.currency.wallet;

					if (roleCost > userBalance) {
						string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
						args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
						co_return;
					}

					InventoryRole newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
					discordGuildMember.data.roles.push_back(newRole);
					discordGuildMember.data.currency.wallet -= roleCost;
					discordGuildMember.writeDataToDB().get();

					unsigned int newBalance = discordGuildMember.data.currency.wallet;

					string roleID = discordGuild.data.guildShop.roles.at(objectShopIndex).roleId;

					args->coreClient->roles->addRoleToGuildMemberAsync({ .guildId = args->message.data.guildId, .userId = args->message.data.author.id, .roleId = roleID }).get();

					string msgString = "------\nCongratulations! You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ <@&" + newRole.roleId + "> (" + newRole.roleName + ")\n------\n__**Your new wallet balance:**__ " + to_string(newBalance) + " " + args->coreClient->discordUser->data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Role Purchased:**__");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
					
					unsigned int maxIdx = 0;
					InventoryRole tempItem;
					unsigned int len = (unsigned int)discordGuildMember.data.roles.size();
					for (unsigned int x = 0; x < len; x += 1) {
						maxIdx = x;
						for (unsigned int y = x + 1; y < len; y += 1) {
							if (discordGuildMember.data.roles.at(y).roleCost
						> discordGuildMember.data.roles.at(maxIdx).roleCost) {
								maxIdx = y;
							}
						}
						tempItem = discordGuildMember.data.roles.at(x);
						discordGuildMember.data.roles.at(x) = discordGuildMember.data.roles.at(maxIdx);
						discordGuildMember.data.roles.at(maxIdx) = tempItem;
					}
					discordGuildMember.writeDataToDB().get();
					co_return;
				}
				else if (objectType ==  "item") {
					unsigned int  itemCost = discordGuild.data.guildShop.items.at(objectShopIndex).itemCost;
					unsigned int userBalance = discordGuildMember.data.currency.wallet;

					if (itemCost > userBalance) {
						string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setDescription(msgString);
						msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
						args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 });
						co_return;
					}

					InventoryItem newItem = discordGuild.data.guildShop.items.at(objectShopIndex);
					discordGuildMember.data.items.push_back(newItem);
					discordGuildMember.data.currency.wallet -= itemCost;
					discordGuildMember.writeDataToDB().get();

					string itemEmoji = discordGuild.data.guildShop.items.at(objectShopIndex).emoji;
					string itemName = discordGuild.data.guildShop.items.at(objectShopIndex).itemName;
					unsigned int newBalance = discordGuildMember.data.currency.wallet;
					string msgString = "------\nCongratulations!You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ " + itemEmoji + itemName + "\n------\n__**Your new wallet balance:**__ " + to_string(newBalance) + " " + args->coreClient->discordUser->data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Item Purchased:**__");

					unsigned int maxIdx = 0;
					InventoryItem tempItem;
					unsigned int len = (unsigned int)discordGuildMember.data.items.size();
					for (unsigned int x = 0; x < len; x += 1) {
						maxIdx = x;
						for (unsigned int y = x + 1; y < len; y += 1) {
							if (discordGuildMember.data.items.at(y).itemCost
						> discordGuildMember.data.items.at(maxIdx).itemCost) {
								maxIdx = y;
							}
						}
						tempItem = discordGuildMember.data.items.at(x);
						discordGuildMember.data.items.at(x) = discordGuildMember.data.items.at(maxIdx);
						discordGuildMember.data.items.at(maxIdx) = tempItem;
					}
					discordGuildMember.writeDataToDB().get();
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
					co_return;
				}
				co_return;
			}
			catch (exception& e) {
				cout << "Buy::execute() Error: " << e.what() << endl << endl;
			}

			
			}
		};
		Buy buy{};
	};
	

#endif
