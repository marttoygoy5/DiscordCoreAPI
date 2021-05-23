// SlashCommandStuff.hpp - Stuff for slash commands.
// May 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SLASH_COMMAND_STUFF_
#define _SLASH_COMMAND_STUFF_

#include "pch.h"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	enum class ApplicationCommandOptionType {
		SUB_COMMAND = 1,
		SUB_COMMAND_GROUP = 2,
		STRING = 3,
		INTEGER = 4,
		BOOLEAN = 5,
		USER = 6,
		CHANNEL = 7,
		ROLE = 8,
		MENTIONABLE = 9
	};

	struct SubCommandData {

	};

	struct SubCommandGroupData {
		SubCommandData* subCommands{ nullptr };
		unsigned int subCommandCount{ 0u };
		void addSubCommand(SubCommandData subCommandData) {
			this->subCommands = new SubCommandData[this->subCommandCount + 1];
			this->subCommands[subCommandCount] = subCommandData;
			this->subCommandCount += 1;
		}
	};

	struct CreateSlashCommandData {

	};

	class SlashCommandManager {
	public:
		SlashCommandManager(DiscordCoreInternal::HttpAgentResources agentResources, Scheduler* pScheduler = nullptr)
			:httpPostAgent(agentResources, pScheduler)
		{

		}

	protected:
		DiscordCoreInternal::HttpRequestAgent httpPostAgent;

	};
}
#endif
