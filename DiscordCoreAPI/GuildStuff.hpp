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
#include "GuildMemberStuff.hpp"
#include "RoleStuff.hpp"
#include "UserStuff.hpp"

namespace DiscordCoreAPI
{
	class GuildManager;

	class DiscordCoreClient;

	class DiscordCoreClientBase {
	public:
		ChannelManager* channels{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		RoleManager* roles{ nullptr };
		UserManager* users{ nullptr };
	};

	class Guild {
	public:

		DiscordCoreInternal::GuildData data;
		DiscordCoreClientBase* clientCoreBase{ nullptr };
		DiscordCoreClient* clientCore{ nullptr };
		Guild() {};

	protected:
		friend class GuildManagerAgent;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientExt;
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };

		Guild(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::GuildData dataNew,  DiscordCoreClient* clientCoreNew, DiscordCoreClientBase* clientCoreBaseNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->clientCore = clientCoreNew;
			this->clientCoreBase = clientCoreBaseNew;
			this->data = dataNew;
		}

		task<void> initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
					Channel channel(channelData, this->clientCore);
					this->clientCoreBase->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
					guildMemberData.guildId = this->data.id;
					GuildMember guildMember(guildMemberData, this->clientCore);
					this->clientCoreBase->guildMembers->insertGuildMemberAsync(guildMember).get();
				}
				cout << "Caching roles for guild: " << this->data.name << endl;
				for (auto const& [key, value] : data.roles) {
					DiscordCoreInternal::RoleData roleData = value;
					Role role(this->agentResources, roleData, this->clientCore);
					this->clientCoreBase->roles->insertRoleAsync(role).get();
				}
				cout << "Caching users for guild: " << this->data.name << endl << endl;
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					DiscordCoreInternal::UserData userData = data.members.at(x).user;
					User user(userData, this->clientCore);
					this->clientCoreBase->users->insertUserAsync(user).get();
				}
			}
			catch (exception error) {
				cout << "Error: " << error.what() << endl;
			}

			co_return;
		}
	};
	
	struct GetGuildData {
		string guildId;
	};

	struct FetchGuildData {
		string guildId;
	};

	class GuildManagerAgent : public agent {
	protected:
		friend class DiscordCoreClientExt;
		friend class DiscordCoreClient;
		friend class GuildManager;
		static unbounded_buffer<DiscordCoreInternal::FetchGuildData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestGetBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static overwrite_buffer<map<string, Guild>> cache;
		
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClientBase* clientCoreBase{ nullptr };
		DiscordCoreClient* clientCore{ nullptr };

		GuildManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew,  DiscordCoreClient* clientCoreNew, DiscordCoreClientBase* clientCoreBaseNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->clientCore = clientCoreNew;
			this->clientCoreBase = clientCoreBaseNew;
		}

		static task<void> initialize() {
			GuildManagerAgent::requestFetchBuffer = new unbounded_buffer<DiscordCoreInternal::FetchGuildData>;
			GuildManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildData>;
			GuildManagerAgent::outBuffer = new unbounded_buffer<Guild>;
			co_return;
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
			json jsonValue;
			try_receive(requestAgent.workReturnBuffer, jsonValue);
			DiscordCoreInternal::GuildData guildData = dataPackage.oldGuildData;
			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			Guild guildNew(this->agentResources, this->threads, guildData, this->clientCore, this->clientCoreBase);
			co_return guildNew;
		}

		void run() {
			DiscordCoreInternal::GetGuildData dataPackage;
			if (try_receive(GuildManagerAgent::requestGetBuffer, dataPackage)) {
				map<string, Guild> cacheTemp;
				if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackage.guildId)) {
						Guild guild = cacheTemp.at(dataPackage.guildId);
						send(GuildManagerAgent::outBuffer, guild);
					}
				}
			}
			DiscordCoreInternal::FetchGuildData fetchData;
			if (try_receive(GuildManagerAgent::requestFetchBuffer, fetchData)) {
				map<string, Guild> cacheTemp;
				if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(fetchData.guildId)) {
						dataPackage.oldGuildData = cacheTemp.at(fetchData.guildId).data;
						cacheTemp.erase(fetchData.guildId);
					}
				}
				fetchData.oldGuildData = dataPackage.oldGuildData;
				Guild guild = getObjectAsync(fetchData).get();
				cacheTemp.insert(make_pair(fetchData.guildId, guild));
				send(GuildManagerAgent::outBuffer, guild);
				asend(cache, cacheTemp);
			}
			DiscordCoreInternal::GuildData guildData;
			Guild guildNew(this->agentResources, this->threads, guildData, this->clientCore, this->clientCoreBase);
			while (GuildManagerAgent::guildsToInsert.try_pop(guildNew)) {
				map<string, Guild> cacheTemp;
				if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
				}				
				guildNew.initialize().get();
				cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
				asend(cache, cacheTemp);
			}
			done();
		}
	};

	class GuildManager {
	public:

		task<Guild> fetchAsync(FetchGuildData getGuildData) {
			DiscordCoreInternal::FetchGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->agentResources, this->threads, this->clientCore, this->clientCoreBase, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestFetchBuffer, dataPackage);
			guildManagerAgent.start();
			agent::wait(&guildManagerAgent);
			DiscordCoreInternal::GuildData guildData;
			Guild guild(this->agentResources, this->threads, guildData, this->clientCore, this->clientCoreBase);
			try_receive(GuildManagerAgent::outBuffer, guild);
			co_return guild;
		}

		task<Guild> getGuildAsync(GetGuildData getGuildData) {
			DiscordCoreInternal::GetGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->agentResources, this->threads, this->clientCore, this->clientCoreBase, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestGetBuffer, dataPackage);
			guildManagerAgent.start();
			agent::wait(&guildManagerAgent);
			DiscordCoreInternal::GuildData guildData;
			Guild guild(this->agentResources, this->threads, guildData, this->clientCore, this->clientCoreBase);
			try_receive(GuildManagerAgent::outBuffer, guild);
			co_return guild;
		}

		task<void> insertGuildAsync(Guild guild) {
			GuildManagerAgent guildManagerAgent(this->agentResources, this->threads, this->clientCore, this->clientCoreBase, this->threads->at(2).scheduler);
			GuildManagerAgent::guildsToInsert.push(guild);
			guildManagerAgent.start();
			guildManagerAgent.wait(&guildManagerAgent);
			co_return;
		}

	protected:

		friend class DiscordCoreClientExt;
		friend class DiscordCoreClient;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* clientCore{ nullptr };
		DiscordCoreClientBase* clientCoreBase{ nullptr };

		GuildManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* clientCoreNew, DiscordCoreClientBase* clientCoreBaseNew) {
			this->threads = threadsNew;
			this->agentResources = agentResourcesNew;
			this->clientCore = clientCoreNew;
			this->clientCoreBase = clientCoreBaseNew;
		}

	};
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchGuildData>* GuildManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestGetBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
}
#endif