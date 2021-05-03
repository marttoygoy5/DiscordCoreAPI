// EventDataTypes.hpp - Header for the event data types.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_DATA_TYPES_
#define _EVENT_DATA_TYPES_

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "ClientClasses.hpp"

namespace CommanderNS {
	namespace EventDataTypes {
		struct GuildMemberAddData {
			std::string guildId;
			ClientClasses::GuildMember guildMember;
		};

		struct MessageCreationData {
			ClientClasses::Message message;
		};
	}
}
#endif