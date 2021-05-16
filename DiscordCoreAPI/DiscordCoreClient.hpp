// DiscordCoreClient.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_
#define _DISCORD_CORE_CLIENT_

#include "pch.h"
#include "SystemThreads.hpp"
#include "HttpStuff.hpp"
#include "WebSocketStuff.hpp"
#include "GuildStuff.hpp"
#include "EventMachine.hpp"

namespace DiscordCoreAPI {

	struct DiscordCoreClient : agent, implements<DiscordCoreClient, winrt::Windows::Foundation::IInspectable> {
	public:

		com_ptr<GuildManager> guilds{ nullptr };
		com_ptr<EventMachine> EventMachine{ nullptr };
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };

		DiscordCoreClient(hstring botTokenNew)
			:webSocketWorkloadSource(this->webSocketWorkCollectionBuffer),
			webSocketWorkloadTarget(this->webSocketWorkCollectionBuffer)
		{
			this->botToken = botTokenNew;
		}

		void terminate() {
			this->doWeQuit = true;
			this->pWebSocketConnectionAgent->terminate();
			this->pWebSocketReceiverAgent->terminate();
			this->terminate();
		}

		task<void> login() {
			this->initialize(this->botToken);
			this->pWebSocketConnectionAgent->start();
			this->pWebSocketReceiverAgent->start();
			this->start();
			co_return;
		}

		~DiscordCoreClient() {

		}

	protected:
		friend class GuildManager;
		friend class Guild;
		friend class GuildMemberManager;
		friend class GuildMember;
		friend class ChannelManager;
		friend class Channel;
		hstring botToken;
		bool doWeQuit = false;
		hstring baseURL = L"https://discord.com/api/v9";
		com_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		com_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;

		task<void> initialize(hstring botTokenNew) {
			this->pSystemThreads = make_self<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			this->EventMachine = make_self<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = make_self<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->pSystemThreads->Threads.at(1));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			agentResources.pSocketPath = pWebSocketConnectionAgent->returnSocketPathPointer();
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->pSystemThreads->Threads.at(0).scheduler);
			this->pWebSocketReceiverAgent = make_self<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->Threads.at(2));
			this->pWebSocketConnectionAgent->initialize(botTokenNew);
			this->guilds = make_self<GuildManager>(this->pSystemThreads->getThreads().get(), agentResources);
			GuildManagerAgent::initialize().get();
			ChannelManagerAgent::initialize().get();
		}

		void run() {
			while (doWeQuit == false) {
				DiscordCoreInternal::WebSocketWorkload workload;
				if (try_receive(this->webSocketWorkloadSource, workload)) {
					if (workload.eventType == DiscordCoreInternal::WebSocketEventType::GUILD_CREATE) {
						this->guilds->insertGuild(workload.payLoad);
					}
					if (workload.eventType == DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE) {
						DiscordCoreInternal::MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						DiscordCoreInternal::HttpAgentPointers pointers;
						DiscordCoreAPI::MessageCreationData messageCreationData;
						cout << "WE ARE HERE WE ARE HERE WE ARE HERE" << endl;
						DiscordCoreAPI::Guild guild = this->guilds->fetchAsync(messageData.guildId).get();
						//auto messageManager = fetchChannelAsync(messageData.channelId, guild).get().messages;
						//messageCreationData.message = Message(messageData, &guild, pointers, messageManager);
						cout << "WE ARE HERE WE ARE HERE WE ARE HERE" << endl;
						
						this->EventMachine->onMessageCreationEvent(messageCreationData);
					}
				}
			}
			done();
		}
	};
}

#endif