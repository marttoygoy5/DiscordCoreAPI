// WebSocketAgent.hpp - Header for the Websocket agent class.
// May 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WEBSOCKET_AGENT_
#define _WEBSOCKET_AGENT_

#include "pch.h"
#include "EventMachine.hpp"
#include "DataParsingFunctions.hpp"
#include "DataManipFunctions.hpp"
#include "SystemThreads.hpp"
#include "ClientClasses.hpp"

namespace CommanderNS {

	struct WebSocketAgent : public  concurrency::agent, implements<WebSocketAgent, winrt::Windows::Foundation::IInspectable>{
	public:

		WebSocketAgent(ISource<hstring>& source, Scheduler* pScheduler)
			:_source(source),
			agent(*pScheduler)
		{}

		~WebSocketAgent() {};

		void run() {
			while (1 == 1) {
				hstring payloadString;
				if (try_receive(_source, payloadString)) {
					json jsonValue = jsonValue.parse(payloadString);
					onMessageReceived(jsonValue);
				}				
			}
		};

	protected:
		ISource<hstring>& _source;
		friend struct DiscordCoreAPI;
		com_ptr<SystemThreads> pSystemThreads;
		com_ptr<EventMachine> pEventMachine;
		com_ptr<RestAPI> pRestAPI;
		com_ptr<ClientClasses::Client> pClient;
		shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;

		void initialize(hstring botTokenNew, winrt::com_ptr<EventMachine> pEventMachineNew, com_ptr<SystemThreads> pSystemThreadsNew, com_ptr<RestAPI> pRestAPINew, com_ptr<ClientClasses::Client> pClientNew, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
			this->pSystemThreads = pSystemThreadsNew;
			this->pClient = pClientNew;
			this->pRestAPI = pRestAPINew;
			this->pEventMachine = pEventMachineNew;
			this->pHttpHandler = pHttpHandler;
		}

		fire_and_forget onGuildCreate(json payload) {
			CommanderNS::ClientDataTypes::GuildData guildData;
			string id = payload.at("d").at("id");
			CommanderNS::DataParsingFunctions::parseObject(payload.at("d"), &guildData);
			ClientClasses::Guild guild(guildData, this->pRestAPI, this->pSystemThreads, this->pHttpHandler);
			this->pClient->Guilds.insert(std::make_pair(id, guild));
			for (unsigned int y = 0; y < guild.Data.members.size(); y += 1) {
				ClientClasses::User user(guild.Data.members.at(y).user);
				this->pClient->Users.insert(make_pair(user.Data.id, user));
			}
			co_return;
		}
		
		task<void> onMessageCreate(json payload) {
			EventDataTypes::MessageCreationData messageCreationData;
			ClientDataTypes::MessageData messageData;
			DataParsingFunctions::parseObject(payload.at("d"), &messageData);
			string guildId = payload.at("d").at("guild_id");
			string channelId = payload.at("d").at("channel_id");
			auto tempPtr = this->pClient->Guilds.getGuildAsync(messageData.guildId).get().Channels.getChannelAsync(messageData.channelId).get().messageManager;
			ClientClasses::MessageManager* pMessageManager = tempPtr;
			messageCreationData.message = ClientClasses::Message(messageData, this->pRestAPI, this->pClient->Guilds.getGuildAsync(guildId).get().Channels.getChannelAsync(channelId).get().messageManager,
				this->pHttpHandler, this->pSystemThreads); (messageData, this->pRestAPI, pMessageManager);
			messageCreationData.threadContext = &this->pSystemThreads->Threads.at(1);
			this->pEventMachine->onMessageCreationEvent(messageCreationData);
			co_return;
		}

		task<void> onMessageDelete(json payload){
			EventDataTypes::MessageDeletionData messageDeletionData;
			ClientDataTypes::MessageData messageData;
			DataParsingFunctions::parseObject(payload.at("d"), &messageData);
			string guildId = payload.at("d").at("guild_id");
			string channelId = payload.at("d").at("channel_id");
			ClientClasses::Message message(messageData, this->pRestAPI, this->pClient->Guilds.getGuildAsync(guildId).get().Channels.getChannelAsync(channelId).get().messageManager,
				this->pHttpHandler, this->pSystemThreads);
			this->pClient->Guilds.getGuildAsync(guildId).get().Channels.getChannelAsync(channelId).get().messageManager->erase(messageData.id);
			messageDeletionData.message = message;
			messageDeletionData.threadContext = &this->pSystemThreads->Threads.at(1);
			this->pEventMachine->onMessageDeletionEvent(messageDeletionData);
			co_return;
		}

		task<void> onMessageReactionAdd(json payload) {
			ClientDataTypes::ReactionAddEventData reactionAddEventData;
			DataParsingFunctions::parseObject(payload.at("d"), &reactionAddEventData);
			ClientDataTypes::ReactionData reactionData;
			EventDataTypes::ReactionAddData reactionAddData;
			reactionData.channelId = reactionAddEventData.channelId;
			reactionData.emoji = reactionAddEventData.emoji;
			reactionData.guildId = reactionAddEventData.guildId;
			reactionData.member = reactionAddEventData.member;
			reactionData.messageId = reactionAddEventData.messageId;
			reactionData.userId = reactionAddEventData.userId;
			ClientClasses::Reaction reaction(reactionData);
			reactionAddData.reaction = reaction;
			reactionAddData.threadContext = &this->pSystemThreads->Threads.at(1);
			this->pEventMachine->onReactionAddEvent(reactionAddData);
			co_return;
		}

		void onMessageReceived(json payload) {
			try {

				if (payload.at("t") == "PRESENCE_UPDATE") {
					return;
				}

				if (payload.at("t") == "MESSAGE_CREATE") {
					onMessageCreate(payload);
				}

				if (payload.at("t") == "MESSAGE_DELETE") {
					onMessageDelete(payload);
				}

				if (payload.at("t") == "GUILD_CREATE") {
					onGuildCreate(payload);
					return;
				}

				if (payload.at("t") == "MESSAGE_REACTION_ADD") {
					onMessageReactionAdd(payload);
				}

				if (payload.at("t") == "GUILD_MEMBER_ADD") {
					ClientDataTypes::GuildMemberData guildMemberData;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->pClient->Guilds.guildGetRateLimit, payload.at("d").at("guild_id"), payload.at("d").at("user").at("id"), &guildMemberData).get();
					EventDataTypes::GuildMemberAddData guildMemberAddData;
					guildMemberAddData.guildId = payload.at("d").at("guild_id");
					guildMemberAddData.guildMember = ClientClasses::GuildMember(guildMemberData);
					guildMemberAddData.threadContext = &this->pSystemThreads->Threads.at(1);
					this->pEventMachine->onGuildMemberAddEvent(guildMemberAddData);
				}
			}
			catch (winrt::hresult_error const& ex) {
				std::wcout << ex.message().c_str() << std::endl;

			};
		};

	};
}
#endif