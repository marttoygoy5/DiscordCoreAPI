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

	class RoleManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Role>* outBuffer;
		static concurrent_queue<Role> RolesToInsert;
		static map<string, Role> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::RoleManager* pRoleManager;
		Guild* pGuild;

		RoleManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::RoleManager* pRoleManagerNew, Guild* pGuildNew)
			:agent(*threadsNew->at(8).scheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pRoleManager = pRoleManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			RoleManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			RoleManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			RoleManagerAgent::outBuffer = new unbounded_buffer<Role>;
			co_return;
		}

		task<Role> getRoleAsync(string roleId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + this->pGuild->data.id + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(3).scheduler);
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
			Role roleNew = RoleManagerAgent::cache.at(roleId);
			co_return roleNew;
		}

		void run() {
			string getData;
			try {
				getData = receive(RoleManagerAgent::requestGetBuffer, 1U);
				if (RoleManagerAgent::cache.contains(getData)) {
					Role Role = RoleManagerAgent::cache.at(getData);
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
				if (RoleManagerAgent::cache.contains(getData)) {
					DiscordCoreInternal::RoleData oldRoleData = RoleManagerAgent::cache.at(getData).data;
					RoleManagerAgent::cache.erase(getData);
					Role Role = getRoleAsync(getData).get();
					RoleManagerAgent::cache.insert(make_pair(getData, Role));
					send(RoleManagerAgent::outBuffer, Role);
				}
				else {
					Role Role = getRoleAsync(getData).get();
					RoleManagerAgent::cache.insert(make_pair(getData, Role));
					send(RoleManagerAgent::outBuffer, Role);
				}
			}
			catch (exception error) {}
			Role Role;
			while (RoleManagerAgent::RolesToInsert.try_pop(Role)) {
				if (RoleManagerAgent::cache.contains(getData)) {
					RoleManagerAgent::cache.erase(getData);
				}
				RoleManagerAgent::cache.insert(make_pair(getData, Role));
			}
			done();
		}
	};

	class RoleManager {
	public:

		Guild* guild{ nullptr };

		RoleManager() {}
		

		task<Role> fetchAsync(string roleId) {
			RoleManagerAgent managerAgent(this->pSystemThreads->threads, this->agentResources, this, this->guild);
			DiscordCoreInternal::RoleData roleData;
			send(RoleManagerAgent::requestFetchBuffer, roleId);
			managerAgent.start();
			Role newRole = receive(RoleManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return newRole;
		}

		task<Role> getGuildMemberAsync(string roleId) {
			RoleManagerAgent managerAgent(this->pSystemThreads->threads, this->agentResources, this, this->guild);
			DiscordCoreInternal::RoleData roleData;
			send(RoleManagerAgent::requestFetchBuffer, roleId);
			managerAgent.start();
			Role newRole = receive(RoleManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return newRole;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		DiscordCoreInternal::SystemThreads* pSystemThreads;
		DiscordCoreInternal::HttpAgentResources agentResources;

		RoleManager(Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::SystemThreads* pSystemThreadsNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->pSystemThreads = pSystemThreadsNew;
		}
	};
}
#endif
