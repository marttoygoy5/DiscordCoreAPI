// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"

namespace DiscordCoreAPI {
	class Role : DiscordCoreInternal::RoleData {
	public:
		Role() { }
	proteced:

	};
	class RoleManager {
	public:
		string guildId;
		RoleManager(string guildIdNew) {
			this->guildId = guildIdNew;
		}
	protected:

	};
}
#endif
