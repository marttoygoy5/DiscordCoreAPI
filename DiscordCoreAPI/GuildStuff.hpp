// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"

namespace DiscordCoreAPI
{
	class Guild {
	public:
		string guildId;

		Guild() {};
		Guild(DiscordCoreInternal::GuildData data) {
			this->data = data;
		}

		~Guild() {}

	protected:
		DiscordCoreInternal::GuildData data;

	};

	class GuildManager {
	public:
		GuildManager(){}

		task<Guild> fetchAsync(string guildId) {

		}

		task<Guild> getGuildAsync(string guildId) {
			try {
				Guild guild = this->Guilds.at(guildId);
				co_return guild;
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				Guild guild;
				co_return guild;
			}
		}
		~GuildManager() {}
	protected:
		concurrent_unordered_map<string, Guild> Guilds;

	};

}
#endif
