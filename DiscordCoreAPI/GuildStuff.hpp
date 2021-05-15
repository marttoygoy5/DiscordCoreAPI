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

		com_ptr<ChannelManager> channels{ nullptr };
		com_ptr<GuildMemberManager> guildMembers{ nullptr };
		com_ptr<RoleManager> roles{ nullptr };
		com_ptr<GuildManager> guilds{ nullptr };

		Guild() {}
		Guild(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentPointers pointersNew, GuildManager* guildsNew, com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreadsNew) {
			this->initialize(dataNew, pointersNew, guildsNew, pSystemThreadsNew).get();
		}

		task<void> initialize(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentPointers pointersNew, GuildManager* guildsNew, com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreadsNew) {
			this->pSystemThreads = pSystemThreadsNew;
			co_await resume_foreground(*this->pSystemThreads->Threads.at(0).threadQueue.get());
			this->data = dataNew;
			this->pointers = pointersNew;
			this->guilds.attach(guildsNew);
			this->channels = make_self<ChannelManager>(this, this->pointers, this->pSystemThreads);
			for (unsigned int x = 0; x < data.channels.size(); x += 1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData, this->pointers, this->channels, this);
				ChannelManagerAgent::cache.insert(make_pair(channelData.id, channel));
			}
			this->guildMembers = make_self<GuildMemberManager>(this, this->pointers);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this, this->guildMembers);
				this->guildMembers->insert(make_pair(guildMemberData.user.id, guildMember));
			}
			this->roles = make_self<RoleManager>(this, this->pointers, this->pSystemThreads);
			for (auto const& [key, value] : data.roles) {
				DiscordCoreInternal::RoleData roleData = value;
				Role role(roleData, this->pointers, this->roles, this);
				this->roles->insert(make_pair(key, role));
			}
		}

		~Guild() {}

	protected:
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentPointers pointers;
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads;
	};

	class GuildManagerAgent: public agent {
	public:
		static unbounded_buffer<string>* requestBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static map<string, Guild> cache;

		GuildManagerAgent(Scheduler* pScheduler)
			:agent(*pScheduler)
		{}

		static task<void> initialize() {
			GuildManagerAgent::requestBuffer = new unbounded_buffer<string>;
			GuildManagerAgent::outBuffer = new unbounded_buffer<Guild>;
			co_return;
		}

		~GuildManagerAgent() {}

	protected:

		void run() {
			try {
				string guildId = receive(GuildManagerAgent::requestBuffer, 100U);
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
			catch (exception error) {
				//cout << "run() Error: " << error.what() << endl;
			}
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

		GuildManager(DiscordCoreInternal::HttpAgentPointers pointers, Scheduler* pScheduler, com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreadsNew)
		{
			this->pointers = pointers;
			this->pScheduler = pScheduler;
			this->pSystemThreads = pSystemThreadsNew;
		}

		task<Guild> fetchAsync(string guildId) {
			cout << "THIS IS IT THIS IS IT" << endl;
			cout << "THIS IS IT THIS IS IT 2222222" << endl;
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + guildId;
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			cout << "WEVE SEND IT WEVE SEND IT!" << endl;
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			cout << "WEVE GOT IT WEVE GOT IT!" << endl;
			GuildManagerAgent managerAgent(this->pScheduler);
			managerAgent.start();
			DiscordCoreInternal::GuildData guildData;
			try {
				send(GuildManagerAgent::requestBuffer, guildId);
				guildData = receive(GuildManagerAgent::outBuffer, 1000U).data;
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			cout << jsonValue << endl;
			cout << "WEVE FINSHED WEVE FINISHED IT!0000" << endl;
			Guild guild;
			cout << "WEVE FINSHED WEVE FINISHED IT!111" << endl;
			GuildManagerAgent::guildsToInsert.push(guild);
			cout << "WEVE FINSHED WEVE FINISHED IT!2222" << endl;
			co_return guild;
		}

		task<Guild> getGuildAsync(string guildId) {
			try {
				GuildManagerAgent managerAgent(this->pScheduler);
				managerAgent.start();
				send(GuildManagerAgent::requestBuffer, guildId);
				Guild guild = receive(GuildManagerAgent::outBuffer, 1000U);
				co_return guild;
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::GuildData guildData;
				Guild guild = Guild(guildData, this->pointers, this, this->pSystemThreads);
				co_return guild;
			}
		}

		~GuildManager() {}

	protected:

		friend struct DiscordCoreClient;

		Scheduler* pScheduler;

		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads;

		DiscordCoreInternal::HttpAgentPointers pointers;

		task<void> insertGuild(json payload) {
			GuildManagerAgent managerAgent(this->pScheduler);
			managerAgent.start();
			DiscordCoreInternal::GuildData guildData;
			DiscordCoreInternal::parseObject(payload, &guildData);
			Guild guild(guildData, this->pointers, this, this->pSystemThreads);
			cout << "WEVE SENT IT WEVE SENT IT!11111" << endl;
			GuildManagerAgent::guildsToInsert.push(guild);
			cout << "WEVE SENT IT WEVE SENT IT!" << endl;
			agent::wait(&managerAgent);
			co_return;
		}
	};
	map<string, Guild> GuildManagerAgent::cache;
	unbounded_buffer<string>* GuildManagerAgent::requestBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
	
}
#endif
