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

	class Guild;
	
	class GuildMember {
	public:
		DiscordCoreInternal::GuildMemberData data;
		GuildMemberManager* guildMembers{ nullptr };
		GuildMember() {}
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, GuildMemberManager* guildMembersNew) {
			this->data = guildMemberData;
			this->guildMembers = guildMembersNew;
		}

	protected:
		friend class GuildMemberManager;

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
		static unbounded_buffer<GuildMember>* outBuffer;
		static concurrent_queue<GuildMember> guildMembersToInsert;
		static overwrite_buffer<map<string, GuildMember>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::GuildMemberManager* pGuildMemberManager;

		GuildMemberManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, Scheduler* pScheduler, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildMemberManager* pGuildMemberManagerNew)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pGuildMemberManager = pGuildMemberManagerNew;
		}

		static task<void> initialize() {
			GuildMemberManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::outBuffer = new unbounded_buffer<GuildMember>;
			co_return;
		}

		task<GuildMember> getGuildMemberAsync(DiscordCoreInternal::GetGuildMemberData getData){
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + getData.guildId + "/members/" + getData.guildMemberId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(getData.agentResources, getData.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::GuildMemberData guildMemberData = getData.oldGuildMemberData;
			DiscordCoreInternal::parseObject(jsonValue, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, this->pGuildMemberManager);
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
				GuildMember GuildMember = getGuildMemberAsync(getData).get();
				cacheTemp.insert(make_pair(getData.guildId + getData.guildMemberId, GuildMember));
				send(GuildMemberManagerAgent::outBuffer, GuildMember);
				asend(GuildMemberManagerAgent::cache, cacheTemp);
			}
			GuildMember guildMember;
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

		task<GuildMember> fetchAsync(GetGuildMemberData getGuildMemberData) {
			DiscordCoreInternal::GetGuildMemberData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildMemberData.guildId;
			dataPackage.guildMemberId = getGuildMemberData.guildMemberId;
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(2).scheduler, dataPackage.agentResources, this);
			DiscordCoreInternal::GuildMemberData guildMemberDat;
			send(GuildMemberManagerAgent::requestFetchBuffer, dataPackage);
			managerAgent.start();
			GuildMember guildMember = receive(GuildMemberManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData getGuildMemberData) {
			DiscordCoreInternal::GetGuildMemberData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildMemberData.guildId;
			dataPackage.guildMemberId = getGuildMemberData.guildMemberId;
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(2).scheduler, dataPackage.agentResources, this);
			send(GuildMemberManagerAgent::requestGetBuffer, dataPackage);
			managerAgent.start();
			GuildMember guildMember = receive(GuildMemberManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return guildMember;
		}

		task<void> insertGuildMember(GuildMember guildMember) {
			GuildMemberManagerAgent guildMemberManagerAgent(this->threads, this->threads->at(2).scheduler, this->agentResources, this);
			guildMemberManagerAgent.start();
			GuildMemberManagerAgent::guildMembersToInsert.push(guildMember);
			guildMemberManagerAgent.wait(&guildMemberManagerAgent);
			co_return;
		}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		friend class Guild;

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestGetBuffer;
	unbounded_buffer<GuildMember>* GuildMemberManagerAgent::outBuffer;
	concurrent_queue<GuildMember> GuildMemberManagerAgent::guildMembersToInsert;
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache;
};
#endif