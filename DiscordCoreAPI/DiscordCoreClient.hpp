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

		com_ptr<DiscordCoreAPI::EventMachine> Events{ nullptr };

		DiscordCoreClient(hstring botToken)
			:webSocketWorkloadSource(this->webSocketWorkCollectionBuffer),
			webSocketWorkloadTarget(this->webSocketWorkCollectionBuffer)
		{
			this->pSystemThreads = make_self<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			this->Events = make_self<DiscordCoreAPI::EventMachine>();
			this->botToken = botToken;
			this->pWebSocketConnectionAgent = make_self<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->pSystemThreads->Threads.at(1));
			this->pWebSocketReceiverAgent = make_self<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->Threads.at(2));
			this->pGETAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(3).scheduler);
			this->pGETAgent->start();
			this->pPUTAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(4).scheduler);
			this->pPUTAgent->start();
			this->pPOSTAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(5).scheduler);
			this->pPOSTAgent->start();
			this->pPATCHAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(6).scheduler);
			this->pPATCHAgent->start();
			this->pDELETEAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(7).scheduler);
			this->pDELETEAgent->start();
			this->pWebSocketConnectionAgent->initialize(botToken);
			this->pWebSocketConnectionAgent->start();
			this->pWebSocketReceiverAgent->start();
			DiscordCoreInternal::HttpAgentPointers pointers;
			pointers.pGETAgent.attach(this->pGETAgent.get());
			pointers.pPUTAgent.attach(this->pPUTAgent.get());
			pointers.pPOSTAgent.attach(this->pPOSTAgent.get());
			pointers.pPATCHAgent.attach(this->pPATCHAgent.get());
			pointers.pDELETEAgent.attach(this->pDELETEAgent.get());
			this->guilds = make_self<GuildManager>(pointers);
		}

		com_ptr<GuildManager> guilds{ nullptr };

		void terminate() {
			this->doWeQuit = true;
		}

		void login() {
			this->start();
		}

		~DiscordCoreClient() {
			this->pWebSocketConnectionAgent->terminate();
			this->pWebSocketReceiverAgent->terminate();
			this->pDELETEAgent->terminate();
			this->pPATCHAgent->terminate();
			this->pPOSTAgent->terminate();
			this->pPUTAgent->terminate();
			this->pGETAgent->terminate();
			this->terminate();
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
		com_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };
		com_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		com_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pGETAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPUTAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPOSTAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPATCHAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pDELETEAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;

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
						pointers.pGETAgent.attach(this->pGETAgent.get());
						pointers.pPUTAgent.attach(this->pPUTAgent.get());
						pointers.pPOSTAgent.attach(this->pPOSTAgent.get());
						pointers.pPATCHAgent.attach(this->pPATCHAgent.get());
						pointers.pDELETEAgent.attach(this->pDELETEAgent.get());
						//this->guilds->fetchAsync(messageData.guildId).get();
						auto guild = this->guilds->fetchAsync(messageData.guildId).get(); //this->guilds->getGuildAsync(messageData.guildId).get();
						com_ptr<MessageManager> messageManager = guild.channels->fetchAsync(messageData.channelId).get().messages;
						messageCreationData.message = Message(messageData, &guild, pointers, messageManager);
						this->Events->onMessageCreationEvent(messageCreationData);
					}
				}
			}
			done();
		}
	};
}

#endif