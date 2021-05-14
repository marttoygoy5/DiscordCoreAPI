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
		com_ptr<MessageManager> messages{ nullptr };
		com_ptr<ChannelManager> channels{ nullptr };
		Guild* guild{ nullptr };
		Channel(DiscordCoreInternal::ChannelData channelData, DiscordCoreInternal::HttpAgentPointers pointersNew, com_ptr<ChannelManager> channelManager, Guild* guildNew) {
			data = channelData;
			this->pointers = pointersNew;
			this->channels = channelManager;
			this->guild = guildNew;
			this->messages = make_self<MessageManager>(this->pointers, this->guild);
		}
	protected:
		friend class ChannelManager;
		friend class Guild;
		DiscordCoreInternal::HttpAgentPointers pointers;
	};

	class ChannelManager : concurrent_unordered_map<string, Channel>, public implements<ChannelManager, winrt::Windows::Foundation::IInspectable> {
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
			try {
				channelData = this->at(channelId).data;
				this->unsafe_erase(channelId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &channelData);
			com_ptr<ChannelManager> pChannelManager;
			pChannelManager.attach(this);
			Channel channel(channelData, this->pointers, pChannelManager, this->guild);
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
				com_ptr<ChannelManager> pChannelManager;
				pChannelManager.attach(this);
				Channel channel(channelData, this->pointers, pChannelManager, this->guild);
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
