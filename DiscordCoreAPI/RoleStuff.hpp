// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class RoleManager;

	class Guild;

	class Role {
	public:
		Guild* guild{ nullptr };
		RoleManager* roles{ nullptr };
		DiscordCoreInternal::RoleData data;
		Role() {}

	protected:
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;

		Role(DiscordCoreInternal::RoleData roleData, DiscordCoreInternal::HttpAgentResources agentResourcesNew, RoleManager* rolesNew, Guild* guildNew) {
			this->data = roleData;
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->roles = rolesNew;
		}
	};

	struct GetRoleData {
		string guildId;
		string roleId;
	};

	class RoleManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestGetBuffer;
		static unbounded_buffer<Role>* outBuffer;
		static concurrent_queue<Role> RolesToInsert;
		static map<string, Role> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreInternal::ThreadContext threads;
		DiscordCoreAPI::RoleManager* pRoleManager;
		Guild* pGuild;

		RoleManagerAgent(DiscordCoreInternal::ThreadContext threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::RoleManager* pRoleManagerNew, Guild* pGuildNew)
			:agent(*threadsNew.scheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pRoleManager = pRoleManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			RoleManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetRoleData>;
			RoleManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetRoleData>;
			RoleManagerAgent::outBuffer = new unbounded_buffer<Role>;
			co_return;
		}

		task<Role> getRoleAsync(DiscordCoreInternal::GetRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				DiscordCoreInternal::RoleData roleData;
				if (RoleManagerAgent::cache.contains(jsonValue.at(x).at("id"))) {
					roleData = RoleManagerAgent::cache.at(jsonValue.at(x).at("id")).data;
					RoleManagerAgent::cache.erase(jsonValue.at(x).at("id"));
					DiscordCoreInternal::parseObject(jsonValue, &roleData);
					Role newRole(roleData, this->agentResources, this->pRoleManager, this->pGuild);
				}
				else {
					DiscordCoreInternal::parseObject(jsonValue, &roleData);
					Role newRole(roleData, this->agentResources, this->pRoleManager, this->pGuild);
				}
			}
			Role roleNew = RoleManagerAgent::cache.at(dataPackage.roleId);
			co_return roleNew;
		}

		void run() {
			DiscordCoreInternal::GetRoleData getData;
			try {
				getData = receive(RoleManagerAgent::requestGetBuffer, 1U);
				if (RoleManagerAgent::cache.contains(getData.roleId)) {
					Role Role = RoleManagerAgent::cache.at(getData.roleId);
					send(RoleManagerAgent::outBuffer, Role);
				}
				else {
					Role Role;
					send(RoleManagerAgent::outBuffer, Role);
				}
			}
			catch (exception error) {}
			try {
				getData = receive(RoleManagerAgent::requestFetchBuffer, 1U);
				if (RoleManagerAgent::cache.contains(getData.roleId)) {
					DiscordCoreInternal::RoleData oldRoleData = RoleManagerAgent::cache.at(getData.roleId).data;
					RoleManagerAgent::cache.erase(getData.roleId);
					Role Role = getRoleAsync(getData).get();
					RoleManagerAgent::cache.insert(make_pair(getData.roleId, Role));
					send(RoleManagerAgent::outBuffer, Role);
				}
				else {
					Role Role = getRoleAsync(getData).get();
					RoleManagerAgent::cache.insert(make_pair(getData.roleId, Role));
					send(RoleManagerAgent::outBuffer, Role);
				}
			}
			catch (exception error) {}
			Role Role;
			while (RoleManagerAgent::RolesToInsert.try_pop(Role)) {
				if (RoleManagerAgent::cache.contains(getData.roleId)) {
					RoleManagerAgent::cache.erase(getData.roleId);
				}
				RoleManagerAgent::cache.insert(make_pair(getData.roleId, Role));
			}
			done();
		}
	};

	class RoleManager {
	public:

		Guild* guild{ nullptr };

		RoleManager() {}

		task<Role> fetchAsync(GetRoleData getRoleData) {
			DiscordCoreInternal::GetRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(2);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent managerAgent(dataPackage.threadContext, dataPackage.agentResources, this, this->guild);
			send(RoleManagerAgent::requestFetchBuffer, dataPackage);
			managerAgent.start();
			Role newRole = receive(RoleManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return newRole;
		}

		task<Role> getRoleAsync(GetRoleData getRoleData) {
			DiscordCoreInternal::GetRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(2);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent managerAgent(dataPackage.threadContext, dataPackage.agentResources, this, this->guild);
			send(RoleManagerAgent::requestGetBuffer, dataPackage);
			managerAgent.start();
			Role newRole = receive(RoleManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return newRole;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		RoleManager(Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->threads = threadsNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outBuffer;
	concurrent_queue<Role> RoleManagerAgent::RolesToInsert;
	map<string, Role> RoleManagerAgent::cache;
}
#endif
