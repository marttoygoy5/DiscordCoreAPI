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
		Guild(DiscordCoreInternal::GuildData data, DiscordCoreInternal::HttpAgentPointers pointersNew, com_ptr<GuildManager> guildsNew) {
			this->data = data;
			this->pointers = pointersNew;
			this->guilds = guildsNew;
			this->channels = make_self<ChannelManager>(this, this->pointers);
			for (unsigned int x = 0; x < data.channels.size(); x += 1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData, this->pointers, this->channels, this);
				this->channels->insert(make_pair(channelData.id, channel));
			}
			this->guildMembers = make_self<GuildMemberManager>(this, this->pointers);
			for (unsigned int x = 0; x < data.members.size(); x += 1){
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this, this->guildMembers);
				this->guildMembers->insert(make_pair(guildMemberData.user.id, guildMember));
			}
			this->roles = make_self<RoleManager>(this, this->pointers);
			for (auto const& [key,  value]: data.roles) {
				DiscordCoreInternal::RoleData roleData = value;
				Role role(roleData, this->pointers, this->roles, this);
				this->roles->insert(make_pair(key, role));
			}
		}

		~Guild() {}

	protected:
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};

	class GuildManager : concurrent_unordered_map<string, Guild>, public implements <GuildManager, winrt::Windows::Foundation::IInspectable> {
	public:

		GuildManager() {}
		GuildManager(DiscordCoreInternal::HttpAgentPointers pointers){
			this->pointers = pointers;
		}

		task<Guild> fetchAsync(string guildId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + guildId;
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			/*
			DiscordCoreInternal::GuildData guildData;
			try {
				guildData = this->at(guildId).data;
				this->unsafe_erase(guildId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}

			DiscordCoreInternal::parseObject(jsonValue, &guildData);
			*/
			DiscordCoreInternal::GuildData guildData;
			com_ptr<GuildManager> pGuildManager = make_self<GuildManager>(this->pointers);
			pGuildManager->swap(*this);
			Guild guild = Guild(guildData, this->pointers, pGuildManager);
			this->insert(make_pair(guildId, guild));
			co_return guild;
		}

		task<Guild> getGuildAsync(string guildId) {
			try {
				Guild guild = this->at(guildId);
				co_return guild;
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::GuildData guildData;
				com_ptr<GuildManager> pGuildManager = make_self<GuildManager>(this->pointers);
				pGuildManager->swap(*this);
				Guild guild(guildData, this->pointers, pGuildManager);
				co_return guild;
			}
		}

		~GuildManager() {}

	protected:

		friend struct DiscordCoreClient;

		DiscordCoreInternal::HttpAgentPointers pointers;

		task<void> insertGuild(json payload) {
			DiscordCoreInternal::GuildData guildData;
			try {
				guildData = this->at(payload.at("id")).data;
				this->unsafe_erase(payload.at("id"));
			}
			catch(exception error) {
				cout << "insertGuild() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(payload, &guildData);
			com_ptr<GuildManager> pGuildManager = make_self<GuildManager>(this->pointers);
			pGuildManager->swap(*this);
			Guild guild(guildData, this->pointers, pGuildManager);
			this->insert(make_pair(guild.data.id, guild));
			co_return;
		}
	};

}
#endif