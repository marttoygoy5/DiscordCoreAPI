// EventDataTypes.hpp - Header for the event data types.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "ClientDataTypes.hpp"

namespace CommanderNS {
	namespace EventDataTypes {
		struct GuildMemberAddData {
			std::string guildId;
			ClientDataTypes::GuildMemberData guildMemberData;
		};

		struct MessageCreationData {
			ClientDataTypes::MessageData messageData;
		};
	}
}