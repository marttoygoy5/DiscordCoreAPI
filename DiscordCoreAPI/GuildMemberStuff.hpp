// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"

namespace DiscordCoreAPI {

	class GuildMemberManager;

	class Guild;

	class GuildMember {
	public:
		DiscordCoreInternal::GuildMemberData data;
		Guild* guild{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		GuildMember() {}
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, Guild* guildNew, GuildMemberManager* guildMembersNew) {
			this->data = guildMemberData;
			this->guild = guildNew;
			this->guildMembers = guildMembersNew;
		}

	protected:
		friend class GuildMemberManager;

	};

	struct GetGuildMemberData {
		string guildId;
		string guildMemberId;
		DiscordCoreInternal::GuildMemberData oldGuildMemberData;
	};

	class GuildMemberManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class GuildMemberManager;

		static unbounded_buffer<GetGuildMemberData>* requestFetchBuffer;
		static unbounded_buffer<GetGuildMemberData>* requestGetBuffer;
		static unbounded_buffer<GuildMember>* outBuffer;
		static concurrent_queue<GuildMember> GuildMembersToInsert;
		static map<string, GuildMember> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::GuildMemberManager* pGuildMemberManager;
		Guild* pGuild;

		GuildMemberManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildMemberManager* pGuildMemberManagerNew, Guild* pGuildNew)
			:agent(*threadsNew->at(8).scheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pGuildMemberManager = pGuildMemberManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			GuildMemberManagerAgent::requestFetchBuffer = new unbounded_buffer<GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetBuffer = new unbounded_buffer<GetGuildMemberData>;
			GuildMemberManagerAgent::outBuffer = new unbounded_buffer<GuildMember>;
			co_return;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData getData){
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + getData.guildId + "/members/" + getData.guildMemberId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(3).scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::GuildMemberData guildMemberData = getData.oldGuildMemberData;
			DiscordCoreInternal::parseObject(jsonValue, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, this->pGuild, this->pGuildMemberManager);
			co_return guildMemberNew;
		}

		void run() {
			GetGuildMemberData getData;
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
			while (GuildMemberManagerAgent::GuildMembersToInsert.try_pop(guildMember)) {
				if (GuildMemberManagerAgent::cache.contains(getData.guildId + guildMember.data.user.id)) {
					GuildMemberManagerAgent::cache.erase(getData.guildId + guildMember.data.user.id);
				}
				GuildMemberManagerAgent::cache.insert(make_pair(getData.guildId + guildMember.data.user.id, guildMember));
			}
			done();
		}
	};

	class GuildMemberManager {
	public:
		Guild* guild{ nullptr };

		task<GuildMember> fetchAsync(string guildId, string guildMemberId) {
			GuildMember guildMember;
			co_return guildMember;
		}

		task<Guild> fetchAsync(string guildId) {
			co_await resume_foreground(*this->threads->at(3).threadQueue.get());
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this);
			send(GuildManagerAgent::requestFetchBuffer, guildId);
			guildManagerAgent.start();
			Guild guild = receive(GuildManagerAgent::outBuffer);
			agent::wait(&guildManagerAgent);
			co_return guild;
		}

		task<GuildMember> fetchAsync(string guildId, string guildMemberId) {
			GuildMemberManagerAgent managerAgent(this->threads, this->agentResources, this, this->guild);
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GetGuildMemberData getData;
			getData.guildId = guildId;
			getData.guildMemberId = guildMemberId;
			send(GuildMemberManagerAgent::requestGetBuffer, getData);
			managerAgent.start();
			GuildMember guildMember = receive(GuildMemberManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(string guildId, string guildMemberId) {
			GuildMemberManagerAgent managerAgent(this->threads, this->agentResources, this, this->guild);
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GetGuildMemberData getData;
			getData.guildId = guildId;
			getData.guildMemberId = guildMemberId;
			send(GuildMemberManagerAgent::requestGetBuffer, getData);
			managerAgent.start();
			GuildMember guildMember = receive(GuildMemberManagerAgent::outBuffer);
			agent::wait(&managerAgent);
			co_return guildMember;
		}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		friend class Guild;

		GuildMemberManager(Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->guild = guildNew;
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
		}
	};
	unbounded_buffer<GetGuildMemberData>* GuildMemberManagerAgent::requestFetchBuffer;
	unbounded_buffer<GetGuildMemberData>* GuildMemberManagerAgent::requestGetBuffer;
	unbounded_buffer<GuildMember>* GuildMemberManagerAgent::outBuffer;
	concurrent_queue<GuildMember> GuildMemberManagerAgent::GuildMembersToInsert;
	map<string, GuildMember> GuildMemberManagerAgent::cache;
};
#endif
