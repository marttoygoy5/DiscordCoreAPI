// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"
#include "DataParsingFunctions.hpp"
#include "ChannelStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "RoleStuff.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI
{
	class GuildManager;

	class Guild {
	public:

		DiscordCoreInternal::GuildData data;

		ChannelManager* channels{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		RoleManager* roles{ nullptr };
		GuildManager* guilds{ nullptr };

		Guild() {}

	protected:
		friend class DiscordCoreClient;
		friend class GuildManagerAgent;
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Guild(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->initialize(dataNew, agentResourcesNew, guildsNew, threadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
			this->threads = threadsNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->guilds = guildsNew;
			this->channels = new ChannelManager(this, this->agentResources, this->threads);
			for (unsigned int x = 0; x < data.channels.size(); x += 1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData, this, this->agentResources, this->channels, this->threads);
				ChannelManagerAgent::cache.insert(make_pair(channelData.id, channel));
			}
			this->guildMembers = new GuildMemberManager(this, this->agentResources, this->threads);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this, this->guildMembers);
				GuildMemberManagerAgent::cache.insert(make_pair(guildMember.guild->data.id + guildMemberData.user.id, guildMember));
			}
			/*
			
		this->roles = make_self<RoleManager>(this, this->agentResources, this->pSystemThreads);
		for (auto const& [key, value] : data.roles) {
			DiscordCoreInternal::RoleData roleData = value;
			Role role(roleData, this->agentResources, this->roles, this);
			this->roles->insert(make_pair(key, role));
		}
		*/
			co_return;
		}
	};

	class GuildManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class GuildManager;
		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static map<string, Guild> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::GuildManager* pGuildManager;

		GuildManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildManager* pGuildManagerNew)
			:agent(*threadsNew->at(8).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pGuildManager = pGuildManagerNew;
		}

		static task<void> initialize() {
			GuildManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			GuildManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			GuildManagerAgent::outBuffer = new unbounded_buffer<Guild>;
			co_return;
		}

		task<Guild> getObjectAsync(string guildId, Guild guild) {
			co_await resume_foreground(*this->threads->at(3).threadQueue.get());
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(3).scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::GuildData guildData = guild.data;
			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			Guild guildNew(guildData, this->agentResources, this->pGuildManager, this->threads);
			co_return guildNew;
		}

		void run() {
			string guildId;
			try {
				guildId = receive(GuildManagerAgent::requestGetBuffer, 1U);
				if (GuildManagerAgent::cache.contains(guildId)) {
					Guild guild = GuildManagerAgent::cache.at(guildId);
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					DiscordCoreInternal::GuildData guildData;
					Guild guild(guildData, this->agentResources, this->pGuildManager, this->threads);
					send(GuildManagerAgent::outBuffer, guild);
				}
			}
			catch (exception error) {}
			try {
				guildId = receive(GuildManagerAgent::requestFetchBuffer, 1U);
				if (GuildManagerAgent::cache.contains(guildId)) {
					Guild guild = GuildManagerAgent::cache.at(guildId);
					GuildManagerAgent::cache.erase(guildId);
					guild = getObjectAsync(guildId, guild).get();
					GuildManagerAgent::cache.insert(make_pair(guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					Guild guild = getObjectAsync(guildId, guild).get();
					GuildManagerAgent::cache.insert(make_pair(guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
			}
			catch (exception error) {}
			DiscordCoreInternal::GuildData guildData;
			Guild guild(guildData, this->agentResources, this->pGuildManager, this->threads);
			while (GuildManagerAgent::guildsToInsert.try_pop(guild)) {
				if (GuildManagerAgent::cache.contains(guild.data.id)) {
					GuildManagerAgent::cache.erase(guild.data.id);
				}
				GuildManagerAgent::cache.insert(make_pair(guild.data.id, guild));
			}
			done();
		}

	};

	class GuildManager {
	public:

		GuildManager() {}

		task<Guild> fetchAsync(string guildId) {
			co_await resume_foreground(*this->threads->at(3).threadQueue.get());
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this);
			send(GuildManagerAgent::requestFetchBuffer, guildId);
			guildManagerAgent.start();
			Guild guild = receive(GuildManagerAgent::outBuffer);
			agent::wait(&guildManagerAgent);
			co_return guild;
		}

		task<Guild> getGuildAsync(string guildId) {
			co_await resume_foreground(*this->threads->at(3).threadQueue.get());
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this);
			send(GuildManagerAgent::requestGetBuffer, guildId);
			guildManagerAgent.start();
			Guild guild = receive(GuildManagerAgent::outBuffer);
			agent::wait(&guildManagerAgent);
			co_return guild;
		}

		task<void> insertGuild(Guild guild) {
			GuildManagerAgent managerAgent(this->threads, this->agentResources, this);
			managerAgent.start();
			GuildManagerAgent::guildsToInsert.push(guild);
			managerAgent.wait(&managerAgent);
			cout << "CURRENT GUILD NAME: " << GuildManagerAgent::cache.at(guild.data.id).data.name << endl;
			co_return;
		}

	protected:

		friend class DiscordCoreClient;

		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };

		DiscordCoreInternal::HttpAgentResources agentResources;

		GuildManager(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
		}

	};
	map<string, Guild> GuildManagerAgent::cache;
	unbounded_buffer<string>* GuildManagerAgent::requestFetchBuffer;
	unbounded_buffer<string>* GuildManagerAgent::requestGetBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
}
#endif
