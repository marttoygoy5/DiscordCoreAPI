// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES
#define _FOUNDATION_CLASSES

namespace CommanderNS {

	namespace FoundationClasses {

		struct RateLimitation {
			int msRemain = 1;
			int getsRemaining = 1;
			int currentMsTime = 1;
		};
	};
};
#endif
