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

	class GuildMember {
	public:
		string guildMemberId, guildId;
		GuildMember() {}
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, string guildIdNew) {
			this->guildMemberId = guildMemberData.user.id;
			this->guildId = guildIdNew;
			this->data = guildMemberData;
		}

	protected:
		DiscordCoreInternal::GuildMemberData data;

	};

	class GuildMemberManager{
	public:
		string guildId;
		GuildMemberManager() {}
		GuildMemberManager(string guildIdNew) {
			this->guildId = guildIdNew;
		}

		task<GuildMember> fetchAsync(string guildMemberID) {
			DiscordCoreInternal::GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, this->guildId);
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(string guildMemberID) {
			try {
				GuildMember guildMember = this->guildMembers.at(guildMemberID);
				co_return guildMember;
			}
			catch (exception error) {
				cout << "getGuildMemberAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::GuildMemberData guildMemberData;
				GuildMember guildMember(guildMemberData, this->guildId);
				co_return guildMember;
			}
		}
		~GuildMemberManager() {}

	protected:
		friend class Guild;
		concurrent_unordered_map<string, GuildMember> guildMembers;
	};
}
#endif
