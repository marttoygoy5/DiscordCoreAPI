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
	public:

		static map<string, GuildMember> cache;

	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class GuildMemberManager;

		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestGetBuffer;
		static unbounded_buffer<GuildMember>* outBuffer;
		static concurrent_queue<GuildMember> guildMembersToInsert;

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
			try {
				getData = receive(GuildMemberManagerAgent::requestGetBuffer, 1U);
				if (GuildMemberManagerAgent::cache.contains(getData.guildId + getData.guildMemberId)) {
					GuildMember GuildMember = GuildMemberManagerAgent::cache.at(getData.guildId + getData.guildMemberId);
					send(GuildMemberManagerAgent::outBuffer, GuildMember);
				}
				else {
					GuildMember GuildMember;
					send(GuildMemberManagerAgent::outBuffer, GuildMember);
				}
			}
			catch (exception error) {}
			try {
				getData = receive(GuildMemberManagerAgent::requestFetchBuffer, 1U);
				if (GuildMemberManagerAgent::cache.contains(getData.guildId + getData.guildMemberId)) {
					getData.oldGuildMemberData = GuildMemberManagerAgent::cache.at(getData.guildId + getData.guildMemberId).data;
					GuildMemberManagerAgent::cache.erase(getData.guildId + getData.guildMemberId);
					GuildMember GuildMember = getGuildMemberAsync(getData).get();
					GuildMemberManagerAgent::cache.insert(make_pair(getData.guildId + getData.guildMemberId, GuildMember));
					send(GuildMemberManagerAgent::outBuffer, GuildMember);
				}
				else {
					GuildMember GuildMember = getGuildMemberAsync(getData).get();
					GuildMemberManagerAgent::cache.insert(make_pair(getData.guildId + getData.guildMemberId, GuildMember));
					send(GuildMemberManagerAgent::outBuffer, GuildMember);
				}
			}
			catch (exception error) {}
			GuildMember guildMember;
			while (GuildMemberManagerAgent::guildMembersToInsert.try_pop(guildMember)) {
				if (GuildMemberManagerAgent::cache.contains(guildMember.data.guildId + guildMember.data.user.id)) {
					GuildMemberManagerAgent::cache.erase(guildMember.data.guildId + guildMember.data.user.id);
				}
				GuildMemberManagerAgent::cache.insert(make_pair(guildMember.data.guildId + guildMember.data.user.id, guildMember));
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
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(4).scheduler, dataPackage.agentResources, this);
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
			GuildMemberManagerAgent managerAgent(this->threads, this->threads->at(4).scheduler, dataPackage.agentResources, this);
			send(GuildMemberManagerAgent::requestGetBuffer, dataPackage);
			managerAgent.start();
			GuildMember guildMember = receive(GuildMemberManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return guildMember;
		}

		task<void> insertGuildMember(GuildMember guildMember) {
			GuildMemberManagerAgent guildMemberManagerAgent(this->threads, this->threads->at(4).scheduler, this->agentResources, this);
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
	map<string, GuildMember> GuildMemberManagerAgent::cache;
};
#endif