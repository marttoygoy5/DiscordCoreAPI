// AddShopRole.hpp - Header for the "add shop role" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ADD_SHOP_ROLE_
#define _ADD_SHOP_ROLE_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class AddShopRole :public BaseFunction {
	public:
		AddShopRole() {
			this->commandName = "addshoprole";
			this->helpDescription = "__**Add Shop Role:**__ Enter !addshoprole = NAME, HEXCOLORVALUE, COST or /addshoprole NAME, HEXCOLORVALUE, COST.";
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				Channel channel = args->coreClient->channels->getChannelAsync({ .channelId = args->message.data.channelId }).get();
				bool areWeInADm = areWeInADM(args->message, channel);

				if (areWeInADm == true) {
					return;
				}

				if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::INTERACTION) {
					args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				bool doWeHaveAdmin = doWeHaveAdminPermissions(args->message);

				if (doWeHaveAdmin == false) {
					return;
				}
				Guild guild = args->coreClient->guilds->getGuildAsync({ .guildId = args->message.data.guildId }).get();

				DiscordGuild discordGuild(guild.data);

				regex nameRegExp("\.{1,36}");
				regex hexColorRegExp("\.{1,24}");
				regex costRegExp("\\d{1,8}");
				if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), nameRegExp)) {
					string msgString = "------\n**Please enter a proper role name! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
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
				if (args->argumentsArray.size() < 2 || !regex_search(args->argumentsArray.at(1), hexColorRegExp)) {
					string msgString = "------\n**Please enter a valid hex color value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
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
				if (args->argumentsArray.size() < 3 || !regex_search(args->argumentsArray.at(2), costRegExp) || stoll(args->argumentsArray.at(2)) <= 0) {
					string msgString = "------\n**Please enter a valid cost value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
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

				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, nameRegExp);
				string roleName = matchResults.str();
				regex_search(args->argumentsArray.at(1).c_str(), matchResults, hexColorRegExp);
				string roleColor = matchResults.str();
				regex_search(args->argumentsArray.at(2).c_str(), matchResults, costRegExp);
				unsigned int roleCost = (unsigned int)stoll(matchResults.str());

				for (auto& value : discordGuild.data.guildShop.roles) {
					if (roleName == value.roleName) {
						string msgString = "------\n**Sorry, but a role by that name already exists!**\n------";
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
				}

				string rolePermsString;
				DiscordCoreInternal::Permissions permissions[13] = { DiscordCoreInternal::Permissions::CREATE_INSTANT_INVITE, DiscordCoreInternal::Permissions::ADD_REACTIONS, DiscordCoreInternal::Permissions::VIEW_CHANNEL,
				DiscordCoreInternal::Permissions::SEND_MESSAGES, DiscordCoreInternal::Permissions::CHANGE_NICKNAME, DiscordCoreInternal::Permissions::USE_EXTERNAL_EMOJIS,DiscordCoreInternal::Permissions::CONNECT, DiscordCoreInternal::Permissions::EMBED_LINKS,
				DiscordCoreInternal::Permissions::ATTACH_FILES, DiscordCoreInternal::Permissions::SPEAK, DiscordCoreInternal::Permissions::USE_SLASH_COMMANDS, DiscordCoreInternal::Permissions::READ_MESSAGE_HISTORY, DiscordCoreInternal::Permissions::STREAM };
				rolePermsString = DiscordCoreAPI::PermissionsConverter::addPermissionsToString(rolePermsString, permissions, 13);

				CreateRoleData createRoleData;
				createRoleData.color = roleColor;
				createRoleData.guildId = args->message.data.guildId;
				createRoleData.hoist = true;
				createRoleData.mentionable = true;
				createRoleData.name = roleName;
				createRoleData.permissions = rolePermsString;
				Role role = args->coreClient->roles->createRoleAsync(createRoleData).get();
				if (role.data.id == "") {
					throw exception("Role not initialized!");
				}

				InventoryRole currentRole;
				currentRole.roleCost = roleCost;
				currentRole.roleId = role.data.id;
				currentRole.roleName = role.data.name;

				discordGuild.data.guildShop.roles.push_back(currentRole);
				discordGuild.writeDataToDB();

				string msgString = "";
				msgString = "Nicely done! You've added a new role to the store's inventory, giving the server access to it!\nIt is as follows:\n------\n__**Role:**__ <@&" + currentRole.roleId + ">__**Cost**__ : " + to_string(roleCost) + " "
					+ args->coreClient->discordUser->data.currencyName + "\n------";
				DiscordCoreInternal::EmbedData msgEmbed;
				msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Role Added:**__");
				args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
				return;
			}
			catch (exception& e) {
				cout << "AddShopRole::execute() Error: " << e.what() << endl;
			}
		}
	};
	AddShopRole addShopRole{};
}
#endif