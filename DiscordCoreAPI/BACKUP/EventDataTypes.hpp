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
#include "SystemThreads.hpp"

namespace CommanderNS {

	namespace EventDataTypes {

		struct GuildMemberAddData {
			ClientClasses::GuildMember guildMember;
			std::string guildId;
			ThreadContext* threadContext;
		};

		struct MessageCreationData {
			ClientClasses::Message message;
			ThreadContext* threadContext;
		};

		struct MessageDeletionData {
			ClientClasses::Message message;
			ThreadContext* threadContext;
		};

		struct ReactionAddData {
			ClientClasses::Reaction reaction;
			ThreadContext* threadContext;
		};
	}
}
#endif
