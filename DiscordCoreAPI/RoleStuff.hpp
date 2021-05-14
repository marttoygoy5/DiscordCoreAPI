// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"

namespace DiscordCoreAPI {
	class RoleManager;

	class Role {
	public:
		RoleManager* roles;
		DiscordCoreInternal::RoleData data;
		Role() {}
		Role(DiscordCoreInternal::RoleData roleData, DiscordCoreInternal::HttpAgentPointers pointersNew, RoleManager* rolesNew) {
			this->data = roleData;
			this->pointers = pointersNew;
			this->roles = rolesNew;
		}
		~Role() {}
	protected:
		friend class RoleManager;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};

	class RoleManager: map<string, Role> {
	public:
		Guild* guild;
		RoleManager() {}
		RoleManager(Guild* guildNew, DiscordCoreInternal::HttpAgentPointers pointers) {
			this->pointers = pointers;
			this->guild = guildNew;
		}

		task<Role> fetchAsync(string guildId, string roleId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + guildId + "/roles";
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			DiscordCoreInternal::RoleData roleData;
			Role role(roleData, this->pointers, this);
			try {
				role = this->at(roleId);
				this->erase(guildId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &role.data);
			this->insert(make_pair(roleId, role));
			co_return role;
		}

		task<Role> getGuildAsync(string roleId) {
			try {
				Role role = this->at(roleId);
				co_return role;
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::RoleData roleData;
				Role role(roleData, this->pointers, this);
				co_return role;
			}
		}

		~RoleManager() {}
	protected:
		friend struct DiscordCoreClient;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};
}
#endif
