// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "pch.h"
#include "DatabaseStuff.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class GuildMemberManager;

	class RoleManager;

	class UserManager;

	class DiscordCoreClientBase {
	public:
		ChannelManager* channels{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		RoleManager* roles{ nullptr };
		UserManager* users{ nullptr };
	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		static map<string, DiscordGuild> guildMap;
		static map<string, DiscordGuildMember> guildMemberMap;
	};

	class GuildMember {
	public:
		GuildMemberData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		GuildMember() {};

	protected:
		friend class DiscordCoreClient;
		friend class GuildMemberManager;
		friend class GuildMemberManagerAgent;
		friend class Guild;

		GuildMember(GuildMemberData guildMemberData, DiscordCoreClient* discordCoreClientNew) {
			this->data = guildMemberData;
			this->discordCoreClient = discordCoreClientNew;
		}

	};

	struct FetchGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	struct GetGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	class GuildMemberManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class GuildMemberManager;

		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestFetchGuildMemberBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestGetGuildMemberBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* requestGetRolesBuffer;
		static unbounded_buffer<GuildMember>* outGuildMemberBuffer;
		static concurrent_queue<GuildMember> guildMembersToInsert;
		static overwrite_buffer<map<string, GuildMember>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		GuildMemberManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, Scheduler* pScheduler, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreClient* discordCoreClientNew)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize() {
			GuildMemberManagerAgent::requestFetchGuildMemberBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetGuildMemberBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetRolesBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>;
			GuildMemberManagerAgent::outGuildMemberBuffer = new unbounded_buffer<GuildMember>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<GuildMember> getObjectDataAsync(DiscordCoreInternal::GetGuildMemberData dataPackage){
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManagerAgent::getObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::getObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildMemberData guildMemberData;
			DiscordCoreInternal::parseObject(returnData.data, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, this->discordCoreClient);
			co_return guildMemberNew;
		}

		void run() {
			try {
				DiscordCoreInternal::GetGuildMemberData dataPackage01;
				if (try_receive(GuildMemberManagerAgent::requestGetGuildMemberBuffer, dataPackage01)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + dataPackage01.guildMemberId)) {
							GuildMember GuildMember = cacheTemp.at(dataPackage01.guildId + dataPackage01.guildMemberId);
							send(GuildMemberManagerAgent::outGuildMemberBuffer, GuildMember);
						}
					}
				}
				if (try_receive(GuildMemberManagerAgent::requestFetchGuildMemberBuffer, dataPackage01)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + dataPackage01.guildMemberId)) {
							dataPackage01.oldGuildMemberData = cacheTemp.at(dataPackage01.guildId + dataPackage01.guildMemberId).data;
							cacheTemp.erase(dataPackage01.guildId + dataPackage01.guildMemberId);
						}
					}
					GuildMember GuildMember = getObjectDataAsync(dataPackage01).get();
					cacheTemp.insert(make_pair(dataPackage01.guildId + dataPackage01.guildMemberId, GuildMember));
					send(GuildMemberManagerAgent::outGuildMemberBuffer, GuildMember);
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
				GuildMemberData dataPackage02;
				GuildMember guildMember(dataPackage02, this->discordCoreClient);
				while (GuildMemberManagerAgent::guildMembersToInsert.try_pop(guildMember)) {
					map<string, GuildMember> cacheTemp;
					try_receive(GuildMemberManagerAgent::cache, cacheTemp);
					cacheTemp.insert(make_pair(guildMember.data.guildId + guildMember.data.user.id, guildMember));
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class GuildMemberManager {
	public:

		task<GuildMember> fetchAsync(FetchGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threads, this->threads->at(2).scheduler, dataPackageNew.agentResources, this->discordCoreClient);
			send(GuildMemberManagerAgent::requestFetchGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->discordCoreClient);
			try_receive(GuildMemberManagerAgent::outGuildMemberBuffer, guildMember);
			co_await mainThread;
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(3);
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threads, this->threads->at(2).scheduler, dataPackageNew.agentResources, this->discordCoreClient);
			send(GuildMemberManagerAgent::requestGetGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->discordCoreClient);
			try_receive(GuildMemberManagerAgent::outGuildMemberBuffer, guildMember);
			co_await mainThread;
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(GuildMember guildMember) {
			GuildMemberManagerAgent guildMemberManagerAgent(this->threads, this->threads->at(2).scheduler, this->agentResources, this->discordCoreClient);
			GuildMemberManagerAgent::guildMembersToInsert.push(guildMember);
			guildMemberManagerAgent.start();
			agent::wait(&guildMemberManagerAgent);
			exception error;
			while (guildMemberManagerAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestFetchGuildMemberBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestGetGuildMemberBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* GuildMemberManagerAgent::requestGetRolesBuffer;
	unbounded_buffer<GuildMember>* GuildMemberManagerAgent::outGuildMemberBuffer;
	concurrent_queue<GuildMember> GuildMemberManagerAgent::guildMembersToInsert;
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache;
};
#endif