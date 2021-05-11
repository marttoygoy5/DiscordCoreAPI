// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES
#define _FOUNDATION_CLASSES

namespace CommanderNS {

	struct HTTPData {
		json data;
	};

	enum class RateLimitType{
		UNSET =0, 
		GET_MESSAGE = 1,
		POST_MESSAGE = 2, 
		DELETE_MESSAGE = 3,
		GET_USER = 4, 
		GET_USER_SELF = 5, 
		GET_GUILD = 6, 
		GET_CHANNEL = 7, 
		GET_REACTION = 8,
		PUT_REACTION = 9,
		DELETE_REACTION = 10,
		GET_GUILD_MEMBER = 11,
		GET_ROLES = 12,
		GET_USER_GUILDS = 13
	};

	struct RateLimitData {
		int getsRemaining = 1;
		float msRemain = 0;
		float timeStartedAt = 0;
		RateLimitType rateLimitType;
		string bucket;
	};
};
#endif
