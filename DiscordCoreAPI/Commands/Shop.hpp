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
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild.data);

				bool areWeInADm = areWeInADM(args->eventData, channel, discordGuild);

				if (areWeInADm ==  true) {
					co_return;
				}

				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventHandler::deleteInputEventResponse(args->eventData).get();
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

				if (areWeAllowed == false) {
					co_return;
				}

				InputEventData event01;
				GuildMember botMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.discordCoreClient->currentUser->data.id }).get();
				if (!(DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, Permissions::MANAGE_MESSAGES))) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this command!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
						responseData.channelId = args->eventData.messageData.channelId;
						responseData.messageId = args->eventData.messageData.id;
						responseData.embeds.push_back(msgEmbed);
						event01 = InputEventHandler::respondToEvent(responseData).get();
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

				vector<Role> rolesArray = args->eventData.discordCoreClient->roles->getGuildRolesAsync({ .guildId = args->eventData.getGuildId() }).get();

				for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x+=1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuild.data.guildShop.roles.at(x);
					for (unsigned int y = 0; y< rolesArray.size(); y += 1) {
						if (rolesArray.at(y).data.id== shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						for (unsigned int z = 0; z < discordGuild.data.guildShop.roles.size(); z += 1) {
							if (shopRole.roleId == discordGuild.data.guildShop.roles.at(z).roleId) {
								discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + z);
								discordGuild.writeDataToDB();
							}
						}
						string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild cache!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Removed Guild Role:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
							responseData.channelId = args->eventData.messageData.channelId;
							responseData.messageId = args->eventData.messageData.id;
							responseData.embeds.push_back(msgEmbed);
							event01 = InputEventHandler::respondToEvent(responseData).get();
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
						x -= 1;
					}
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
					rolesMsgEmbeds[x].setColor(discordGuild.data.borderColor);
					rolesMsgEmbeds[x].setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
				}

				for (auto x = 0; x < itemsMessageEmbeds.size(); x += 1) {
					itemsMessageEmbeds[x].setTimeStamp(getTimeAndDate());
					itemsMessageEmbeds[x].setTitle("__**Shop Inventory(Items) Page " + to_string(rolesMsgEmbeds.size() + x + 1) + " of " + to_string(itemsMessageEmbeds.size() + rolesMsgEmbeds.size()) + "**__:");
					itemsMessageEmbeds[x].setDescription(itemsMsgStrings[x]);
					itemsMessageEmbeds[x].setColor(discordGuild.data.borderColor);
					itemsMessageEmbeds[x].setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
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
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatar());
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
						responseData.channelId = args->eventData.messageData.channelId;
						responseData.messageId = args->eventData.messageData.id;
						responseData.embeds.push_back(messageEmbed);
						event01 = InputEventHandler::respondToEvent(responseData).get();
						InputEventHandler::deleteInputEventResponse(event01, 20000).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
						responseData.applicationId = args->eventData.interactionData.applicationId;
						responseData.embeds.push_back(messageEmbed);
						responseData.interactionId = args->eventData.interactionData.id;
						responseData.interactionToken = args->eventData.interactionData.token;
						responseData.type = InteractionCallbackType::ChannelMessage;
						event01 = InputEventHandler::respondToEvent(responseData).get();
						event01.interactionData.applicationId = args->eventData.interactionData.applicationId;
						event01.interactionData.token = args->eventData.interactionData.token;
						InputEventHandler::deleteInputEventResponse(event01, 20000).get();
					}
					co_return;
				}

				unsigned int currentPageIndex = 0;
				string userID = args->eventData.messageData.author.id;
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					event01.eventType = args->eventData.eventType;
					event01.messageData = args->eventData.messageData;
					event01.discordCoreClient = args->eventData.discordCoreClient;
					event01.requesterId = args->eventData.requesterId;
					recurseThroughMessagePages(userID, event01,  currentPageIndex, finalMsgEmbedsArray, true).get();
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					event01.eventType = args->eventData.eventType;
					event01.interactionData = args->eventData.interactionData;
					event01.discordCoreClient = args->eventData.discordCoreClient;
					event01.requesterId = args->eventData.requesterId;
					event01.messageData = args->eventData.messageData;
					recurseThroughMessagePages(userID, event01, currentPageIndex, finalMsgEmbedsArray, true).get();
				}

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