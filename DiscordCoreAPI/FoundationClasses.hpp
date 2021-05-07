// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES_
#define _FOUNDATION_CLASSES_

namespace CommanderNS {

	namespace FoundationClasses {

		enum class RateLimitType {
			MESSAGE_DELETE = 0,
			MESSAGE_CREATE = 1,
			MESSAGE_GET = 2,
			REACTION_ADD_REMOVE = 3,
			GUILD_GET = 4,
			CHANNEL_GET = 5
		};

		struct RateLimitData {
			string bucket = "";
			float timeStartedAt = 0;
			RateLimitType rateLimitType;
			float msRemain = 0;
			unsigned int getsRemaining = 0;
		};
	};
};
#endif
