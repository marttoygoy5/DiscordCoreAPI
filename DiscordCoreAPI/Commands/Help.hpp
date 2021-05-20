// Help.hpp - Header for the "help" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELP_
#define _HELP_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	extern map<string, DiscordCoreAPI::DiscordCoreFunction> commands;
	extern shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreAPI;

	struct HelpArguments:DiscordCoreAPI::DiscordCoreFunctionBaseArguments {
		string commandName;
	};

	void Help(DiscordCoreAPI::DiscordCoreFunctionBaseArguments* arguments) {
		HelpArguments* args = (HelpArguments*)arguments;
		DiscordCoreAPI::DiscordCoreFunction helpCommand;
		helpCommand.commandName = "help";
		helpCommand.functionPtr = &Help;
	}

	

}
#endif
