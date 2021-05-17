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

	class DiscordCoreClient : public agent {
	public:

		GuildManager* guilds{ nullptr };
		shared_ptr<EventMachine> EventMachine{ nullptr };
		shared_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };
		DiscordCoreClient(hstring botTokenNew)
			:webSocketWorkloadSource(this->webSocketWorkCollectionBuffer),
			webSocketWorkloadTarget(this->webSocketWorkCollectionBuffer) {
			this->botToken = botTokenNew;
		}

		void terminate() {
			this->doWeQuit = true;
			this->pWebSocketReceiverAgent->terminate();
			this->pWebSocketConnectionAgent->terminate();
			this->terminate();
		}

		task<void> login() {
			this->initialize(this->botToken).get();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			co_return;
		}

	protected:
		friend class GuildManager;
		friend class Guild;
		friend class GuildMemberManager;
		friend class GuildMember;
		friend class ChannelManager;
		friend class Channel;
		bool doWeQuit = false;
		hstring botToken;
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
			co_await resume_foreground(*this->pSystemThreads->getThreads().get()->at(0).threadQueue.get());
			this->EventMachine = make_shared<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->pSystemThreads->getThreads().get()->at(1));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			agentResources.pSocketPath = pWebSocketConnectionAgent->returnSocketPathPointer();
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->pSystemThreads->getThreads().get()->at(3).scheduler);
			this->pWebSocketReceiverAgent = make_shared<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->getThreads().get()->at(2));
			this->pWebSocketConnectionAgent->initialize(botTokenNew);
			this->guilds = new GuildManager(this->pSystemThreads->getThreads().get(), agentResources);
			GuildManagerAgent::initialize().get();
			ChannelManagerAgent::initialize().get();
			MessageManagerAgent::initialize().get();
			ReactionManagerAgent::initialize().get();
		}

		task<DiscordCoreAPI::GuildCreationData>createGuild(DiscordCoreInternal::GuildData guildData) {
			try {
				apartment_context mainThread;
				co_await resume_foreground(*this->pSystemThreads->threads->at(9).threadQueue.get());
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.baseURL = this->baseURL;
				agentResources.botToken = this->botToken;
				agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
				GuildCreationData guildCreationData;
				guildCreationData.guild = Guild(guildData, agentResources, this->guilds, this->pSystemThreads.get()->getThreads().get());
				co_await mainThread;
				co_return guildCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
		}

		task<DiscordCoreAPI::MessageCreationData> createMessage(DiscordCoreInternal::MessageData messageData) {
			try {
				apartment_context mainThread;
				co_await resume_foreground(*this->pSystemThreads->threads->at(9).threadQueue.get());
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.botToken = this->botToken;
				agentResources.baseURL = this->baseURL;
				agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
				Guild guild = this->guilds->getGuildAsync(messageData.guildId).get();
				MessageManager* msgManager = guild.channels->getChannelAsync(messageData.channelId).get().messages;
				Message message(messageData, &guild, agentResources, msgManager, this->pSystemThreads.get()->getThreads().get());
				MessageCreationData messageCreationData;
				messageCreationData.message = message;
				co_await mainThread;
				co_return messageCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
			
		}

		task<DiscordCoreAPI::ReactionAddData> createReaction(DiscordCoreInternal::ReactionData reactionData) {
			try {
				apartment_context mainThread;
				co_await resume_foreground(*this->pSystemThreads->threads->at(9).threadQueue.get());
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.baseURL = this->baseURL;
				agentResources.botToken = this->botToken;
				agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
				ReactionAddData reactionAddData;
				Guild guild = this->guilds->getGuildAsync(reactionData.guildId).get();
				Reaction reaction(agentResources, reactionData, &guild);
				reactionAddData.reaction = reaction;
				co_await mainThread;
				co_return reactionAddData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}			
		}

		void run() {
			while (doWeQuit == false) {
				DiscordCoreInternal::WebSocketWorkload workload;
				if (try_receive(this->webSocketWorkloadSource, workload)) {
					if (workload.eventType == DiscordCoreInternal::WebSocketEventType::GUILD_CREATE) {
						DiscordCoreInternal::GuildData guildData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
						GuildCreationData guildCreationData = createGuild(guildData).get();
						this->EventMachine->onGuildCreationEvent(guildCreationData);
					}
					if (workload.eventType == DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE) {
						DiscordCoreInternal::MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						MessageCreationData messageCreationData = createMessage(messageData).get();
						this->EventMachine->onMessageCreationEvent(messageCreationData);
					}
					if (workload.eventType == DiscordCoreInternal::WebSocketEventType::REACTION_ADD) {
						DiscordCoreInternal::ReactionData reactionData;
						DiscordCoreInternal::parseObject(workload.payLoad, &reactionData);
						ReactionAddData reactionAddData = createReaction(reactionData).get();
						this->EventMachine->onReactionAddEvent(reactionAddData);
					}
				}
			}
			done();
		}
	};
}
#endif
