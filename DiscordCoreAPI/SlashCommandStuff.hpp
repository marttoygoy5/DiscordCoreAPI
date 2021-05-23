// SlashCommandStuff.hpp - Stuff for slash commands.
// May 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SLASH_COMMAND_STUFF_
#define _SLASH_COMMAND_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
#include "HttpStuff.hpp"
#include "UserStuff.hpp"

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

	struct ApplicationCommandOptionChoiceData {
		string name;
		int	valueInt;
		int valueString;
	};

	struct ApplicationCommandOptionData {
		ApplicationCommandOptionType type;
		string name;
		string description;
		bool required;
		vector<ApplicationCommandOptionChoiceData>	choices;
		vector<ApplicationCommandOptionData> options;
	};

	struct SubCommandData {

	};

	struct CreateSlashCommandData {
		string applicationId;
		string name;
		string description;
		vector<ApplicationCommandOptionData> options;
		bool defaultPermission;
	};

	class SlashCommandManager {
	public:
		SlashCommandManager(DiscordCoreInternal::HttpAgentResources agentResources, Scheduler* pScheduler = nullptr)
			:httpPostAgent(agentResources, pScheduler)
		{

		}

		void createSlashCommand(CreateSlashCommandData createSlashCommandData) {

		}

	protected:
		DiscordCoreInternal::HttpRequestAgent httpPostAgent;

	};
}
#endif
