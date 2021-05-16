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

		shared_ptr<GuildManager> guilds{ nullptr };
		shared_ptr<EventMachine> EventMachine{ nullptr };
		shared_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };

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
			this->initialize(this->botToken).get();;
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
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;

		task<void> initialize(hstring botTokenNew) {
			this->pSystemThreads = make_shared<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			co_await resume_foreground(*this->pSystemThreads->pThreads->at(0).threadQueue.get());
			this->EventMachine = make_shared<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->pSystemThreads->getThreads().get()->at(1));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			agentResources.pSocketPath = pWebSocketConnectionAgent->returnSocketPathPointer();
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->pSystemThreads->pThreads->at(3).scheduler);
			this->pWebSocketReceiverAgent = make_shared<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->getThreads().get()->at(2));
			this->pWebSocketConnectionAgent->initialize(botTokenNew);
			this->guilds = make_shared<GuildManager>(this->pSystemThreads->getThreads().get(), agentResources);
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
						DiscordCoreInternal::HttpAgentResources agentResources;
						agentResources.baseURL = this->baseURL;
						agentResources.botToken = this->botToken;
						agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
						DiscordCoreAPI::MessageCreationData messageCreationData;
						Guild guild = this->guilds->fetchAsync(messageData.guildId).get();
						cout << guild.data.name << endl;
						cout << "CHANNEL ID: " << messageData.channelId << endl;
						cout << "CHANNEL NAME: " <<guild.channels->getChannelAsync(messageData.channelId).get().data.name << endl;
						DiscordCoreAPI::Message message(messageData, &guild, agentResources, guild.channels->fetchAsync(messageData.channelId).get().messages);
						messageCreationData.message = Message();
						cout << "25252525WERE HERE WERE HERE" << endl;
						this->EventMachine->onMessageCreationEvent(messageCreationData);
					}
				}
			}
			done();
		}
	};
}
#endif
