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
#include "UserStuff.hpp"
//#include "DMChannelStuff.hpp"
#include "EventMachine.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
	return;
}

namespace DiscordCoreAPI {

	class DiscordCoreClient;

	struct CommandData {
		Message message;
		shared_ptr<DiscordCoreClient> pClient;
	};

	class DiscordCoreClient : public agent {
	public:
		User* currentUser{ nullptr };
		UserManager* users{ nullptr };
		GuildManager* guilds{ nullptr };
		ChannelManager* channels{ nullptr };
		MessageManager* messages{ nullptr };
		RoleManager* roles{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		ReactionManager* reactions{ nullptr };
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
		}

		task<void> login() {
			this->initialize(this->botToken).get();
			this->pWebSocketConnectionAgent->initialize(this->botToken);
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
		DiscordCoreInternal::WebSocketConnectionAgent* pWebSocketConnectionAgent{ nullptr };
		DiscordCoreInternal::WebSocketReceiverAgent* pWebSocketReceiverAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		
		task<void> initialize(hstring botTokenNew) {
			_set_purecall_handler(myPurecallHandler);
			this->pSystemThreads = make_shared<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			apartment_context mainThread;
			co_await resume_foreground(*this->pSystemThreads->mainThreadContext.threadQueue.get());
			this->EventMachine = make_shared<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = new DiscordCoreInternal::WebSocketConnectionAgent(this->webSocketIncWorkloadBuffer, this->pSystemThreads->getThreads().get()->at(0));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			agentResources.pSocketPath = pWebSocketConnectionAgent->returnSocketPathPointer();
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->pSystemThreads->getThreads().get()->at(3).scheduler);
			this->pWebSocketReceiverAgent = new DiscordCoreInternal::WebSocketReceiverAgent(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->getThreads().get()->at(1));
			GuildManagerAgent::initialize().get();
			GuildMemberManagerAgent::initialize();
			ChannelManagerAgent::initialize().get();
			MessageManagerAgent::initialize().get();
			ReactionManagerAgent::initialize().get();
			RoleManagerAgent::initialize().get();
			UserManagerAgent::initialize().get();

			this->reactions = new ReactionManager(agentResources, this->pSystemThreads->threads);
			this->users = new UserManager(agentResources, this->pSystemThreads->threads);
			this->messages = new MessageManager(agentResources, this->pSystemThreads->threads, this->reactions);
			this->roles = new RoleManager(agentResources, this->pSystemThreads->threads);
			this->guildMembers = new GuildMemberManager(agentResources, this->pSystemThreads->threads);
			this->channels = new ChannelManager(agentResources, this->pSystemThreads->threads);
			this->guilds = new GuildManager(agentResources, this->pSystemThreads->threads, this->guildMembers, this->roles, this->channels, this->users);
			User user = this->users->fetchCurrentUser().get();
			this->currentUser = &user;
			co_await mainThread;
		}

		task<DiscordCoreAPI::GuildCreationData> createGuild(DiscordCoreInternal::GuildData guildData) {
			try {
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.baseURL = this->baseURL;
				agentResources.botToken = this->botToken;
				agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
				Guild guild(agentResources, this->pSystemThreads->getThreads().get(), guildData, this->guilds, this->users, this->guildMembers, this->roles, this->channels);
				GuildCreationData guildCreationData(guild);
				co_return guildCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
		}

		task<DiscordCoreAPI::MessageCreationData> createMessage(DiscordCoreInternal::MessageData messageData) {
			try {
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.botToken = this->botToken;
				agentResources.baseURL = this->baseURL;
				agentResources.pSocketPath = this->pWebSocketConnectionAgent->returnSocketPathPointer();
				MessageManager* msgManager = this->messages;
				Message message(messageData, msgManager, this->reactions);
				MessageCreationData messageCreationData(message);
				co_return messageCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
		}

		task<DiscordCoreAPI::ReactionAddData> createReaction(DiscordCoreInternal::ReactionData reactionData) {
			try {
				Reaction reaction(reactionData, this->reactions);
				ReactionAddData reactionAddData(reaction);
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