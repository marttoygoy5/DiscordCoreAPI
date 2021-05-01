// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

namespace CommanderNS {

	namespace FoundationClasses {
		
		struct RateLimitation {
			int msRemain;
			int getsRemaining = 1;
			int currentMsTime = 0;
		};

	};

};
