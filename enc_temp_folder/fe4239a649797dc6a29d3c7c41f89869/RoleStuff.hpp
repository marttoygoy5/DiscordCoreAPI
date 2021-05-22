// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
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

	struct ModifyRoleData {
		string guildId;
		string roleId;
		string name;
		string permissions;
		int colorFirst[3];
		int color;
		bool hoist;
		bool mentionable;
		int actualColor() {
			if (this->colorFirst[0] > 255) {
				this->colorFirst[0] = 255;
			}
			else if (this->colorFirst[0] < 0) {
				this->colorFirst[0] = 0;
			}
			if (this->colorFirst[1] > 255) {
				this->colorFirst[1] = 255;
			}
			else if (this->colorFirst[1] < 0) {
				this->colorFirst[1] = 0;
			}
			if (this->colorFirst[2] > 255) {
				this->colorFirst[2] = 255;
			}
			else if (this->colorFirst[2] < 0) {
				this->colorFirst[2] = 0;
			}
			int colorValue = 65536 * this->colorFirst[0] + 256 * this->colorFirst[1] + this->colorFirst[2];
			return colorValue;
		};
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
		static unbounded_buffer<DiscordCoreInternal::ModifyRoleDataInternal>* requestModifyBuffer;
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
			RoleManagerAgent::requestModifyBuffer = new unbounded_buffer<DiscordCoreInternal::ModifyRoleDataInternal>;
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
			agent::wait(&requestAgent);
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			map<string, Role> cacheTemp = receive(RoleManagerAgent::cache, 1U);
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				DiscordCoreInternal::RoleData roleData;
				if (cacheTemp.contains(jsonValue.at(x).at("id"))) {
					roleData = cacheTemp.at(jsonValue.at(x).at("id")).data;
					cout << roleData.id << endl;
					cacheTemp.erase(jsonValue.at(x).at("id"));
				}
				DiscordCoreInternal::parseObject(jsonValue.at(x), &roleData);
				Role newRole(this->agentResources, roleData, this->roles);
				cacheTemp.insert(make_pair(newRole.data.id, newRole));
			}
			asend(RoleManagerAgent::cache, cacheTemp);
			DiscordCoreInternal::RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId).data;
				cacheTemp.erase(dataPackage.roleId);
			}
			Role newRole(this->agentResources, roleData, this->roles);
			co_return newRole;
			
		}

		task<Role> patchRoleAsync(DiscordCoreInternal::ModifyRoleDataInternal dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			DiscordCoreInternal::RoleData roleData;
			DiscordCoreInternal::parseObject(jsonValue, &roleData);
			Role newRole(this->agentResources, roleData, this->roles);
			send(RoleManagerAgent::outBuffer, newRole);
			co_return newRole;
		}

		void run() {
			DiscordCoreInternal::GetRoleData getData;
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
			DiscordCoreInternal::ModifyRoleDataInternal modifyData;
			if (try_receive(RoleManagerAgent::requestModifyBuffer, modifyData)) {
				Role role = patchRoleAsync(modifyData).get();
				map<string, Role> cacheTemp;
				if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(modifyData.roleId)) {
						cacheTemp.erase(modifyData.roleId);
					}
				}
				cacheTemp.insert(make_pair(modifyData.roleId, role));
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

		task<Role> updateRoleAsync(ModifyRoleData modifyRoleData) {
			DiscordCoreInternal::ModifyRoleData modifyRoleDataNew;
			modifyRoleDataNew.color = modifyRoleData.color;
			modifyRoleDataNew.hoist = modifyRoleData.hoist;
			modifyRoleDataNew.mentionable = modifyRoleData.mentionable;
			modifyRoleDataNew.name = modifyRoleData.name;
			modifyRoleDataNew.permissions = modifyRoleData.permissions;
			DiscordCoreInternal::ModifyRoleDataInternal dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(5);
			dataPackage.content = DiscordCoreInternal::getModifyRolePayload(modifyRoleDataNew);
			dataPackage.guildId = modifyRoleData.guildId;
			dataPackage.roleId = modifyRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestModifyBuffer, dataPackage);
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
	unbounded_buffer<DiscordCoreInternal::ModifyRoleDataInternal>* RoleManagerAgent::requestModifyBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outBuffer;
	concurrent_queue<Role> RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>> RoleManagerAgent::cache;
}
#endif