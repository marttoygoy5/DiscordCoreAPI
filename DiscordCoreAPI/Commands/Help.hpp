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
		}

		virtual  void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				HelpCommandArguments* newArgs = (HelpCommandArguments*)args;
				Guild guild = newArgs->coreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
				Channel channel = newArgs->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();
				Role role = newArgs->coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				DiscordCoreInternal::Permissions permsArray[1] = { DiscordCoreInternal::Permissions::MANAGE_GUILD };
				string newPerms = DiscordCoreInternal::PermissionsConverter::addPermissionsToString(role.data.permissions, permsArray, 1);
				GuildMember guildMember = newArgs->coreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
				vector<Role> roles = newArgs->coreClient->roles->getGuildMemberRolesAsync({ .guildId = args->message.data.guildId, .guildMember = guildMember }).get();
				DiscordGuildMember guildMemberNew = args->coreClient->getDiscordGuildMember(guildMember.data);
				guildMemberNew.data.currency.wallet = 1111444;
				guildMemberNew.writeDataToDB();
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
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Help help{};
}
#endif
