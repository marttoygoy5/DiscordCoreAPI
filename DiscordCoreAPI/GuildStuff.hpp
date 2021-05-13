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

namespace DiscordCoreAPI
{
	struct DiscordCoreClient;

	class Guild {
	public:
		string guildId;
		ChannelManager channelManager{ nullptr };
		GuildMemberManager guildMemberManager{ nullptr };
		Guild() {}
		Guild(DiscordCoreInternal::GuildData data) {
			this->data = data;
			this->channelManager = ChannelManager(this->guildId);
			for (unsigned int x = 0; x < data.channels.size(); x+=1) {
				DiscordCoreInternal::ChannelData channelData = data.channels.at(x);
				Channel channel(channelData);
				this->channelManager.channels.insert(make_pair(channelData.id, channel));
			}
			this->guildMemberManager = GuildMemberManager(this->guildId);
			for (unsigned int x = 0; x < data.members.size(); x += 1) {
				DiscordCoreInternal::GuildMemberData guildMemberData = data.members.at(x);
				GuildMember guildMember(guildMemberData, this->guildId);
				this->guildMemberManager.guildMembers.insert(make_pair(guildMemberData.user.id, guildMember));
			}
		}

		~Guild() {}

	protected:
		DiscordCoreInternal::GuildData data;

	};

	class GuildManager {
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
			co_return guild;
		}

		task<Guild> getGuildAsync(string guildId) {
			try {
				Guild guild = this->guilds.at(guildId);
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
		concurrent_unordered_map<string, Guild> guilds;
	};

}
#endif
