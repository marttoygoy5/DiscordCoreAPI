// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "pch.h"
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
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
	};

	class GuildMember {
	public:
		DiscordCoreInternal::GuildMemberData data;
		DiscordCoreClient* coreClient{ nullptr };

		GuildMember() {};

	protected:
		friend class GuildMemberManager;
		friend class GuildMemberManagerAgent;
		friend class Guild;

		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, DiscordCoreClient* coreClientNew) {
			this->data = guildMemberData;
			this->coreClient = coreClientNew;
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

		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestGetBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* requestRolesGetBuffer;
		static unbounded_buffer<GuildMember>* outBuffer;
		static concurrent_queue<GuildMember> guildMembersToInsert;
		static overwrite_buffer<map<string, GuildMember>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		GuildMemberManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, Scheduler* pScheduler, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreClient* coreClientNew)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			GuildMemberManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestRolesGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>;
			GuildMemberManagerAgent::outBuffer = new unbounded_buffer<GuildMember>;
			return;
		}

		task<GuildMember> getObjectAsync(DiscordCoreInternal::GetGuildMemberData getData){
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + getData.guildId + "/members/" + getData.guildMemberId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(getData.agentResources, getData.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			try {
				agent::wait(&requestAgent);
			}
			catch (const exception& e) {
				cout << "GuildMemberManagerAgent::getObjectAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
			}
			DiscordCoreInternal::GuildMemberData guildMemberData = getData.oldGuildMemberData;
			DiscordCoreInternal::parseObject(returnData.data, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, this->coreClient);
			co_return guildMemberNew;
		}

		void run() {
			DiscordCoreInternal::GetGuildMemberData getData;
			if (try_receive(GuildMemberManagerAgent::requestGetBuffer, getData)) {
				map<string, GuildMember> cacheTemp;
				if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.guildId + getData.guildMemberId)) {
						GuildMember GuildMember = cacheTemp.at(getData.guildId + getData.guildMemberId);
						send(GuildMemberManagerAgent::outBuffer, GuildMember);
					}
				}
			}
			if (try_receive(GuildMemberManagerAgent::requestFetchBuffer, getData)) {
				map<string, GuildMember> cacheTemp;
				if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.guildId + getData.guildMemberId)) {
						getData.oldGuildMemberData = cacheTemp.at(getData.guildId + getData.guildMemberId).data;
						cacheTemp.erase(getData.guildId + getData.guildMemberId);
					}
				}				
				GuildMember GuildMember = getObjectAsync(getData).get();
				cacheTemp.insert(make_pair(getData.guildId + getData.guildMemberId, GuildMember));
				send(GuildMemberManagerAgent::outBuffer, GuildMember);
				asend(GuildMemberManagerAgent::cache, cacheTemp);
			}
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->coreClient);
			while (GuildMemberManagerAgent::guildMembersToInsert.try_pop(guildMember)) {
				map<string, GuildMember> cacheTemp;
				if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(guildMember.data.guildId + guildMember.data.user.id)) {
						cacheTemp.erase(guildMember.data.guildId + guildMember.data.user.id);
					}
				}
				cacheTemp.insert(make_pair(guildMember.data.guildId + guildMember.data.user.id, guildMember));
				asend(GuildMemberManagerAgent::cache, cacheTemp);
			}
			done();
		}
	};

	class GuildMemberManager {
	public:

		task<GuildMember> fetchAsync(FetchGuildMemberData getGuildMemberData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetGuildMemberData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildMemberData.guildId;
			dataPackage.guildMemberId = getGuildMemberData.guildMemberId;
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(2).scheduler, dataPackage.agentResources, this->coreClient);
			DiscordCoreInternal::GuildMemberData guildMemberDat;
			send(GuildMemberManagerAgent::requestFetchBuffer, dataPackage);
			managerAgent.start();
			try {
				agent::wait(&managerAgent);
			}
			catch (const exception& e) {
				cout << "GuildMemberManager::fetchAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->coreClient);
			try_receive(GuildMemberManagerAgent::outBuffer, guildMember);
			co_await mainThread;
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData getGuildMemberData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetGuildMemberData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildMemberData.guildId;
			dataPackage.guildMemberId = getGuildMemberData.guildMemberId;
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(2).scheduler, dataPackage.agentResources, this->coreClient);
			send(GuildMemberManagerAgent::requestGetBuffer, dataPackage);
			managerAgent.start();
			try {
				agent::wait(&managerAgent);
			}
			catch (const exception& e) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->coreClient);
			try_receive(GuildMemberManagerAgent::outBuffer, guildMember);
			co_await mainThread;
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(GuildMember guildMember) {
			GuildMemberManagerAgent guildMemberManagerAgent(this->threads, this->threads->at(2).scheduler, this->agentResources, this->coreClient);
			guildMemberManagerAgent.start();
			GuildMemberManagerAgent::guildMembersToInsert.push(guildMember);
			try {
				agent::wait(&guildMemberManagerAgent);
			}
			catch (const exception& e) {
				cout << "GuildMemberManager::insertGuildMemberAsync() Error: " << e.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestGetBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* GuildMemberManagerAgent::requestRolesGetBuffer;
	unbounded_buffer<GuildMember>* GuildMemberManagerAgent::outBuffer;
	concurrent_queue<GuildMember> GuildMemberManagerAgent::guildMembersToInsert;
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache;
};
#endif