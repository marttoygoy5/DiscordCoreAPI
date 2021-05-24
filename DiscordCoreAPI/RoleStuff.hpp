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
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

	class RoleManager;

	class DiscordCoreClient;

	class Role {
	public:
		DiscordCoreInternal::RoleData data;
		DiscordCoreClient* coreClient{ nullptr };

		Role() {};

	protected:
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;

		Role(DiscordCoreInternal::RoleData roleData,  DiscordCoreClient* coreClientNew) {
			this->data = roleData;
			this->coreClient = coreClientNew;
		}
	};

	struct UpdateRoleData {
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

	struct FetchRoleData {
		string guildId;
		string roleId;
	};

	struct GetRoleData {
		string guildId;
		string roleId;
	};

	struct GetGuildMemberRolesData {
		string guildId;
		GuildMember guildMember;
	};

	class RoleManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static unbounded_buffer<DiscordCoreInternal::FetchRoleData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::UpdateRoleDataInternal>* requestModifyBuffer;
		static unbounded_buffer<Role>* outBuffer;
		static concurrent_queue<Role> rolesToInsert;
		static overwrite_buffer<map<string, Role>> cache;
		single_assignment<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			RoleManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetRoleData>;
			RoleManagerAgent::requestModifyBuffer = new unbounded_buffer<DiscordCoreInternal::UpdateRoleDataInternal>;
			RoleManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRoleData>;
			RoleManagerAgent::outBuffer = new unbounded_buffer<Role>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Role> getObjectAsync(DiscordCoreInternal::FetchRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			if (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			map<string, Role> cacheTemp = receive(RoleManagerAgent::cache, 1U);
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				DiscordCoreInternal::RoleData roleData;
				if (cacheTemp.contains(returnData.data.at(x).at("id"))) {
					roleData = cacheTemp.at(returnData.data.at(x).at("id")).data;
					cacheTemp.erase(returnData.data.at(x).at("id"));
				}
				DiscordCoreInternal::parseObject(returnData.data.at(x), &roleData);
				Role newRole(roleData, this->coreClient);
				cacheTemp.insert(make_pair(newRole.data.id, newRole));
			}
			asend(RoleManagerAgent::cache, cacheTemp);
			DiscordCoreInternal::RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId).data;
				cacheTemp.erase(dataPackage.roleId);
			}
			Role newRole(roleData, this->coreClient);
			co_return newRole;
		}

		task<Role> patchObjectAsync(DiscordCoreInternal::UpdateRoleDataInternal dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			if (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::paatchObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->coreClient);
			send(RoleManagerAgent::outBuffer, newRole);
			co_return newRole;
		}

		void run() {
			try {
				DiscordCoreInternal::GetRoleData getData;
				if (try_receive(RoleManagerAgent::requestGetBuffer, getData)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(getData.roleId)) {
							Role role = cacheTemp.at(getData.roleId);
							send(RoleManagerAgent::outBuffer, role);
						}
					}
				}
				DiscordCoreInternal::FetchRoleData fetchData;
				if (try_receive(RoleManagerAgent::requestFetchBuffer, fetchData)) {
					Role role = getObjectAsync(fetchData).get();
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(fetchData.roleId)) {
							cacheTemp.erase(fetchData.roleId);
						}
					}
					cacheTemp.insert(make_pair(fetchData.roleId, role));
					send(RoleManagerAgent::outBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::UpdateRoleDataInternal modifyData;
				if (try_receive(RoleManagerAgent::requestModifyBuffer, modifyData)) {
					Role role = patchObjectAsync(modifyData).get();
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
				Role roleNew(roleData, this->coreClient);
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
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class RoleManager {
	public:

		task<Role> fetchAsync(FetchRoleData getRoleData) {
			apartment_context mainThread;
			co_await resume_background();
			if (getRoleData.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient,  this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			if (roleManagerAgent.getError(error)) {
				cout << "RoleManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->coreClient);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<Role> getRoleAsync(GetRoleData getRoleData) {
			apartment_context mainThread;
			co_await resume_background();
			if (getRoleData.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getRoleData.guildId;
			dataPackage.roleId = getRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestGetBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			if (roleManagerAgent.getError(error)) {
				cout << "RoleManager::getRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->coreClient);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<Role> updateRoleAsync(UpdateRoleData modifyRoleData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::UpdateRoleData modifyRoleDataNew;
			modifyRoleDataNew.colorFirst[0] = modifyRoleData.colorFirst[0];
			modifyRoleDataNew.colorFirst[1] = modifyRoleData.colorFirst[1];
			modifyRoleDataNew.colorFirst[2] = modifyRoleData.colorFirst[2];
			modifyRoleDataNew.color = modifyRoleData.color;
			modifyRoleDataNew.hoist = modifyRoleData.hoist;
			modifyRoleDataNew.mentionable = modifyRoleData.mentionable;
			modifyRoleDataNew.name = modifyRoleData.name;
			modifyRoleDataNew.permissions = modifyRoleData.permissions;
			DiscordCoreInternal::UpdateRoleDataInternal dataPackage;
			dataPackage.content = DiscordCoreInternal::getModifyRolePayload(modifyRoleDataNew);
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(5);
			dataPackage.guildId = modifyRoleData.guildId;
			dataPackage.roleId = modifyRoleData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestModifyBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			if (roleManagerAgent.getError(error)) {
				cout << "RoleManager::updateRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->coreClient);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<vector<Role>> getGuildMemberRoles(GetGuildMemberRolesData getGuildMemberRolesData) {
			apartment_context mainThread;
			co_await resume_background();
			vector<Role> rolesVector;
			for (unsigned int x = 0; x < getGuildMemberRolesData.guildMember.data.roles.size(); x += 1) {
				Role newRole = getRoleAsync({ .guildId = getGuildMemberRolesData.guildId, .roleId = getGuildMemberRolesData.guildMember.data.roles.at(x) }).get();
				rolesVector.push_back(newRole);
			}
			co_await mainThread;
			co_return rolesVector;
		}

		task<void> insertRoleAsync(Role role) {
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			RoleManagerAgent::rolesToInsert.push(role);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			if (roleManagerAgent.getError(error)) {
				cout << "RoleManager::inserRoleAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };;
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* coreClient{ nullptr };

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::FetchRoleData>* RoleManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::UpdateRoleDataInternal>* RoleManagerAgent::requestModifyBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outBuffer;
	concurrent_queue<Role> RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>> RoleManagerAgent::cache;
}
#endif