// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES_
#define _FOUNDATION_CLASSES_

namespace CommanderNS {

	enum class RateLimitType {
		UNSET = 0,
		MESSAGE_DELETE = 1,
		MESSAGE_CREATE = 2,
		MESSAGE_GET = 3,
		REACTION_ADD_REMOVE = 4,
		GUILD_GET = 5,
		CHANNEL_GET = 6,
		GUILD_MEMBER_GET = 7,
		USER_GET = 8,
		USER_SELF_GET = 9,
		ROLES_GET = 10
	};

	struct RateLimitData {
		string bucket = "";
		float timeStartedAt = 0;
		RateLimitType rateLimitType = RateLimitType::UNSET;
		float msRemain = 0;
		unsigned int getsRemaining = 0;
	};

	struct HTTPData {
		json data;
	};
};
#endif
