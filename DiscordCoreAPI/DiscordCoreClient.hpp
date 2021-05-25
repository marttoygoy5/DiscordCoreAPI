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
#include "SlashCommandStuff.hpp"
#include "WebSocketStuff.hpp"
#include "GuildStuff.hpp"
#include "UserStuff.hpp"
#include "DatabaseStuff.hpp"
#include "EventMachine.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
	return;
}

namespace DiscordCoreAPI {

	class DiscordCoreClient;

	struct CommandData {
		Message message;
		DiscordCoreClient* coreClient;
	};

	class DiscordCoreClient :public DiscordCoreClientBase,  protected agent {
	public:
		User* currentUser{ nullptr };
		GuildManager* guilds{ nullptr };
		ReactionManager* reactions{ nullptr };
		MessageManager* messages{ nullptr };
		SlashCommandManager* slashCommands{ nullptr };
		shared_ptr<DiscordCoreAPI::EventMachine> EventMachine{ nullptr };
		DiscordUser* discordUser{ nullptr };
		static map<string, DiscordGuild> guildMap;
		static map<string, DiscordGuildMember> guildMemberMap;
		DiscordCoreClient(hstring botTokenNew)
			:webSocketWorkloadSource(this->webSocketWorkCollectionBuffer),
			webSocketWorkloadTarget(this->webSocketWorkCollectionBuffer) {
			this->botToken = botTokenNew;
		}

		void login() {
			this->initialize(this->botToken).get();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			return;
		}

		void checkForErrors() {
			exception error;
			if (try_receive(errorBuffer, error)) {
				cout << "DiscordCoreClient::run() Error: " << error.what() << endl << endl;
			}
			return;
		}

		void terminate() {
			this->doWeQuit = true;
			this->pWebSocketReceiverAgent->terminate();
			this->pWebSocketConnectionAgent->terminate();
		}

	protected:

		bool doWeQuit = false;
		hstring botToken;
		hstring baseURL = L"https://discord.com/api/v9";
		DiscordCoreInternal::WebSocketConnectionAgent* pWebSocketConnectionAgent{ nullptr };
		DiscordCoreInternal::WebSocketReceiverAgent* pWebSocketReceiverAgent{ nullptr };
		ISource<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadSource;
		ITarget<DiscordCoreInternal::WebSocketWorkload>& webSocketWorkloadTarget;
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		shared_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };
		single_assignment<exception> errorBuffer;
		
		task<void> initialize(hstring botTokenNew) {
			_set_purecall_handler(myPurecallHandler);
			this->pSystemThreads = make_shared<DiscordCoreInternal::SystemThreads>();
			this->pSystemThreads->initialize().get();
			apartment_context mainThread;
			co_await resume_foreground(*this->pSystemThreads->mainThreadContext.threadQueue.get());
			this->EventMachine = make_shared<DiscordCoreAPI::EventMachine>();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = new DiscordCoreInternal::WebSocketConnectionAgent(this->webSocketIncWorkloadBuffer, this->botToken, this->pSystemThreads->getThreads().get()->at(0));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->pSystemThreads->getThreads().get()->at(3).scheduler);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SOCKET_PATH;
			workload.relativePath = "/gateway/bot";
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			if (requestAgent.getError(error)) {
				cout << "DiscordCoreClient::initialize() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			this->pWebSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->pWebSocketReceiverAgent = new DiscordCoreInternal::WebSocketReceiverAgent(this->webSocketIncWorkloadBuffer, this->webSocketWorkloadTarget, this->pSystemThreads->getThreads().get()->at(1));
			ReactionManagerAgent::initialize();
			UserManagerAgent::initialize();
			MessageManagerAgent::initialize();
			RoleManagerAgent::initialize();
			GuildMemberManagerAgent::initialize();
			ChannelManagerAgent::initialize();
			GuildManagerAgent::initialize();
			this->reactions = new ReactionManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->users = new UserManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->messages = new MessageManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->roles = new RoleManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->guildMembers = new GuildMemberManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->channels = new ChannelManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->guilds = new GuildManager(agentResources, this->pSystemThreads->getThreads().get(), (DiscordCoreClient*)this, this);
			this->currentUser = new User(this->users->fetchCurrentUserAsync().get().data, this);
			this->slashCommands = new SlashCommandManager(agentResources, this->pSystemThreads->getThreads().get(), this->currentUser->data.id);
			DatabaseManager::initialize(this->currentUser->data.id, this->pSystemThreads->getThreads().get()->at(10).scheduler);
			this->discordUser = new DiscordUser(this->currentUser->data.username, this->currentUser->data.id, this->guilds->guildCount);
			co_await mainThread;
		}

		task<OnGuildCreationData> createGuild(DiscordCoreInternal::GuildData guildData) {
			try {
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.baseURL = this->baseURL;
				agentResources.botToken = this->botToken;
				Guild guild(agentResources, this->pSystemThreads->getThreads().get(), guildData, (DiscordCoreClient*)this, this);
				OnGuildCreationData guildCreationData(guild);
				guildCreationData.guild = guild;
				co_return guildCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
		}

		task<OnMessageCreationData> createMessage(DiscordCoreInternal::MessageData messageData) {
			try {
				Message message(messageData, this);
				OnMessageCreationData messageCreationData(message);
				messageCreationData.message = message;
				co_return messageCreationData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}
		}

		task<OnReactionAddData> createReaction(DiscordCoreInternal::ReactionData reactionData) {
			try {
				Reaction reaction(reactionData, this);
				OnReactionAddData reactionAddData(reaction);
				reactionAddData.reaction = reaction;
				co_return reactionAddData;
			}
			catch (exception error) {
				cout << "Error Message: " << error.what() << endl;
			}			
		}

		void run() {
			try {
				while (doWeQuit == false) {
					DiscordCoreInternal::WebSocketWorkload workload;
					if (try_receive(this->webSocketWorkloadSource, workload)) {
						if (workload.eventType == DiscordCoreInternal::WebSocketEventType::GUILD_CREATE) {
							DiscordCoreInternal::GuildData guildData;
							DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
							OnGuildCreationData guildCreationData = createGuild(guildData).get();
							this->EventMachine->onGuildCreationEvent(guildCreationData);
						}
						if (workload.eventType == DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE) {
							DiscordCoreInternal::MessageData messageData;
							DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
							OnMessageCreationData messageCreationData = createMessage(messageData).get();
							this->EventMachine->onMessageCreationEvent(messageCreationData);
						}
						if (workload.eventType == DiscordCoreInternal::WebSocketEventType::REACTION_ADD) {
							DiscordCoreInternal::ReactionData reactionData;
							DiscordCoreInternal::parseObject(workload.payLoad, &reactionData);
							OnReactionAddData reactionAddData = createReaction(reactionData).get();
							this->EventMachine->onReactionAddEvent(reactionAddData);
						}
					}
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};
}
#endif