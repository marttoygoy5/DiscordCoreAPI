// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"
#include "MessageStuff.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	class ChannelManager;

	class Guild;

	class Channel {
	public:
		Channel() {}
		DiscordCoreInternal::ChannelData data;
		shared_ptr<ChannelManager> channels{ nullptr };
		Guild* guild{ nullptr };
		Channel(DiscordCoreInternal::ChannelData channelData, DiscordCoreInternal::HttpAgentPointers pointersNew, shared_ptr<ChannelManager> channelManager, Guild* guildNew) {
			data = channelData;
			this->pointers = pointersNew;
			this->channels = channelManager;
			this->guild = guildNew;
		}
	protected:
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};

	class ChannelManager: map<string, Channel> {
	public:
		Guild* guild{ nullptr };
		ChannelManager(){}
		ChannelManager(Guild* guild, DiscordCoreInternal::HttpAgentPointers pointers){
			this->guild = guild;
			this->pointers = pointers;
		}

		task<Channel> fetchAsync(string channelId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + channelId;
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			DiscordCoreInternal::ChannelData channelData;
			Channel channel(channelData, this->pointers, make_shared<ChannelManager>(this->guild, this->pointers), this->guild);
			try {
				channel = this->at(channelId);
				this->erase(channelId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &channel.data);
			this->insert(make_pair(channelId, channel));
			co_return channel;
		}

		task<Channel> getChannelAsync(string channelId) {
			try {
				Channel channel = this->at(channelId);
				co_return channel;
			}
			catch (exception error) {
				cout << "getChannelAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::ChannelData channelData;
				Channel channel(channelData, this->pointers, make_shared<ChannelManager>(this->guild, this->pointers), this->guild);
				co_return channel;
			}
		}
		~ChannelManager() {}

	protected:
		friend class Guild;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};
}
#endif
