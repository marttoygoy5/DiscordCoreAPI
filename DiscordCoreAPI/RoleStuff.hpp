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

	class DiscorddiscordCoreClient;

	class Role {
	public:
		DiscordCoreInternal::RoleData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Role() {};

	protected:
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;

		Role(DiscordCoreInternal::RoleData roleData,  DiscordCoreClient* discordCoreClientNew) {
			this->data = roleData;
			this->discordCoreClient = discordCoreClientNew;
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
		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestGetRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchRoleData>* requestPatchRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PutRoleData>* requestPutRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostRoleData>* requestPostRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteRoleData>* requestDeleteRoleBuffer;
		static unbounded_buffer<Role>* outRoleBuffer;
		static unbounded_buffer<vector<Role>>* outRolesBuffer;
		static concurrent_queue<Role>* rolesToInsert;
		static overwrite_buffer<map<string, Role>>* cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize() {
			RoleManagerAgent::requestGetRoleBuffer = new unbounded_buffer<DiscordCoreInternal::GetRoleData>;
			RoleManagerAgent::requestPatchRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PatchRoleData>;
			RoleManagerAgent::requestPostRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PostRoleData>;
			RoleManagerAgent::requestDeleteRoleBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteRoleData>;
			RoleManagerAgent::requestFetchRolesBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRolesData>;
			RoleManagerAgent::requestFetchRoleBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRoleData>;
			RoleManagerAgent::requestPutRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PutRoleData>;
			RoleManagerAgent::rolesToInsert = new concurrent_queue<Role>;
			RoleManagerAgent::outRolesBuffer = new unbounded_buffer<vector<Role>>;
			RoleManagerAgent::outRoleBuffer = new unbounded_buffer<Role>;
			RoleManagerAgent::cache = new overwrite_buffer<map<string, Role>>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<vector<Role>>getObjectDataAsync(DiscordCoreInternal::FetchRolesData  dataPackage) {
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
				Role newRole(roleData, this->discordCoreClient);
				roleVector.push_back(newRole);
			}
			co_return roleVector;
		}

		task<Role> getObjectDataAsync(DiscordCoreInternal::FetchRoleData dataPackage) {
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
				Role newRole(roleData, this->discordCoreClient);
				cacheTemp.insert(make_pair(newRole.data.id, newRole));
			}
			asend(RoleManagerAgent::cache, cacheTemp);
			DiscordCoreInternal::RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId).data;
				cacheTemp.erase(dataPackage.roleId);
			}
			Role newRole(roleData, this->discordCoreClient);
			co_return newRole;
		}

		task<Role> patchObjectDataAsync(DiscordCoreInternal::PatchRoleData dataPackage) {
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
			Role newRole(roleData, this->discordCoreClient);
			co_return newRole;
		}

		task<Role> postObjectDataAsync(DiscordCoreInternal::PostRoleData dataPackage) {
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
			Role newRole(roleData, this->discordCoreClient);
			co_return newRole;
		}

		task<void> putObjectDataAsync(DiscordCoreInternal::PutRoleData dataPackage) {
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

		task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteRoleData dataPackage) {
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
				DiscordCoreInternal::GetRoleData dataPackage01;
				if (try_receive(RoleManagerAgent::requestGetRoleBuffer, dataPackage01)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.roleId)) {
							Role role = cacheTemp.at(dataPackage01.roleId);
							send(RoleManagerAgent::outRoleBuffer, role);
						}
					}
				}
				DiscordCoreInternal::FetchRoleData dataPackage02;
				if (try_receive(RoleManagerAgent::requestFetchRoleBuffer, dataPackage02)) {
					Role role = getObjectDataAsync(dataPackage02).get();
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.roleId)) {
							cacheTemp.erase(dataPackage02.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage02.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PatchRoleData dataPackage03;
				if (try_receive(RoleManagerAgent::requestPatchRoleBuffer, dataPackage03)) {
					Role role = patchObjectDataAsync(dataPackage03).get();
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.roleId)) {
							cacheTemp.erase(dataPackage03.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage03.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PutRoleData dataPackage04;
				if (try_receive(RoleManagerAgent::requestPutRoleBuffer, dataPackage04)) {
					putObjectDataAsync(dataPackage04).get();
				}
				DiscordCoreInternal::DeleteRoleData dataPackage05;
				if (try_receive(RoleManagerAgent::requestDeleteRoleBuffer, dataPackage05)) {
					deleteObjectDataAsync(dataPackage05).get();
				}
				DiscordCoreInternal::FetchRolesData dataPackage06;
				if (try_receive(RoleManagerAgent::requestFetchRolesBuffer, dataPackage06)) {
					vector<Role> roles = getObjectDataAsync(dataPackage06).get();
					send(RoleManagerAgent::outRolesBuffer, roles);
				}
				DiscordCoreInternal::PostRoleData dataPacakge07;
				if (try_receive(RoleManagerAgent::requestPostRoleBuffer, dataPacakge07)) {
					Role role = postObjectDataAsync(dataPacakge07).get();
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.data.id)) {
							cacheTemp.erase(role.data.id);
						}
					}
					cacheTemp.insert(make_pair(dataPacakge07.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::RoleData dataPackage08;
				Role roleNew(dataPackage08, this->discordCoreClient);
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

		task<Role> fetchAsync(FetchRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<Role> getRoleAsync(GetRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<Role> updateRoleAsync(UpdateRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PatchRoleData dataPackageNew;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = dataPackage.permissions;
			dataPackageNew.content = DiscordCoreInternal::getModifyRolePayload(dataPackageNew);
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(5);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestPatchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::updateRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<vector<Role>> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			vector<Role> rolesVectorNew;
			for (auto value : rolesVector) {
				for (auto value2: dataPackage.guildMember.data.roles) {
					if (value2 == value.data.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			co_await mainThread;
			co_return rolesVector;
		}

		task<vector<Role>> getGuildRolesAsync(GetRolesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRolesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(RoleManagerAgent::requestFetchRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getGuildRolesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Role> newRoles;
			try_receive(RoleManagerAgent::outRolesBuffer, newRoles);
			co_await mainThread;
			co_return newRoles;
		}

		task<Role>createRoleAsync(CreateRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PostRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(5);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = stoll(dataPackage.permissions);
			dataPackageNew.content = DiscordCoreInternal::getCreateRolePayload(dataPackageNew);
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestPostRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(AddRoleToGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PutRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(5);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(4).scheduler);
			send(RoleManagerAgent::requestPutRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
		}

		task<void> removeRoleFromGuildMemberAsync(RemoveRoleFromGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(7);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(6).scheduler);
			send(RoleManagerAgent::requestDeleteRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
		}

		task<void> insertRoleAsync(Role role) {
			RoleManagerAgent roleManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
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
		DiscordCoreClient* discordCoreClient{ nullptr };

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::FetchRolesData>* RoleManagerAgent::requestFetchRolesBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchRoleData>* RoleManagerAgent::requestPatchRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PutRoleData>* RoleManagerAgent::requestPutRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PostRoleData>* RoleManagerAgent::requestPostRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteRoleData>* RoleManagerAgent::requestDeleteRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchRoleData>* RoleManagerAgent::requestFetchRoleBuffer;
	unbounded_buffer<vector<Role>>* RoleManagerAgent::outRolesBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outRoleBuffer;
	concurrent_queue<Role>* RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>>* RoleManagerAgent::cache;
}
#endif