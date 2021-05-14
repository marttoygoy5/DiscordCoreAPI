// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "pch.h"
#include "DataParsingFunctions.hpp"
#include "ChannelStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "RoleStuff.hpp"

namespace DiscordCoreAPI
{

	class GuildManager;

	class Guild : public DiscordCoreInternal::GuildData{
	public:
		string guildId;
		ChannelManager channels{ nullptr };
		GuildMemberManager guildMembers{ nullptr };
		RoleManager roles{ nullptr };
		GuildManager guilds;
		Guild(DiscordCoreInternal::GuildData data) {
			*this = data;
			this->channels = ChannelManager(this->guildId);						


			for (unsigned int x = 0; x < data.channels.size(); x+=1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData);
				this->channels.insert(make_pair(channelData.id, channel));
			}
			this->guildMembers = GuildMemberManager(this->guildId);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData);
				this->guildMembers.insert(make_pair(guildMemberData.user.id, guildMember));
			}
			
			
		}

		~Guild() {}

	protected:
		friend class GuildManager;
		DiscordCoreInternal::HttpAgentPointers pointers;
		void init(DiscordCoreInternal::HttpAgentPointers pointers, GuildManager guilds) {
			this->pointers = pointers;
			this->guilds = guilds;
		}
		
	};

	class GuildManager: concurrent_unordered_map<string, Guild> {
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
			json jsonValue = receive(&pointers.pGETAgent->workReturnBuffer);
			DiscordCoreInternal::GuildData guildData;
			Guild guild(guildData);
			try {
				guild = this->at(guildId);
				this->unsafe_erase(guildId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &guild);
			guild.init(this->pointers, *this);
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
				Guild guild(guildData);
				co_return guild;
			}
		}
		~GuildManager() {}
	protected:
		DiscordCoreInternal::HttpAgentPointers pointers;
	};

}
#endif