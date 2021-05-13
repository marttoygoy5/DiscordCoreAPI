// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "pch.h"

namespace DiscordCoreInternal
{
	class Guild {
	public:


	protected:


	};

	class GuildManager {
	public:
		task<Guild> fetchAsync(string guildId) {

		}

		task<Guild> getGuildAsync(string guildId) {
			try {
				co_return this->Guilds.at(guildId);
			}
			catch (exception error) {
				cout << "getGuildAsync() Error: " << error.what() << endl;
				Guild guild;
				co_return guild;
			}
		}

	protected:
		concurrent_unordered_map<string, Guild> Guilds;

	};

}
#endif
