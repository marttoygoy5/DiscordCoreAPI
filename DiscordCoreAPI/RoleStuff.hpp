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
		string hexColorValue;
		bool hoist;
		bool mentionable;
	};

	struct GetRolesData{
		string guildId;
	};

	struct FetchRoleData {
		string guildId;
		string roleId;
	};

	struct RemoveRoleFromGuildMemberData {
		string guildId;
		string userId;
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

	struct AddRoleToGuildMemberData {
		string guildId;
		string userId;
		string roleId;
	};

	struct CreateRoleData {
		string name;
		string permissions;
		string hexColorValue;
		bool hoist;
		bool mentionable;
		string guildId;
	};

	class RoleManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static unbounded_buffer<DiscordCoreInternal::FetchRolesData>* requestFetchRolesBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchRoleData>* requestFetchRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchRoleData>* requestModifyBuffer;
		static unbounded_buffer<DiscordCoreInternal::PutRoleData>* requestAddRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostRoleData>* requestPostBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteRoleData>* requestDeleteRoleBuffer;
		static unbounded_buffer<Role>* outBuffer;
		static unbounded_buffer<vector<Role>>* outRolesBuffer;
		static concurrent_queue<Role>* rolesToInsert;
		static overwrite_buffer<map<string, Role>>* cache;
		unbounded_buffer<exception> errorBuffer;

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
			RoleManagerAgent::requestModifyBuffer = new unbounded_buffer<DiscordCoreInternal::PatchRoleData>;
			RoleManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::PostRoleData>;
			RoleManagerAgent::requestDeleteRoleBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteRoleData>;
			RoleManagerAgent::requestFetchRolesBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRolesData>;
			RoleManagerAgent::requestFetchRoleBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRoleData>;
			RoleManagerAgent::requestAddRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PutRoleData>;
			RoleManagerAgent::rolesToInsert = new concurrent_queue<Role>;
			RoleManagerAgent::outRolesBuffer = new unbounded_buffer<vector<Role>>;
			RoleManagerAgent::outBuffer = new unbounded_buffer<Role>;
			RoleManagerAgent::cache = new overwrite_buffer<map<string, Role>>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<vector<Role>>getObjectAsync(DiscordCoreInternal::FetchRolesData  dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
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
			vector<Role> roleVector;
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				RoleData roleData;
				DiscordCoreInternal::parseObject(returnData.data.at(x), &roleData);
				Role newRole(roleData, this->coreClient);
				roleVector.push_back(newRole);
			}
			co_return roleVector;
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
			while (requestAgent.getError(error)) {
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
				RoleData roleData;
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

		task<Role> patchObjectAsync(DiscordCoreInternal::PatchRoleData dataPackage) {
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
			while (requestAgent.getError(error)) {
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
			RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->coreClient);
			co_return newRole;
		}

		task<Role> postObjectAsync(DiscordCoreInternal::PostRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::postObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::postObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::postObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->coreClient);
			co_return newRole;
		}

		task<void> putObjectAsync(DiscordCoreInternal::PutRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::putObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::putObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::putObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		task<void> deleteObjectAsync(DiscordCoreInternal::DeleteRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::deleteObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
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
				if (try_receive(RoleManagerAgent::requestFetchRoleBuffer, fetchData)) {
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
				DiscordCoreInternal::PatchRoleData modifyData;
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
				DiscordCoreInternal::PutRoleData addRoleData;
				if (try_receive(RoleManagerAgent::requestAddRoleBuffer, addRoleData)) {
					putObjectAsync(addRoleData).get();
				}
				DiscordCoreInternal::DeleteRoleData removeRoleData;
				if (try_receive(RoleManagerAgent::requestDeleteRoleBuffer, removeRoleData)) {
					deleteObjectAsync(removeRoleData).get();
				}
				DiscordCoreInternal::FetchRolesData fetchRolesData;
				if (try_receive(RoleManagerAgent::requestFetchRolesBuffer, fetchRolesData)) {
					vector<Role> roles = getObjectAsync(fetchRolesData).get();
					send(RoleManagerAgent::outRolesBuffer, roles);
				}
				DiscordCoreInternal::PostRoleData postData;
				if (try_receive(RoleManagerAgent::requestPostBuffer, postData)) {
					Role role = postObjectAsync(postData).get();
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.data.id)) {
							cacheTemp.erase(role.data.id);
						}
					}
					cacheTemp.insert(make_pair(modifyData.roleId, role));
					send(RoleManagerAgent::outBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::RoleData roleData;
				Role roleNew(roleData, this->coreClient);
				while (RoleManagerAgent::rolesToInsert->try_pop(roleNew)) {
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
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchRoleBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
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
			while (roleManagerAgent.getError(error)) {
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
			modifyRoleDataNew.hexColorValue = modifyRoleData.hexColorValue;
			modifyRoleDataNew.hoist = modifyRoleData.hoist;
			modifyRoleDataNew.mentionable = modifyRoleData.mentionable;
			modifyRoleDataNew.name = modifyRoleData.name;
			modifyRoleDataNew.permissions = modifyRoleData.permissions;
			DiscordCoreInternal::PatchRoleData dataPackage;
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
			while (roleManagerAgent.getError(error)) {
				cout << "RoleManager::updateRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->coreClient);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<vector<Role>> getGuildMemberRolesAsync(GetGuildMemberRolesData getGuildMemberRolesData) {
			apartment_context mainThread;
			co_await resume_background();
			vector<Role> rolesVectorNew = getGuildRolesAsync({ .guildId = getGuildMemberRolesData.guildId }).get();
			vector<Role> rolesVector;
			for (auto value : rolesVectorNew) {
				for (auto value2:getGuildMemberRolesData.guildMember.data.roles) {
					if (value2 == value.data.id) {
						rolesVector.push_back(value);
					}
				}
			}
			co_await mainThread;
			co_return rolesVector;
		}

		task<vector<Role>> getGuildRolesAsync(GetRolesData getGuildRolesData) {
			apartment_context mainThread;
			co_await resume_background();
			if (getGuildRolesData.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRolesData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildRolesData.guildId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchRolesBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
				cout << "RoleManager::getGuildRolesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Role> newRoles;
			try_receive(RoleManagerAgent::outRolesBuffer, newRoles);
			co_await mainThread;
			co_return newRoles;
		}

		task<Role>createRoleAsync(CreateRoleData createRoleData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(5);
			dataPackage.guildId = createRoleData.guildId;
			DiscordCoreInternal::CreateRoleData createRoleDataNew;
			createRoleDataNew.hexColorValue = createRoleData.hexColorValue;
			createRoleDataNew.hoist = createRoleData.hoist;
			createRoleDataNew.mentionable = createRoleData.mentionable;
			createRoleDataNew.name = createRoleData.name;
			createRoleDataNew.permissions = stoll(createRoleData.permissions);
			dataPackage.content = DiscordCoreInternal::getCreateRolePayload(createRoleDataNew);
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestPostBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->coreClient);
			try_receive(RoleManagerAgent::outBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(AddRoleToGuildMemberData addRoleToGuildMemberData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PutRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(5);
			dataPackage.guildId = addRoleToGuildMemberData.guildId;
			dataPackage.userId = addRoleToGuildMemberData.userId;
			dataPackage.roleId = addRoleToGuildMemberData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestAddRoleBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
		}

		task<void> removeRoleFromGuildMemberAsync(RemoveRoleFromGuildMemberData removeRoleFromGuildMemberData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteRoleData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(7);
			dataPackage.guildId = removeRoleFromGuildMemberData.guildId;
			dataPackage.userId = removeRoleFromGuildMemberData.userId;
			dataPackage.roleId = removeRoleFromGuildMemberData.roleId;
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(6).scheduler);
			send(RoleManagerAgent::requestDeleteRoleBuffer, dataPackage);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
		}

		task<void> insertRoleAsync(Role role) {
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			RoleManagerAgent::rolesToInsert->push(role);
			roleManagerAgent.start();
			agent::wait(&roleManagerAgent);
			exception error;
			while (roleManagerAgent.getError(error)) {
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
	unbounded_buffer<DiscordCoreInternal::FetchRolesData>* RoleManagerAgent::requestFetchRolesBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchRoleData>* RoleManagerAgent::requestModifyBuffer;
	unbounded_buffer<DiscordCoreInternal::PutRoleData>* RoleManagerAgent::requestAddRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PostRoleData>* RoleManagerAgent::requestPostBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteRoleData>* RoleManagerAgent::requestDeleteRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchRoleData>* RoleManagerAgent::requestFetchRoleBuffer;
	unbounded_buffer<vector<Role>>* RoleManagerAgent::outRolesBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outBuffer;
	concurrent_queue<Role>* RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>>* RoleManagerAgent::cache;
}
#endif