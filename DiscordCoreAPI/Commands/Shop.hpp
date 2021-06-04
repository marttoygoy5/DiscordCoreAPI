// Shop.hpp - Header for the "shop" command.
// June 2, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SHOP_
#define _SHOP_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Shop : public BaseFunction {
	public:
		Shop() {
			this->commandName = "shop";
			this->helpDescription = "__**Shop:**__ Enter !shop or /shop!";
		}

		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();
				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm ==  true) {
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

				GuildMember botMember = args->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->coreClient->currentUser->data.id }).get();
				if (!(DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, DiscordCoreInternal::Permissions::MANAGE_MESSAGES))) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this command!**\n------";
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

				vector<Role> rolesArray = args->coreClient->roles->getGuildRolesAsync({ .guildId = args->message.data.guildId }).get();

				for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x+=1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuild.data.guildShop.roles.at(x);
					for (unsigned int y = 0; y< rolesArray.size(); y += 1) {
						if (rolesArray.at(y).data.id== shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound ==  false) {
						for (unsigned int z = 0; z < discordGuild.data.guildShop.roles.size(); z += 1) {
							if (shopRole.roleId == discordGuild.data.guildShop.roles.at(z).roleId) {
								discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + z);
							}
						}
						string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild cache!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Removed Guild Role:**__");
						Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
						args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
						x -= 1;
					}
					discordGuild.writeDataToDB();
				}
				
				unsigned int maxIdx = 0;
				InventoryItem tempItem;
				unsigned int len = (unsigned int)discordGuild.data.guildShop.items.size();
				for (unsigned int x = 0; x < len; x += 1) {
					maxIdx = x;
					for (unsigned int  y = x + 1; y < len; y += 1) {
						if (discordGuild.data.guildShop.items.at(y).itemCost
					> discordGuild.data.guildShop.items.at(maxIdx).itemCost) {
							maxIdx = y;
						}
					}
					tempItem = discordGuild.data.guildShop.items.at(x);
					discordGuild.data.guildShop.items.at(x) = discordGuild.data.guildShop.items.at(maxIdx);
					discordGuild.data.guildShop.items.at(maxIdx) = tempItem;
				}

				maxIdx = 0;
				InventoryRole tempRole;
				len = (unsigned int)discordGuild.data.guildShop.roles.size();
				for (unsigned int x = 0; x < len; x += 1) {
					maxIdx = x;
					for (unsigned int y = x + 1; y < len; y += 1) {
						if (discordGuild.data.guildShop.roles.at(y).roleCost
					> discordGuild.data.guildShop.roles.at(maxIdx).roleCost) {
							maxIdx = y;
						}
					}
					tempRole = discordGuild.data.guildShop.roles.at(x);
					discordGuild.data.guildShop.roles.at(x) = discordGuild.data.guildShop.roles.at(maxIdx);
					discordGuild.data.guildShop.roles.at(maxIdx) = tempRole;
				}
				
				vector<string> itemsMsgStrings;
				vector<EmbedData> itemsMessageEmbeds;
				unsigned int currentPage = 0;				
				bool firstLoop = true;

				for (auto& value:discordGuild.data.guildShop.items) {
					if (firstLoop) {
						itemsMsgStrings.resize(1);
						itemsMessageEmbeds.resize(1);
						firstLoop = false;
					}
					string itemsMsgStringTemp;
					itemsMsgStringTemp = "| __**Item:**__ " + value.emoji + " " + value.itemName + " **| Cost**: " + to_string(value.itemCost) + " **| Self-Mod**: " + to_string(value.selfMod) + "**| Opp-Mod**: " + to_string(value.oppMod) + "\n";
						if (itemsMsgStringTemp.length() + itemsMsgStrings.at(currentPage).length() >= 2048) {
							currentPage += 1;
							itemsMsgStrings.resize(currentPage);
							itemsMessageEmbeds.resize(currentPage);
						}
						itemsMessageEmbeds[currentPage] = *new EmbedData();
						itemsMsgStrings[currentPage] += itemsMsgStringTemp;
				}

				vector<string> rolesMsgStrings;
				vector<EmbedData> rolesMsgEmbeds;
				unsigned int currentPage2 = 0;
				bool firstLoop2 = true;

				for (auto& value:discordGuild.data.guildShop.roles) {
					if (firstLoop2) {
						rolesMsgStrings.resize(1);
						rolesMsgEmbeds.resize(1);
						firstLoop2 = false;
					}
					string rolesMsgStringTemp;
					rolesMsgStringTemp = "| __**Role:**__ <@&" + value.roleId + ">**| Cost : **" + to_string(value.roleCost) + "\n";
						if (rolesMsgStringTemp.length() + rolesMsgStrings[currentPage2].length() > 2048) {
							currentPage2 += 1;
							rolesMsgStrings.resize(currentPage2);
							rolesMsgEmbeds.resize(currentPage2);
						}
						rolesMsgEmbeds[currentPage2] = *new EmbedData;
						rolesMsgStrings[currentPage2] += rolesMsgStringTemp;
				}

				for (auto x = 0; x < rolesMsgEmbeds.size(); x += 1) {
					rolesMsgEmbeds[x].setTimeStamp(getTimeAndDate());
					rolesMsgEmbeds[x].setTitle("__**Shop Inventory(Roles) Page " + to_string(x + 1) + " of " + to_string(itemsMessageEmbeds.size() + rolesMsgEmbeds.size()) + "**__");
					rolesMsgEmbeds[x].setDescription(rolesMsgStrings[x]);
					rolesMsgEmbeds[x].setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					rolesMsgEmbeds[x].setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				}

				for (auto x = 0; x < itemsMessageEmbeds.size(); x += 1) {
					itemsMessageEmbeds[x].setTimeStamp(getTimeAndDate());
					itemsMessageEmbeds[x].setTitle("__**Shop Inventory(Items) Page " + to_string(rolesMsgEmbeds.size() + x + 1) + " of " + to_string(itemsMessageEmbeds.size() + rolesMsgEmbeds.size()) + "**__:");
					itemsMessageEmbeds[x].setDescription(itemsMsgStrings[x]);
					itemsMessageEmbeds[x].setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					itemsMessageEmbeds[x].setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				}

				vector<EmbedData> finalMsgEmbedsArray;

				for (auto x = 0; x < rolesMsgEmbeds.size(); x += 1) {
					finalMsgEmbedsArray.push_back(rolesMsgEmbeds[x]);
				}

				for (auto x = 0; x < itemsMessageEmbeds.size(); x += 1) {
					finalMsgEmbedsArray.push_back(itemsMessageEmbeds[x]);
				}				

				if (rolesMsgEmbeds.size()==  0 && itemsMessageEmbeds.size()== 0) {
					string msgString = "Sorry, but we are all out of inventory!";
					EmbedData messageEmbed;
					messageEmbed.setDescription(msgString);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setTitle("__**Empty Inventory:**__");
					messageEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
					messageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = messageEmbed }).get();
					co_return;
				}

				unsigned int currentPageIndex = 0;
				string userID = args->message.data.author.id;
				ReplyMessageData replyMessageData;
				replyMessageData.embed = finalMsgEmbedsArray[currentPageIndex];
				replyMessageData.replyingToMessageData = args->message.data;
				Message newMessage = args->coreClient->messages->replyAsync(replyMessageData).get();
				
				recurseThroughMessagePages(userID, newMessage, args->coreClient, currentPageIndex, finalMsgEmbedsArray, true).get();

				discordGuild.writeDataToDB().get();
				co_return;
			}
			catch (exception& e) {
				cout << "Shop::execute() Error: " << e.what() << endl << endl;
			}
		}
	};
	Shop shop{};
}
#endif