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

	class GuildMemberManager {
	public:
		Guild* guild{ nullptr };
		GuildMemberManager() {}
		GuildMemberManager(Guild* guildNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew) {
			this->guild = guildNew;
			this->agentResources = agentResourcesNew;
		}

		task<GuildMember> fetchAsync(string guildId, string guildMemberId) {
			GuildMember guildMember;
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(string guildMemberID) {
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->guild, this);
			co_return guildMember;
		}

		~GuildMemberManager() {}

	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
		friend class Guild;
	};
};
#endif
