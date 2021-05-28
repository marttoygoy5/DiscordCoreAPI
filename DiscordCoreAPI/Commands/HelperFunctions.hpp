// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELPER_FUNCTIONS_
#define _HELPER_FUNCTIONS_

#include "pch.h"
#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	bool areWeInADM(Channel channel) {
		if (channel.data.type == DiscordCoreInternal::ChannelType::DM) {
			return true;
		}
		else {
			return false;
		};
	}

	bool checkIfAllowedGamingInChannel(Message message, DiscordGuildData guildData) {
		for (auto& value : guildData.gameChannelIds) {
			if (message.data.channelId == value) {
				return true;
			}
		}
		return false;
	}

}
#endif
