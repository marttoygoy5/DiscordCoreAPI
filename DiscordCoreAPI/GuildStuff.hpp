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
#include "ChannelStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "RoleStuff.hpp"
#include "HttpStuff.hpp"
#include "UserStuff.hpp"

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
		UserManager* users{ nullptr };

		Guild() {}

	protected:
		friend class DiscordCoreClient;
		friend class GuildManagerAgent;
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		Guild(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, UserManager* usersNew) {
			this->initialize(dataNew, agentResourcesNew, guildsNew, threadsNew, usersNew).get();
		}

		task<void> initialize(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, UserManager* usersNew) {
			this->threads = threadsNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->guilds = guildsNew;
			this->channels = new ChannelManager(this, this->agentResources, this->threads);
			for (unsigned int x = 0; x < data.channels.size(); x += 1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData, this, this->agentResources, this->channels, this->threads);
				ChannelManagerAgent::cache.insert(make_pair(channelData.id, channel));
				cout << ChannelManagerAgent::cache.at(channelData.id).data.name << endl;
			}
			this->guildMembers = new GuildMemberManager(this, this->agentResources, this->threads);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this, this->guildMembers);
				GuildMemberManagerAgent::cache.insert(make_pair(guildMember.guild->data.id + guildMemberData.user.id, guildMember));
			}
			this->roles = new RoleManager(this, this->agentResources, this->threads);
			for (auto const& [key, value] : data.roles) {
				DiscordCoreInternal::RoleData roleData = value;
				Role role(roleData, this->agentResources, this->roles, this);
				RoleManagerAgent::cache.insert(make_pair(key, role));
			}
			this->users = usersNew;
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::UserData userData = data.members.at(x).user;
				User newUser(this->threads, this->agentResources, userData, this->users);
				this->users->insertUser(newUser);
			}
			co_return;
		}

	};

	struct GetGuildData{
		string guildId;
	};

	class GuildManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class GuildManager;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestGetBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static map<string, Guild> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::GuildManager* pGuildManager;
		DiscordCoreAPI::UserManager* pUserManager;

		GuildManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildManager* pGuildManagerNew, DiscordCoreAPI::UserManager* pUserManagerNew)
			:agent(*threadsNew->at(8).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pGuildManager = pGuildManagerNew;
			this->pUserManager = pUserManagerNew;
		}

		static task<void> initialize() {
			GuildManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildData>;
			GuildManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildData>;
			GuildManagerAgent::outBuffer = new unbounded_buffer<Guild>;
			co_return;
		}

		task<Guild> getObjectAsync(DiscordCoreInternal::GetGuildData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			DiscordCoreInternal::GuildData guildData = dataPackage.oldGuildData;
			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			Guild guildNew(guildData, this->agentResources, this->pGuildManager, this->threads, this->pUserManager);
			co_return guildNew;
		}

		void run() {
			DiscordCoreInternal::GetGuildData getData;
			try {
				getData = receive(GuildManagerAgent::requestGetBuffer, 1U);
				if (GuildManagerAgent::cache.contains(getData.guildId)) {
					Guild guild = GuildManagerAgent::cache.at(getData.guildId);
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					DiscordCoreInternal::GuildData guildData;
					Guild guildNew(guildData, this->agentResources, this->pGuildManager, this->threads, this->pUserManager);
					send(GuildManagerAgent::outBuffer, guildNew);
				}
			}
			catch (exception error) {}
			try {
				getData = receive(GuildManagerAgent::requestFetchBuffer, 1U);
				if (GuildManagerAgent::cache.contains(getData.guildId)) {
					getData.oldGuildData = GuildManagerAgent::cache.at(getData.guildId).data;
					GuildManagerAgent::cache.erase(getData.guildId);
					Guild guild = getObjectAsync(getData).get();
					GuildManagerAgent::cache.insert(make_pair(getData.guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					Guild guild = getObjectAsync(getData).get();
					GuildManagerAgent::cache.insert(make_pair(getData.guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
			}
			catch (exception error) {}
			DiscordCoreInternal::GuildData guildData;
			Guild guildNew(guildData, this->agentResources, this->pGuildManager, this->threads, this->pUserManager);
			while (GuildManagerAgent::guildsToInsert.try_pop(guildNew)) {
				if (GuildManagerAgent::cache.contains(guildNew.data.id)) {
					GuildManagerAgent::cache.erase(guildNew.data.id);
				}
				GuildManagerAgent::cache.insert(make_pair(guildNew.data.id, guildNew));
			}
			done();
		}
	};

	class GuildManager {
	public:

		GuildManager() {}

		task<Guild> fetchAsync(GetGuildData getGuildData) {
			DiscordCoreInternal::GetGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(2);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this, this->pUserManager);
			send(GuildManagerAgent::requestFetchBuffer, dataPackage);
			guildManagerAgent.start();
			Guild guild = receive(GuildManagerAgent::outBuffer);
			agent::wait(&guildManagerAgent);
			co_return guild;
		}

		task<Guild> getGuildAsync(GetGuildData getGuildData) {
			DiscordCoreInternal::GetGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(2);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this, this->pUserManager);
			send(GuildManagerAgent::requestGetBuffer, dataPackage);
			guildManagerAgent.start();
			Guild guild = receive(GuildManagerAgent::outBuffer);
			agent::wait(&guildManagerAgent);
			co_return guild;
		}

		task<void> insertGuild(Guild guild) {
			GuildManagerAgent managerAgent(this->threads, this->agentResources, this, this->pUserManager);
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

		DiscordCoreAPI::UserManager* pUserManager;

		GuildManager(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::UserManager* pUserManagerNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->pUserManager = pUserManagerNew;
		}

	};
	map<string, Guild> GuildManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestGetBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
}
#endif
