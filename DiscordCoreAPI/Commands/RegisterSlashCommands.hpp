// RegisterSlashCommands.hpp - Registers the slash commands of this bot.
// May 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REGISTER_SLASH_COMMANDS_
#define _REGISTER_SLASH_COMMANDS_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {
	
	class RegisterSlashCommands :public BaseFunction {
	public:
		RegisterSlashCommands() {
			this->commandName = "registerslashcommands";
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			CreateApplicationCommandData createSellDrugsCommandData;
			createSellDrugsCommandData.defaultPermission = true;
			createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createSellDrugsCommandData).get();
			CreateApplicationCommandData registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.defaultPermission = true;
			registerSlashCommandsCommandData.description = "Register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(registerSlashCommandsCommandData).get();
		}
	};
	RegisterSlashCommands registerSlashCommands{};
}
#endif
