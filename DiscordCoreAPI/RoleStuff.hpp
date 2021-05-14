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
		Guild* guild{ nullptr };
		com_ptr<RoleManager> roles{ nullptr };
		DiscordCoreInternal::RoleData data;
		Role() {}
		Role(DiscordCoreInternal::RoleData roleData, DiscordCoreInternal::HttpAgentPointers pointersNew, com_ptr<RoleManager> rolesNew, Guild* guildNew) {
			this->data = roleData;
			this->pointers = pointersNew;
			this->guild = guildNew;
			this->roles = rolesNew;
		}
		~Role() {}
	protected:
		friend class RoleManager;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};
	class RoleManager: concurrent_unordered_map<string, Role>,public implements<RoleManager,winrt::Windows::Foundation::IInspectable> {

	public:
		Guild* guild{ nullptr };
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
			Role role;
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				try {
					role = this->at(jsonValue.at(x).at("id"));
					this->unsafe_erase(jsonValue.at(x).at("id"));
					DiscordCoreInternal::parseObject(jsonValue.at(x), &role.data);
					this->insert(make_pair(role.data.id, role));
				}
				catch (exception error) {
					com_ptr<RoleManager> pRoleManager;
					pRoleManager.attach(this);
					DiscordCoreInternal::RoleData roleData;
					role = Role(roleData, this->pointers, pRoleManager, this->guild);
					DiscordCoreInternal::parseObject(jsonValue.at(x), &role.data);
					this->insert(make_pair(role.data.id, role));
					cout << "fetchAsync() Error: " << error.what() << endl;
				}
			}
			co_return this->at(roleId);
		}

		task<Role> getGuildAsync(string roleId) {
			try {
				Role role = this->at(roleId);
				co_return role;
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::RoleData roleData;
				com_ptr<RoleManager> pRoleManager;
				pRoleManager.attach(this);
				Role role(roleData, this->pointers, pRoleManager, this->guild);
				co_return role;
			}
		}

		~RoleManager() {}
	protected:
		friend class Guild;
		friend struct DiscordCoreClient;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};
}
#endif