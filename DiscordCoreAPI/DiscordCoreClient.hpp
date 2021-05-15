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
			this->pDELETEAgent->terminate();
			this->pPATCHAgent->terminate();
			this->pPOSTAgent->terminate();
			this->pPUTAgent->terminate();
			this->pGETAgent->terminate();
			this->terminate();
		}

		task<void> login() {
			this->initialize(this->botToken);
			co_await resume_foreground(*this->pSystemThreads->mainThreadContext.threadQueue.get());
			this->pGETAgent->start();
			this->pPUTAgent->start();
			this->pPOSTAgent->start();
			this->pPATCHAgent->start();
			this->pDELETEAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->pWebSocketReceiverAgent->start();
			this->start();
			co_return;
		}

		~DiscordCoreClient() {

		}

	protected:
		unbounded_buffer<DiscordCoreInternal::HttpWorkload> GETworkSubmissionBuffer;
		unbounded_buffer<DiscordCoreInternal::HttpWorkload> PUTworkSubmissionBuffer;
		unbounded_buffer<DiscordCoreInternal::HttpWorkload> POSTworkSubmissionBuffer;
		unbounded_buffer<DiscordCoreInternal::HttpWorkload> PATCHworkSubmissionBuffer;
		unbounded_buffer<DiscordCoreInternal::HttpWorkload> DELETEworkSubmissionBuffer;

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
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pGETAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPUTAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPOSTAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pPATCHAgent{ nullptr };
		com_ptr<DiscordCoreInternal::HttpRequestAgent> pDELETEAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;

		task<void> initialize(hstring botTokenNew) {
			this->pSystemThreads = make_self<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			co_await resume_foreground(*this->pSystemThreads->mainThreadContext.threadQueue.get());
			this->EventMachine = make_self<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = make_self<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->pSystemThreads->Threads.at(1));
			this->pWebSocketReceiverAgent = make_self<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->Threads.at(2));
			this->pGETAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(3).scheduler);
			this->pPUTAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(4).scheduler);
			this->pPOSTAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(5).scheduler);
			this->pPATCHAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(6).scheduler);
			this->pDELETEAgent = make_self<DiscordCoreInternal::HttpRequestAgent>(this->botToken, this->baseURL, this->pWebSocketConnectionAgent->returnSocketPathPointer(), this->pSystemThreads->Threads.at(7).scheduler);
			this->pWebSocketConnectionAgent->initialize(botTokenNew);
			DiscordCoreInternal::HttpAgentPointers pointers;
			pointers.pGETAgent.attach(this->pGETAgent.get());
			pointers.pPUTAgent.attach(this->pPUTAgent.get());
			pointers.pPOSTAgent.attach(this->pPOSTAgent.get());
			pointers.pPATCHAgent.attach(this->pPATCHAgent.get());
			pointers.pDELETEAgent.attach(this->pDELETEAgent.get());
			this->guilds = make_self<GuildManager>(pointers, this->pSystemThreads->Threads.at(8).scheduler, this->pSystemThreads);
			GuildManagerAgent::initialize().get();
			ChannelManagerAgent::initialize().get();
		}

		task<Channel> fetchChannelAsync(string channelId, Guild guild) {
			co_await resume_foreground(*this->pSystemThreads->Threads.at(0).threadQueue.get());
			co_return guild.channels->fetchAsync(channelId).get();
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
						pointers.pGETAgent.attach(this->pGETAgent.get());
						pointers.pPUTAgent.attach(this->pPUTAgent.get());
						pointers.pPATCHAgent.attach(this->pPATCHAgent.get());
						pointers.pPOSTAgent.attach(this->pPOSTAgent.get());
						pointers.pDELETEAgent.attach(this->pDELETEAgent.get());
						cout << "WE ARE HERE WE ARE HERE WE ARE HERE" << endl;
						Guild guild = this->guilds->fetchAsync(messageData.guildId).get();
						auto messageManager = fetchChannelAsync(messageData.channelId, guild).get().messages;
						messageCreationData.message = Message(messageData, &guild, pointers, messageManager);
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