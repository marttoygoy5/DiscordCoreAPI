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
	extern shared_ptr<DiscordCoreClient> pDiscordCoreAPI;

	class HelpCommand : public  BaseFunction {
	public:
		virtual  void execute(DiscordCoreAPI::DiscordCoreFunctionBaseArguments args) {
			Guild guild = args.pClient->guilds->fetchAsync({ args.message.data.guildId }).get();
			Channel channel = guild.channels->fetchAsync({ args.message.data.channelId }).get();

			Role role = args.pClient->roles->fetchAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" }).get();
			DiscordCoreInternal::Permissions permsArray[1] = { DiscordCoreInternal::Permissions::MANAGE_GUILD };
			string newPerms = DiscordCoreInternal::PermissionsConverter::addPermissionsToString(role.data.permissions, permsArray, 1);
			role.roles->getRoleAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" });
			cout << "GET ROLE: " << role.data.name << endl;
			role.roles->fetchAsync({ .guildId = guild.data.id, .roleId = "790460906450583592" });
			cout << "FATCH ROLE: " << role.data.name << endl;
			DiscordCoreAPI::UpdateRoleData updateRoleData;
			updateRoleData.guildId = guild.data.id;
			updateRoleData.colorFirst[0] = 0;
			updateRoleData.colorFirst[1] = 255;
			updateRoleData.colorFirst[2] = 255;
			updateRoleData.hoist = true;
			updateRoleData.mentionable = true;
			updateRoleData.name = "TESTING ROLE THREE";
			updateRoleData.permissions = newPerms;
			updateRoleData.roleId = "790460906450583592";
			role = role.roles->updateRoleAsync(updateRoleData).get();

			for (unsigned int x = 0; x < args.argumentsArray.size(); x += 1) {
				cout << args.argumentsArray.at(x) << endl;
			}
		}
	};

	HelpCommand helpCommand{};
}
#endif.
