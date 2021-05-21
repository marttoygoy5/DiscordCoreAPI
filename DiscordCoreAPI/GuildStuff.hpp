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
		MessageManager* messages{ nullptr };

		Guild(DiscordCoreInternal::GuildData dataNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildManager* guildsNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, UserManager* usersNew) {
			this->threads = threadsNew;
			this->data = dataNew;
			this->agentResources = agentResourcesNew;
			this->guilds = guildsNew;
			this->messages = new MessageManager(this->agentResources, this->threads);
			this->channels = new ChannelManager(this->agentResources, this->threads, this->messages);
			this->guildMembers = new GuildMemberManager(this->agentResources, this->threads);
			this->roles = new RoleManager(this->agentResources, this->threads);
			this->users = usersNew;
		}

		task<void> initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
					Channel channel(channelData, this->agentResources, this->channels, this->messages, this->threads);
					this->channels->insertChannel(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
					guildMemberData.guildId = this->data.id;
					GuildMember guildMember(guildMemberData, this->guildMembers);
					this->guildMembers->insertGuildMember(guildMember).get();
				}
				cout << "Caching roles for guild: " << this->data.name << endl;
				for (auto const& [key, value] : data.roles) {
					DiscordCoreInternal::RoleData roleData = value;
					Role role(roleData, this->agentResources, this->roles);
					this->roles->insertRole(role).get();
				}
				cout << "Caching users for guild: " << this->data.name << endl << endl;
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					DiscordCoreInternal::UserData userData = data.members.at(x).user;
					User user(this->threads, this->agentResources, userData, this->users);
					this->users->insertUser(user).get();
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

	class GuildManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class GuildManager;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestFetchBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildData>* requestGetBuffer;
		static unbounded_buffer<Guild>* outBuffer;
		static concurrent_queue<Guild> guildsToInsert;
		static overwrite_buffer<map<string, Guild>> cache;
		
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		DiscordCoreAPI::GuildManager* pGuildManager;
		DiscordCoreAPI::UserManager* pUserManager;

		GuildManagerAgent(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::GuildManager* pGuildManagerNew, DiscordCoreAPI::UserManager* pUserManagerNew, Scheduler* pScheduler)
			:agent(*pScheduler)
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
			if (try_receive(GuildManagerAgent::requestFetchBuffer, dataPackage)) {
				map<string, Guild> cacheTemp;
				if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(dataPackage.guildId)) {
						dataPackage.oldGuildData = cacheTemp.at(dataPackage.guildId).data;
						cacheTemp.erase(dataPackage.guildId);
					}
				}
				Guild guild = getObjectAsync(dataPackage).get();
				cacheTemp.insert(make_pair(dataPackage.guildId, guild));
				send(GuildManagerAgent::outBuffer, guild);
				asend(cache, cacheTemp);
			}
			Guild guildNew;
			while (GuildManagerAgent::guildsToInsert.try_pop(guildNew)) {
				map<string, Guild> cacheTemp;
				if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
				}				
				guildNew.initialize();
				cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
				asend(cache, cacheTemp);
			}
			done();
		}
	};

	class GuildManager {
	public:

		task<Guild> fetchAsync(GetGuildData getGuildData) {
			DiscordCoreInternal::GetGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->threads, dataPackage.agentResources, this, this->pUserManager, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestFetchBuffer, dataPackage);
			guildManagerAgent.start();
			agent::wait(&guildManagerAgent);
			Guild guild; 
			try_receive(GuildManagerAgent::outBuffer, guild);
			co_return guild;
		}

		task<Guild> getGuildAsync(GetGuildData getGuildData) {
			DiscordCoreInternal::GetGuildData dataPackage;
			dataPackage.agentResources = this->agentResources;
			dataPackage.threadContext = this->threads->at(3);
			dataPackage.guildId = getGuildData.guildId;
			GuildManagerAgent guildManagerAgent(this->threads, dataPackage.agentResources, this, this->pUserManager, this->threads->at(2).scheduler);
			send(GuildManagerAgent::requestGetBuffer, dataPackage);
			guildManagerAgent.start();
			agent::wait(&guildManagerAgent);
			Guild guild;
			try_receive(GuildManagerAgent::outBuffer, guild);
			co_return guild;
		}

		task<void> insertGuild(Guild guild) {
			GuildManagerAgent guildManagerAgent(this->threads, this->agentResources, this, this->pUserManager, this->threads->at(2).scheduler);
			guildManagerAgent.start();
			GuildManagerAgent::guildsToInsert.push(guild);
			guildManagerAgent.wait(&guildManagerAgent);
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
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestFetchBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildData>* GuildManagerAgent::requestGetBuffer;
	unbounded_buffer<Guild>* GuildManagerAgent::outBuffer;
	concurrent_queue<Guild> GuildManagerAgent::guildsToInsert;
}
#endif
