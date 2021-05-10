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
		CREATE_MESSAGE = 2, 
		DELETE_MESSAGE = 3, 
		GET_USER = 4, 
		GET_USER_SELF = 5, 
		GET_GUILD = 6, 
		GET_CHANNEL = 7, 
		REACTION_ADD_REMOVE = 8
	};

	struct RateLimitData {
		float msRemain = 0;
		int getsRemaining = 1;
		float timeStartedAt = 0;
		RateLimitType rateLimitType;
		string bucket;
	};
};
#endif
