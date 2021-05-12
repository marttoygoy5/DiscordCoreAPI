// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES
#define _FOUNDATION_CLASSES

#include "ClientDataTypes.hpp"

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
		DELETE_ALL_REACTION = 11,
		GET_GUILD_MEMBER = 12,
		GET_GUILD_MEMBERS = 13,
		GET_ROLES = 14,
		GET_USER_GUILDS = 15
	};

	struct RateLimitData {
		int getsRemaining = 1;
		float msRemain = 0;
		float timeStartedAt = 0;
		RateLimitType rateLimitType;
		string bucket;
	};

	class PermissionsConverter {
	public:
		static bool checkForPresence(ClientDataTypes::Permissions permission, string permissionString) {
			cout << permissionString << "  THE STRING" << endl;
			int permissionsInteger = stoll(permissionString);
			if ((permissionsInteger & (int)permission) == (int)permission) {
				return true;
			}
			else {
				return false;
			}
		}
	};
};
#endif
