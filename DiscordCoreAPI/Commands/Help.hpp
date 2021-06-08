// Help.hpp - Header for the "help" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELP_
#define _HELP_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	struct HelpCommandArguments : BaseFunctionArguments {
		unsigned int inValue;
	};

	class Help : public  BaseFunction {
	public:
		Help() {
			this->commandName = "help";
			this->helpDescription = "__**Help:**__ Enter !help or /help for help!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync({ .channelId = args->eventData.getChannelId(), .limit = 25, .beforeThisId = args->eventData.getMessageId() }).get();

				/*
				vector<Role> guildRoles = args->coreClient->roles->getGuildRolesAsync({ .guildId = args->message.data.guildId }).get();
				for (auto role : guildRoles) {
					cout << "ROLE NAME: " << role.data.name << endl;
				}				

				UpdateRoleData roleData;
				roleData.hexColorValue = "FF33DD";
				roleData.guildId = args->message.data.guildId;
				roleData.hoist = true;
				roleData.mentionable = true;
				roleData.roleId = "850239731186663435";
				roleData.name = "NEW ROLE AGAIN";
				string permsString;
				vector<DiscordCoreInternal::Permissions> perms;
				perms.push_back(DiscordCoreInternal::Permissions::ADD_REACTIONS);
				perms.push_back(DiscordCoreInternal::Permissions::ADMINISTRATOR);
				roleData.permissions = PermissionsConverter::addPermissionsToString(permsString, perms);
				Role role = args->coreClient->roles->updateRoleAsync(roleData).get();
				args->coreClient->guilds->fetchAsync({ .guildId = args->message.data.guildId }).get();
				args->coreClient->roles->addRoleToGuildMemberAsync({ .guildId = args->message.data.guildId, .userId = args->message.data.author.id, .roleId = role.data.id }).get();
				args->coreClient->roles->removeRoleFromGuildMemberAsync({ .guildId = args->message.data.guildId, .userId = args->message.data.author.id, .roleId = role.data.id }).get();
				string allowPerms;
				vector<DiscordCoreInternal::Permissions> permsVector;
				permsVector.push_back(DiscordCoreInternal::Permissions::ADD_REACTIONS);
				permsVector.push_back(DiscordCoreInternal::Permissions::ATTACH_FILES);
				permsVector.push_back(DiscordCoreInternal::Permissions::CREATE_INSTANT_INVITE);
				permsVector.push_back(DiscordCoreInternal::Permissions::MANAGE_NICKNAMES);
				allowPerms = PermissionsConverter::addPermissionsToString(allowPerms, permsVector);
				permsVector.clear();
				permsVector.push_back(DiscordCoreInternal::Permissions::MANAGE_CHANNELS);
				permsVector.push_back(DiscordCoreInternal::Permissions::MANAGE_THREADS);
				string denyPerms;
				denyPerms = PermissionsConverter::addPermissionsToString(denyPerms, permsVector);
				args->coreClient->channels->editChannelPermissionOverwritesAsync({ .allow = allowPerms, .deny = denyPerms, .type = 1  ,.roleOrUserId = args->message.data.author.id, .channelId = args->message.data.channelId }).get();
				args->coreClient->channels->deleteChannelPermissionOverwritesAsync({ .channelId = args->message.data.channelId, .roleOrUserId = args->message.data.author.id }).get();
				//Role newRole = args->coreClient->roles->createRoleAsync(roleData).get();
				
				//HelpCommandArguments* newArgs = (HelpCommandArguments*)args;
				//args->coreClient->slashCommands->displayGlobalApplicationCommandsAsync().get();
				/*
				args->coreClient->slashCommands->deleteGlobalApplicationCommand({ .applicationId = args->coreClient->currentUser->data.id, .name = "testname" }).get();

				Role newRole = args->coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				newRole = role.coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				vector<ApplicationCommandOptionData> appCommandOptionDataVector;
				ApplicationCommandOptionChoiceData appCommandOptionChoiceData;
				appCommandOptionChoiceData.name = "THEtestchoice";
				appCommandOptionChoiceData.valueInt = 234;
				ApplicationCommandOptionData appCommandOptionData;
				appCommandOptionData.description = "The SubCommand";
				appCommandOptionData.name = "anoption";
				appCommandOptionData.type = ApplicationCommandOptionType::SUB_COMMAND;
				ApplicationCommandOptionData subCommand;
				subCommand.description = "The first option.";
				subCommand.name = "optionone";
				subCommand.required = true;
				subCommand.type = ApplicationCommandOptionType::INTEGER;
				subCommand.choices.push_back(appCommandOptionChoiceData);
				appCommandOptionData.options.push_back(subCommand);
				appCommandOptionDataVector.push_back(appCommandOptionData);
				CreateApplicationCommandData createSlashCommandData;
				createSlashCommandData.description = "TEST DESCRIPTION";
				createSlashCommandData.name = "testname";
				createSlashCommandData.defaultPermission = true;
				createSlashCommandData.options = appCommandOptionDataVector;
				createSlashCommandData.applicationId = args->coreClient->currentUser->data.id;
				//ApplicationCommand appCommand = args->coreClient->slashCommands->createApplicationCommandAsync(createSlashCommandData).get();
				//cout << "NAME: " << appCommand.data.name << " Id: " << appCommand.data.id << endl;
				EditApplicationCommandData editAppCommandData;
				editAppCommandData.defaultPermission = true;
				editAppCommandData.description = createSlashCommandData.description;
				editAppCommandData.name = createSlashCommandData.name;
				editAppCommandData.options = createSlashCommandData.options;
				editAppCommandData.applicationId = args->coreClient->currentUser->data.id;
				ApplicationCommand appCommandEdit = args->coreClient->slashCommands->editGlobalApplicationCommandAsync(editAppCommandData).get();

				DiscordCoreAPI::UpdateRoleData updateRoleData;
				updateRoleData.guildId = guild.data.id;
				updateRoleData.colorFirst[0] = (int)(((float)rand() / (float)RAND_MAX) * 255.0f);
				updateRoleData.colorFirst[1] = (int)(((float)rand() / (float)RAND_MAX) * 255.0f);
				updateRoleData.colorFirst[2] = (int)(((float)rand() / (float)RAND_MAX) * 255.0f);
				updateRoleData.hoist = true;
				updateRoleData.mentionable = true;
				updateRoleData.name = "TESTING ROLE THREE";
				updateRoleData.permissions = newPerms;
				updateRoleData.roleId = "790460906450583592";
				role = role.coreClient->roles->updateRoleAsync(updateRoleData).get();
				*/
				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Help help{};
}
#endif
