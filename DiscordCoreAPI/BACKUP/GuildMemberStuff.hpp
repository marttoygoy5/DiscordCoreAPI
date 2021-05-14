// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "pch.h"

namespace DiscordCoreAPI {

	class GuildMember : public DiscordCoreInternal::GuildMemberData{
	public:
		string guildMemberId, guildId;
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
			*this = guildMemberData;
			this->guildMemberId = guildMemberData.user.id;
			this->guildId = guildMemberData.guildId;
		}

	protected:

	};

	class GuildMemberManager: concurrent_unordered_map<string, GuildMember>{
	public:
		string guildId;
		GuildMemberManager() {}
		GuildMemberManager(string guildIdNew) {
			this->guildId = guildIdNew;
		}

		task<GuildMember> fetchAsync(string guildMemberID) {
			DiscordCoreInternal::GuildMemberData guildMemberData;
			guildMemberData.guildId = this->guildId;
			GuildMember guildMember(guildMemberData);
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(string guildMemberID) {
			try {
				GuildMember guildMember = this->at(guildMemberID);
				co_return guildMember;
			}
			catch (exception error) {
				cout << "getGuildMemberAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::GuildMemberData guildMemberData;
				GuildMember guildMember(guildMemberData);
				co_return guildMember;
			}
		}
		~GuildMemberManager() {}

	protected:
		friend class Guild;
	};
}
#endif
