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

		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<GuildManager> guilds{ nullptr };

		Guild() {}
		Guild(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew) {
			this->initialize(dataNew, agentResourcesNew, guildsNew, pThreadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew) {
			this->pThreads = pThreadsNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->guilds.reset(guildsNew);
			this->channels = make_shared<ChannelManager>(this, this->agentResources, this->pThreads);
			/*
			for (unsigned int x = 0; x < data.channels.size(); x += 1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData, this->agentResources, this->channels, this);
				ChannelManagerAgent::cache.insert(make_pair(channelData.id, channel));
			}
			this->guildMembers = make_self<GuildMemberManager>(this, this->agentResources);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this, this->guildMembers);
				this->guildMembers->insert(make_pair(guildMemberData.user.id, guildMember));
			}
			this->roles = make_self<RoleManager>(this, this->agentResources, this->pSystemThreads);
			for (auto const& [key, value] : data.roles) {
				DiscordCoreInternal::RoleData roleData = value;
				Role role(roleData, this->agentResources, this->roles, this);
				this->roles->insert(make_pair(key, role));
			}
			*/
			co_return;
		}

		~Guild() {}

	protected:
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;
	};

	class GuildManagerAgent: public agent {
	public:
		static unbounded_buffer<string>* requestFetchBuffer;
		static unbounded_buffer<string>* requestGetBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static map<string, Guild> cache;

		GuildManagerAgent(shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildManager* pGuildManagerNew)
			:agent(*pThreadsNew->at(0).scheduler)
		{
			//cout << pThreads->size() << endl;
			this->agentResources = agentResourcesNew;
			this->pThreads = pThreadsNew;
			this->pGuildManager = pGuildManagerNew;
		}

		static task<void> initialize() {
			GuildManagerAgent::requestFetchBuffer = new unbounded_buffer<string>;
			GuildManagerAgent::requestGetBuffer = new unbounded_buffer<string>;
			GuildManagerAgent::outBuffer = new unbounded_buffer<Guild>;
			co_return;
		}

		~GuildManagerAgent() {}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;
		DiscordCoreAPI::GuildManager* pGuildManager;

		task<Guild> fetchAsync(string guildId, Guild guild) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->pThreads->at(0).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			DiscordCoreInternal::GuildData guildData = guild.data;
			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			Guild guildNew(guildData, this->agentResources, this->pGuildManager, this->pThreads);
			co_return guildNew;
		}

		void run() {
			string guildId;
			try {
				guildId = receive(GuildManagerAgent::requestGetBuffer, 50U);
				cout << "GUILD ID: " << guildId << endl;
				if (GuildManagerAgent::cache.contains(guildId)) {
					Guild guild = GuildManagerAgent::cache.at(guildId);
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					Guild guild;
					send(GuildManagerAgent::outBuffer, guild);
				}
			}
			catch (exception error) {}
			try {
				guildId = receive(GuildManagerAgent::requestFetchBuffer, 50U);
				cout << "GUILD ID: " << guildId << endl;
				if (GuildManagerAgent::cache.contains(guildId)) {
					Guild guild = GuildManagerAgent::cache.at(guildId);
					GuildManagerAgent::cache.erase(guildId);
					guild = fetchAsync(guildId, guild).get();
					GuildManagerAgent::cache.insert(make_pair(guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
				else {
					Guild guild = fetchAsync(guildId, guild).get();
					GuildManagerAgent::cache.insert(make_pair(guildId, guild));
					send(GuildManagerAgent::outBuffer, guild);
				}
			}
			catch (exception error) {}
			Guild guild;
			while (GuildManagerAgent::guildsToInsert.try_pop(guild)) {
				if (GuildManagerAgent::cache.contains(guild.data.id)) {
					GuildManagerAgent::cache.erase(guild.data.id);
					cout << guild.data.name << endl;
				}
				GuildManagerAgent::cache.insert(make_pair(guild.data.id, guild));
				cout << GuildManagerAgent::cache.at(guild.data.id).data.name << endl;
			}
			done();
		}

	};

	class GuildManager :  public implements <GuildManager, winrt::Windows::Foundation::IInspectable> {
	public:

		GuildManager(shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreadsNew,  DiscordCoreInternal::HttpAgentResources agentResourcesNew)
		{
			this->pThreads = pThreadsNew;
			this->agentResources = agentResourcesNew;
		}

		task<Guild> fetchAsync(string guildId) {
			cout << "THIS IS IT THIS IS IT" << endl;
			cout << "THIS IS IT THIS IS IT 2222222" << endl;
			GuildManagerAgent guildManagerAgent(this->pThreads, this->agentResources, this);
			guildManagerAgent.start();
			send(GuildManagerAgent::requestFetchBuffer, guildId);
			Guild guild = receive(GuildManagerAgent::outBuffer);
			co_return guild;
		}

		task<Guild> getGuildAsync(string guildId) {
			GuildManagerAgent guildManagerAgent(this->pThreads, this->agentResources, this);
			guildManagerAgent.start();
			send(GuildManagerAgent::requestGetBuffer, guildId);
			Guild guild = receive(GuildManagerAgent::outBuffer);
			co_return guild;
		}

		~GuildManager() {}

	protected:

		friend struct DiscordCoreClient;

		shared_ptr<concurrent_vector<DiscordCoreInternal::ThreadContext>> pThreads;

		DiscordCoreInternal::HttpAgentResources agentResources;

		task<void> insertGuild(json payload) {
			cout << this->pThreads->size() << " = THREADS SIZE THREADS SIZE" << endl;
			GuildManagerAgent managerAgent(this->pThreads, this->agentResources,this);
			managerAgent.start();
			DiscordCoreInternal::GuildData guildData;
			DiscordCoreInternal::parseObject(payload, &guildData);
			Guild guild(guildData, this->agentResources, this, this->pThreads);
			//cout << payload << endl;
			cout << "WEVE SENT IT WEVE SENT IT!11111" << endl;
			GuildManagerAgent::guildsToInsert.push(guild);
			cout << "WEVE SENT IT WEVE SENT IT!" << endl;
			managerAgent.wait(&managerAgent);
			cout << "CURRENT THREAD ID: " << this_thread::get_id() << endl;
			co_return;
		}
	};
	map<string, Guild> GuildManagerAgent::cache;
	unbounded_buffer<string>* GuildManagerAgent::requestFetchBuffer;
	unbounded_buffer<string>* GuildManagerAgent::requestGetBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
	
}
#endif
