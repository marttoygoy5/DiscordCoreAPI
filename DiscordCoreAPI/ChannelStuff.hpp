// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "pch.h"
#include "MessageStuff.hpp"

namespace DiscordCoreAPI {
	class Channel {
	public:
		string guildId, channelId;
		Channel() {}
		Channel(DiscordCoreInternal::ChannelData channelData) {
			this->data = channelData;
			this->guildId = this->data.guildId;
			this->channelId = this->data.id;
		}
	protected:
		DiscordCoreInternal::ChannelData data;
	};

	class ChannelManager {
	public:
		string guildId;
		ChannelManager() {}
		ChannelManager(string guildIdNew) {
			this->guildId = guildIdNew;
		}

		task<Channel> fetchAsync(string channelId) {
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData);
			co_return channel;
		}

		task<Channel> getChannelAsync(string channelId) {
			try {
				Channel channel = this->channels.at(channelId);
				co_return channel;
			}
			catch (exception error) {
				cout << "getChannelAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::ChannelData channelData;
				Channel channel(channelData);
				co_return channel;
			}
		}
		~ChannelManager() {}

	protected:
		friend class Guild;
		concurrent_unordered_map<string, Channel> channels;
	};
}
#endif
