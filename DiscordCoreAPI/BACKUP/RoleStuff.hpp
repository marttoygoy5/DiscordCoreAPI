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

	class Role {
	public:
		DiscordCoreInternal::RoleData data;
		RoleManager* roles{ nullptr };

		Role() {};

	protected:
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;
		DiscordCoreInternal::HttpAgentResources agentResources;

		Role(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::RoleData roleData, RoleManager* rolesNew) {
			this->data = roleData;
			this->agentResources = agentResourcesNew;
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
		static concurrent_queue<Role> rolesToInsert;
		static overwrite_buffer<map<string, Role>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		RoleManager* roles;

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreAPI::RoleManager* rolesNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->roles = rolesNew;
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
			map<string, Role> cacheTemp = receive(RoleManagerAgent::cache, 1U);
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				DiscordCoreInternal::RoleData roleData;
				if (cacheTemp.contains(dataPackage.roleId)) {
					roleData = cacheTemp.at(dataPackage.roleId).data;
					cacheTemp.erase(dataPackage.roleId);
					DiscordCoreInternal::parseObject(jsonValue, &roleData);
					Role newRole(this->agentResources, roleData, this->roles);
					cacheTemp.insert(make_pair(newRole.data.id, newRole));
					co_return newRole;
				}
				else {
					DiscordCoreInternal::parseObject(jsonValue, &roleData);
					Role newRole(this->agentResources, roleData, this->roles);
					cacheTemp.insert(make_pair(newRole.data.id, newRole));
				}
			}
			asend(RoleManagerAgent::cache, cacheTemp);
		}

		void run() {
			DiscordCoreInternal::GetRoleData getData;
			try {
				if (try_receive(RoleManagerAgent::requestGetBuffer, getData)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(getData.guildId)) {
							Role role = cacheTemp.at(getData.guildId);
							send(RoleManagerAgent::outBuffer, role);
						}
						else {
							Role role = getRoleAsync(getData).get();
							cacheTemp.insert(make_pair(getData.roleId, role));
							send(RoleManagerAgent::outBuffer, role);
							asend(cache, cacheTemp);
						}
					}
					else {
						Role role = getRoleAsync(getData).get();
						cacheTemp.insert(make_pair(getData.roleId, role));
						send(RoleManagerAgent::outBuffer, role);
						asend(cache, cacheTemp);
					}
				}
			}
			catch (exception error) {}
			if (try_receive(RoleManagerAgent::requestFetchBuffer, getData)) {
				map<string, Role> cacheTemp;
				if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.roleId)) {
						cacheTemp.erase(getData.guildId);
					}
				}
				Role role = getRoleAsync(getData).get();
				cacheTemp.insert(make_pair(getData.roleId, role));
				send(RoleManagerAgent::outBuffer, role);
				asend(cache, cacheTemp);
			}
			DiscordCoreInternal::RoleData roleData;
			Role roleNew(this->agentResources, roleData, this->roles);
			while (RoleManagerAgent::rolesToInsert.try_pop(roleNew)) {
				map<string, Role> cacheTemp;
				if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(roleNew.data.id)) {
						cacheTemp.erase(roleNew.data.id);
					}
				}
				cacheTemp.insert(make_pair(roleNew.data.id, roleNew));
				asend(RoleManagerAgent::cache, cacheTemp);
			}
			done();
		}
	};

	class RoleManager {
	public:

		task<Role> fetchAsync(GetRoleData getRoleData) {
			DiscordCoreInternal::GetRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			DiscordCoreInternal::RoleData roleData;
			Role newRole(this->agentResources, roleData, this);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_return newRole;
		}

		task<Role> getRoleAsync(GetRoleData getRoleData) {
			DiscordCoreInternal::GetRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestGetBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			DiscordCoreInternal::RoleData roleData;
			Role newRole(this->agentResources, roleData, this);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_return newRole;
		}

		task<void> insertRole(Role role) {
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this, this->threads->at(2).scheduler);
			roleManagerAgent.start();
			RoleManagerAgent::rolesToInsert.push(role);
			roleManagerAgent.wait(&roleManagerAgent);
			co_return;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };;
		DiscordCoreInternal::HttpAgentResources agentResources;

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outBuffer;
	concurrent_queue<Role> RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>> RoleManagerAgent::cache;
}
#endif
