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

	class HelpCommand : public  BaseFunction {
	public:
		virtual  void execute(DiscordCoreAPI::DiscordCoreFunctionBaseArguments args) {
			try {
				Guild guild = args.coreClient->guilds->getGuildAsync({ args.message.data.guildId }).get();
				Channel channel = args.coreClient->channels->getChannelAsync({ args.message.data.channelId }).get();
				Role role = args.coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				DiscordCoreInternal::Permissions permsArray[1] = { DiscordCoreInternal::Permissions::MANAGE_GUILD };
				cout << "ROLE NAME: " << role.data.name << endl;
				string newPerms = DiscordCoreInternal::PermissionsConverter::addPermissionsToString(role.data.permissions, permsArray, 1);
				GuildMember guildMember = args.coreClient->guildMembers->getGuildMemberAsync({ .guildId = args.message.data.guildId, .guildMemberId = args.message.data.author.id }).get();
				vector<Role> roles = args.coreClient->roles->getGuildMemberRoles({ .guildId = args.message.data.guildId, .guildMember = guildMember }).get();
				for (unsigned int x = 0; x < roles.size(); x += 1) {
					cout << "ROLE NAME: " << roles.at(x).data.name << endl;
				}
				cout << "CURRENT USER ID: " << args.coreClient->currentUser->data.id << endl;
				Role newRole = args.coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				cout << "GET ROLE: " << role.data.name << endl;
				newRole = role.coreClient->roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
				cout << "FATCH ROLE: " << role.data.name << endl;
				vector<ApplicationCommandOptionData> appCommandOptionDataVector;
				ApplicationCommandOptionChoiceData appCommandOptionChoiceData;
				appCommandOptionChoiceData.name = "THEtestchoice";
				appCommandOptionChoiceData.valueInt = 234;
				ApplicationCommandOptionData appCommandOptionData;
				appCommandOptionData.description = "Test Description";
				appCommandOptionData.name = "anoption";
				appCommandOptionData.type = ApplicationCommandOptionType::SUB_COMMAND;
				ApplicationCommandOptionData subCommand;
				subCommand.description = "THE SUBCOMMAND";
				subCommand.name = "subcommand";
				subCommand.required = true;
				subCommand.type = ApplicationCommandOptionType::INTEGER;
				subCommand.choices.push_back(appCommandOptionChoiceData);
				appCommandOptionData.options.push_back(subCommand);
				appCommandOptionDataVector.push_back(appCommandOptionData);
				CreateSlashCommandData createSlashCommandData;
				createSlashCommandData.description = "TEST DESCRIPTION";
				createSlashCommandData.name = "testname";
				createSlashCommandData.defaultPermission = true;
				createSlashCommandData.options = appCommandOptionDataVector;
				createSlashCommandData.applicationId = args.coreClient->currentUser->data.id;
				args.coreClient->slashCommands->createSlashCommandAsync(createSlashCommandData);
				args.coreClient->slashCommands->displaySlashCommandsAsync({ .applicationId = args.coreClient->currentUser->data.id });
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

				for (unsigned int x = 0; x < args.argumentsArray.size(); x += 1) {
					cout << args.argumentsArray.at(x) << endl;
				}
			}
			catch (exception error) {
				cout << "HelpCommand::execute() Error: " << error.what() << endl << endl;
			}
			
		}
	};

	HelpCommand helpCommand{};
}
#endif.
