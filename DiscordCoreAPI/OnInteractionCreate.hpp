// OnInteractionCreate.hpp - Header for interaction-dealings-withs.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ON_INTERACTION_CREATE_
#define _ON_INTERACTION_CREATE_

#include "pch.h"
#include "SlashCommandStuff.hpp"

namespace DiscordCoreInternal {

	string constructStringContent(InteractionIncData interactionIncData) {
		string finalCommandString;
		finalCommandString = interactionIncData.commandName;
		for (auto& value : interactionIncData.optionsArgs) {
			finalCommandString += value + ", ";
		}
		return finalCommandString;
	}
}
#endif
