// DiscordCoreClient.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_
#define _DISCORD_CORE_CLIENT_

#include "pch.h"
#include "HttpStuff.hpp"
#include "SystemThreads.hpp"
#include "GuildStuff.hpp"

namespace DiscordCoreAPI {

	struct DiscordCoreClient : agent, implements<DiscordCoreClient, winrt::Windows::Foundation::IInspectable> {
	public:

		DiscordCoreClient() {}
		DiscordCoreClient(string botToken) {
			this->botToken = botToken;
			this->pSystemThreads = make_self<DiscordCoreInternal::SystemThreads>();
		}

		DiscordCoreInternal::GuildManager Guilds;

		~DiscordCoreClient() {}

	protected:
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };
		string botToken;

		void run() {}

		void login() {}
	};


}

#endif