// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "HttpStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"

namespace DiscordCoreAPI {

	class Guild {
	public:
		GuildData data;
		DiscordCoreClientBase* discordCoreClientBase{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		Guild() {};

	protected:
		friend class GuildManagerAgent;
		friend class DiscordCoreClient;
		friend class GuildManager;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };

		Guild(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, GuildData dataNew,  DiscordCoreClient* discordCoreClientNew, DiscordCoreClientBase* discordCoreClientBaseNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient= discordCoreClientNew;
			this->discordCoreClientBase = discordCoreClientBaseNew;
			this->data = dataNew;
		}

		void initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (auto value:data.channels) {
					value.guildId = this->data.id;
					ChannelData channelData = value;
					Channel channel(channelData, this->discordCoreClient);
					this->discordCoreClientBase->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (auto value:data.members) {
					GuildMemberData guildMemberData = value;
					guildMemberData.guildId = this->data.id;
					DiscordGuildMember discordGuildMember(guildMemberData);
					discordGuildMember.writeDataToDB();
					DiscordCoreClientBase::guildMemberMap.insert(make_pair(guildMemberData.guildId + guildMemberData.user.id, discordGuildMember));
					GuildMember guildMember(guildMemberData, this->discordCoreClient);
					this->discordCoreClientBase->guildMembers->insertGuildMemberAsync(guildMember).get();
				}
				cout << "Caching roles for guild: " << this->data.name << endl;
				for (auto const& value : data.roles) {
					RoleData roleData = value;
					Role role(roleData, this->discordCoreClient);
					this->discordCoreClientBase->roles->insertRoleAsync(role).get();
				}
				cout << "Caching users for guild: " << this->data.name << endl << endl;
				for (auto value : data.members) {
					DiscordCoreInternal::UserData userData = value.user;
					User user(userData, this->discordCoreClient);
					this->discordCoreClientBase->users->insertUserAsync(user).get();
				}
			}
			catch (exception error) {
				cout << "Error: " << error.what() << endl;
			}
			return;
		}
	};
	
	struct GetGuildData {
		string guildId;
	};

	struct FetchGuildData {
		string guildId;
	};

	struct GetAuditLogData {
		AuditLogEvent actionType;
		string guildId;
		unsigned int limit;
		string userId;
	};

	class GuildManagerAgent : public agent {
	protected:
		friend class DiscordCoreClientExt;
		friend class DiscordCoreClient;
		friend class GuildManager;
		static unbounded_buffer<DiscordCoreInternal::GetAuditLogData>* requestGetAuditLogBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchGuildData>* requestFetchGuildBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestGetGuildBuffer;
		static unbounded_buffer<AuditLogData>* outAuditLogBuffer;
		static unbounded_buffer<Guild>* outGuildBuffer;
		static concurrent_queue<Guild>* guildsToInsert;
		static overwrite_buffer<map<string, Guild>> cache;
		static unbounded_buffer<exception>* errorBuffer;
		
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClientBase* discordCoreClientBase{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		GuildManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew,  DiscordCoreClient* coreClientNew, DiscordCoreClientBase* coreClientBaseNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
		}

		static void initialize() {
			GuildManagerAgent::requestFetchGuildBuffer = new unbounded_buffer<DiscordCoreInternal::FetchGuildData>;
			GuildManagerAgent::requestGetGuildBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildData>;
			GuildManagerAgent::requestGetAuditLogBuffer = new unbounded_buffer<DiscordCoreInternal::GetAuditLogData>;
			GuildManagerAgent::outAuditLogBuffer = new unbounded_buffer<AuditLogData>;
			GuildManagerAgent::outGuildBuffer = new unbounded_buffer<Guild>;
			GuildManagerAgent::guildsToInsert = new concurrent_queue<Guild>;
			GuildManagerAgent::errorBuffer = new unbounded_buffer<exception>;
			return;
		}

		bool getError(exception error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Guild> getObjectAsync(DiscordCoreInternal::FetchGuildData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildData guildData;
			DiscordCoreInternal::parseObject(returnData.data, &guildData);
			Guild guildNew(this->agentResources, this->threads, guildData, this->discordCoreClient, this->discordCoreClientBase);
			co_return guildNew;
		}

		task<AuditLogData> getObjectAsync(DiscordCoreInternal::GetAuditLogData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_AUDIT_LOG;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
			if (dataPackage.userId != "") {
				workload.relativePath += "?user_id=" + dataPackage.userId;
				if (to_string((int)dataPackage.actionType)!= "") {
					workload.relativePath += "&action_type=" + to_string((int)dataPackage.actionType);
				}
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (to_string((int)dataPackage.actionType) != "") {
				workload.relativePath += "?action_type=" + to_string((int)dataPackage.actionType);
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + to_string(dataPackage.limit);
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManagerAgent::getObjectAsync() Error 02: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectAsync() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectAsync() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			AuditLogData auditLogData;
			DiscordCoreInternal::parseObject(returnData.data, &auditLogData);
			co_return auditLogData;
		}

		void run() {
			try {
				DiscordCoreInternal::GetGuildData dataPackage01;
				if (try_receive(GuildManagerAgent::requestGetGuildBuffer, dataPackage01)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId)) {
							Guild guild = cacheTemp.at(dataPackage01.guildId);
							send(GuildManagerAgent::outGuildBuffer, guild);
						}
					}
				}
				DiscordCoreInternal::FetchGuildData dataPackage02;
				if (try_receive(GuildManagerAgent::requestFetchGuildBuffer, dataPackage02)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId)) {
							cacheTemp.erase(dataPackage02.guildId);
						}
					}
					Guild guild = getObjectAsync(dataPackage02).get();
					cacheTemp.insert(make_pair(dataPackage02.guildId, guild));
					send(GuildManagerAgent::outGuildBuffer, guild);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::GetAuditLogData dataPackage03;
				if (try_receive(GuildManagerAgent::requestGetAuditLogBuffer, dataPackage03)) {
					AuditLogData auditLog = getObjectAsync(dataPackage03).get();
					send(GuildManagerAgent::outAuditLogBuffer, auditLog);
				}
				GuildData dataPackage04;
				Guild guildNew(this->agentResources, this->threads, dataPackage04, this->discordCoreClient, this->discordCoreClientBase);
				while (GuildManagerAgent::guildsToInsert->try_pop(guildNew)) {
					map<string, Guild> cacheTemp;
					try_receive(GuildManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
					guildNew.initialize();
					cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
					asend(cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class GuildManager {
	public:

		task<Guild> fetchAsync(FetchGuildData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::FetchGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->discordCoreClientBase, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestFetchGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			GuildData guildData;
			Guild guild(this->agentResources, this->threads, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(GuildManagerAgent::outGuildBuffer, guild);
			co_await mainThread;
			co_return guild;
		}

		task<AuditLogData> getAuditLogDataAsync(GetAuditLogData dataPackage) {
			try {
				apartment_context mainThread;
				co_await resume_background();
				DiscordCoreInternal::GetAuditLogData dataPackageNew;
				dataPackageNew.agentResources = this->agentResources;
				dataPackageNew.threadContext = this->threads->at(3);
				dataPackageNew.guildId = dataPackage.guildId;
				dataPackageNew.actionType = (DiscordCoreInternal::AuditLogEvent)dataPackage.actionType;
				dataPackageNew.limit = dataPackage.limit;
				dataPackageNew.userId = dataPackage.userId;
				GuildManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->discordCoreClientBase, this->threads->at(2).scheduler);
				send(GuildManagerAgent::requestGetAuditLogBuffer, dataPackageNew);
				requestAgent.start();
				agent::wait(&requestAgent);
				exception error;
				while (requestAgent.getError(error)) {
					cout << "GuildManager::getAuditLogDataAsync() Error: " << error.what() << endl << endl;
				}
				AuditLogData auditLog;
				try_receive(GuildManagerAgent::outAuditLogBuffer, auditLog);
				co_await mainThread;
				co_return auditLog;
			}
			catch (exception& e) {
				cout << "GuildManager::getAuditLogDataAsync() Error: " << e.what() << endl << endl;
			}			
		}

		task<Guild> getGuildAsync(GetGuildData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->discordCoreClientBase, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestGetGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getGuildAsync() Error: " << error.what() << endl << endl;
			}
			GuildData guildData;
			Guild guild(this->agentResources, this->threads, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(GuildManagerAgent::outGuildBuffer, guild);
			co_await mainThread;
			co_return guild;
		}

		task<void> insertGuildAsync(Guild guild) {
			GuildManagerAgent guildManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->discordCoreClientBase, this->threads->at(2).scheduler);
			GuildManagerAgent::guildsToInsert->push(guild);
			guildManagerAgent.start();
			agent::wait(&guildManagerAgent);
			exception error;
			while (guildManagerAgent.getError(error)) {
				cout << "GuildManager::inserGuildAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeGuildAsync(string guildId) {
			apartment_context mainThread;
			co_await resume_background();
			map<string, Guild> cache;
			try_receive(GuildManagerAgent::cache, cache);
			if (cache.contains(guildId)) {
				cache.erase(guildId);
			}
			asend(GuildManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

	protected:
		friend class DiscordCoreClient;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* discordCoreClient{ nullptr };
		DiscordCoreClientBase* discordCoreClientBase{ nullptr };

		GuildManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, DiscordCoreClientBase* coreClientBaseNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
		}

	};
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchGuildData>* GuildManagerAgent::requestFetchGuildBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestGetGuildBuffer;
	unbounded_buffer < DiscordCoreInternal::GetAuditLogData>* GuildManagerAgent::requestGetAuditLogBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outGuildBuffer;
	unbounded_buffer<AuditLogData>* GuildManagerAgent::outAuditLogBuffer;
	concurrent_queue<Guild>* GuildManagerAgent::guildsToInsert;
	unbounded_buffer<exception>* GuildManagerAgent::errorBuffer;
}
#endif