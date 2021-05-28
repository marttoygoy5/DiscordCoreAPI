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
		DiscordCoreClient(hstring botTokenNew) {
			this->botToken = botTokenNew;
		}

		DiscordGuild getDiscordGuild(DiscordCoreInternal::GuildData guildData) {
			auto guildCursor = DiscordCoreClient::guildMap.find(guildData.id);
			if (guildCursor == DiscordCoreClient::guildMap.end()) {
				DiscordGuild discordGuild(guildData);
				DiscordCoreClient::guildMap.insert(make_pair(guildData.id, discordGuild));
				return discordGuild;
			}
			else {
				return (*guildCursor).second;
			}
		}

		DiscordGuildMember getDiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
			auto guildMemberCursor = DiscordCoreClient::guildMemberMap.find(guildMemberData.guildId + guildMemberData.user.id);
			if (guildMemberCursor == DiscordCoreClient::guildMemberMap.end()){
				DiscordGuildMember discordGuildMember(guildMemberData);
				DiscordCoreClient::guildMemberMap.insert(make_pair(guildMemberData.guildId + guildMemberData.user.id, discordGuildMember));
				return discordGuildMember;
			}
			else {
				return (*guildMemberCursor).second;
			}
		}

		void login() {
			this->initialize(this->botToken).get();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		void terminate() {
			this->doWeQuit = true;
			this->pWebSocketReceiverAgent->terminate();
		}

	protected:

		bool doWeQuit = false;
		hstring botToken;
		hstring baseURL = L"https://discord.com/api/v9";
		DiscordCoreInternal::WebSocketConnectionAgent* pWebSocketConnectionAgent{ nullptr };
		DiscordCoreInternal::WebSocketReceiverAgent* pWebSocketReceiverAgent{ nullptr };
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		shared_ptr<DiscordCoreInternal::SystemThreads> pSystemThreads{ nullptr };
		unbounded_buffer<exception> errorBuffer;
		
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
			while (requestAgent.getError(error)) {
				cout << "DiscordCoreClient::initialize() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			this->pWebSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->pWebSocketReceiverAgent = new DiscordCoreInternal::WebSocketReceiverAgent(this->webSocketIncWorkloadBuffer, this->webSocketWorkCollectionBuffer, this->pSystemThreads->getThreads().get()->at(1));
			ReactionManagerAgent::initialize();
			UserManagerAgent::initialize();
			MessageManagerAgent::initialize();
			RoleManagerAgent::initialize();
			GuildMemberManagerAgent::initialize();
			GuildManagerAgent::initialize();
			ChannelManagerAgent::initialize();
			this->reactions = new ReactionManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->users = new UserManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->messages = new MessageManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->roles = new RoleManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->guildMembers = new GuildMemberManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->channels = new ChannelManager(agentResources, this->pSystemThreads->getThreads().get(), this);
			this->guilds = new GuildManager(agentResources, this->pSystemThreads->getThreads().get(), (DiscordCoreClient*)this, this);
			this->currentUser = new User(this->users->fetchCurrentUserAsync().get().data, this);
			this->slashCommands = new SlashCommandManager(agentResources, this->pSystemThreads->getThreads().get(), this->currentUser->data.id);
			DatabaseManagerAgent::initialize(this->currentUser->data.id, this->pSystemThreads->getThreads().get()->at(10).scheduler);
			this->discordUser = new DiscordUser(this->currentUser->data.username, this->currentUser->data.id);
			this->discordUser->writeDataToDB();
			co_await mainThread;
		}

		task<OnGuildCreationData> createGuild(DiscordCoreInternal::GuildData guildData) {
			try {
				DiscordCoreInternal::HttpAgentResources agentResources;
				agentResources.baseURL = this->baseURL;
				agentResources.botToken = this->botToken;
				Guild guild(agentResources, this->pSystemThreads->getThreads().get(), guildData, (DiscordCoreClient*)this, this);
				DiscordGuild discordGuild(guild.data);
				discordGuild.writeDataToDB();
				DiscordCoreClient::guildMap.insert(make_pair(guild.data.id, discordGuild));
				OnGuildCreationData guildCreationData(guild);
				guildCreationData.guild = guild;
				this->discordUser->data.guildCount += 1;
				this->discordUser->writeDataToDB();
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
					if (try_receive(this->webSocketWorkCollectionBuffer, workload)) {
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
	map<string, DiscordGuild> DiscordCoreClientBase::guildMap;
	map<string, DiscordGuildMember> DiscordCoreClientBase::guildMemberMap;
}
#endif