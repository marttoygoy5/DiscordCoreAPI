// DiscordCoreClient.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_
#define _DISCORD_CORE_CLIENT_

#include "pch.h"
#include "ThreadManager.hpp"
#include "HttpStuff.hpp"
#include "WebSocketStuff.hpp"
#include "GuildStuff.hpp"
#include "UserStuff.hpp"
#include "InteractionManager.hpp"
#include "EventManager.hpp"
#include "SlashCommandStuff.hpp"
#include "DatabaseStuff.hpp"
#include "InputEventHandler.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
	return;
}

namespace DiscordCoreAPI {

	static string commandPrefix;

	struct CommandData {
		InputEventData eventData;
	};

	class DiscordCoreClient :public DiscordCoreClientBase,  protected agent {
	public:
		User* currentUser{ nullptr };
		GuildManager* guilds{ nullptr };
		ReactionManager* reactions{ nullptr };
		MessageManager* messages{ nullptr };
		SlashCommandManager* slashCommands{ nullptr };
		EventManager* eventManager{ nullptr };
		DiscordCoreInternal::ThreadManager* threadManager{ nullptr };
		DiscordUser* discordUser{ nullptr };
		DiscordCoreClient(hstring botTokenNew) {
			this->botToken = botTokenNew;
		}

		DiscordGuild getDiscordGuild(GuildData guildData) {
			auto guildCursor = DiscordCoreClient::guildMap.find(guildData.id);
			if (guildCursor == DiscordCoreClient::guildMap.end()) {
				DiscordGuild discordGuild(guildData);
				DiscordCoreClient::guildMap.insert(make_pair(guildData.id, discordGuild));
				discordGuild.getDataFromDB().get();
				return discordGuild;
			}
			else {
				(*guildCursor).second.getDataFromDB().get();
				return (*guildCursor).second;
			}
		}

		DiscordGuildMember getDiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
			auto guildMemberCursor = DiscordCoreClient::guildMemberMap.find(guildMemberData.guildId + guildMemberData.user.id);
			if (guildMemberCursor == DiscordCoreClient::guildMemberMap.end()){
				DiscordGuildMember discordGuildMember(guildMemberData);
				DiscordCoreClient::guildMemberMap.insert(make_pair(guildMemberData.guildId + guildMemberData.user.id, discordGuildMember));
				discordGuildMember.getDataFromDB().get();
				return discordGuildMember;
			}
			else {
				(*guildMemberCursor).second.getDataFromDB().get();
				return (*guildMemberCursor).second;
			}
		}

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> finalSetup(string botToken);

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		void terminate() {
			this->doWeQuit = true;
			this->pWebSocketReceiverAgent->terminate();
			agent::wait(this->pWebSocketReceiverAgent);
			exception error;
			while (this->pWebSocketReceiverAgent->getError(error)) {
				cout << "DiscordCoreClient::terminate() Error 01: " << error.what() << endl << endl;
			}
			this->pWebSocketConnectionAgent->terminate();
			agent::wait(this->pWebSocketConnectionAgent);
			while (this->pWebSocketReceiverAgent->getError(error)) {
				cout << "DiscordCoreClient::terminate() Error 02: " << error.what() << endl << endl;
			}
		}

	protected:
		bool doWeQuit = false;
		hstring botToken;
		hstring baseURL = L"https://discord.com/api/v9";
		DiscordCoreInternal::WebSocketConnectionAgent* pWebSocketConnectionAgent{ nullptr };
		DiscordCoreInternal::WebSocketReceiverAgent* pWebSocketReceiverAgent{ nullptr };
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		unbounded_buffer<exception> errorBuffer;
		
		void login() {
			this->initialize(this->botToken).get();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			return;
		}

		task<void> initialize(hstring botTokenNew) {
			_set_purecall_handler(myPurecallHandler);
			this->threadManager = new DiscordCoreInternal::ThreadManager();
			this->threadManager->initialize().get();
			apartment_context mainThread;
			co_await resume_foreground(*this->threadManager->mainThreadContext.threadQueue.get());
			this->eventManager = new DiscordCoreAPI::EventManager();
			this->botToken = botTokenNew;
			this->pWebSocketConnectionAgent = new DiscordCoreInternal::WebSocketConnectionAgent(this->webSocketIncWorkloadBuffer, this->botToken, this->threadManager->getThreads().get()->at(0));
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources, this->threadManager->getThreads().get()->at(3).scheduler);
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
			this->pWebSocketReceiverAgent = new DiscordCoreInternal::WebSocketReceiverAgent(this->webSocketIncWorkloadBuffer, this->webSocketWorkCollectionBuffer, this->threadManager->getThreads().get()->at(1));
			ReactionManagerAgent::initialize();
			UserManagerAgent::initialize();
			MessageManagerAgent::initialize();
			RoleManagerAgent::initialize();
			GuildMemberManagerAgent::initialize();
			GuildManagerAgent::initialize();
			ChannelManagerAgent::initialize();
			InteractionManager::initialize(agentResources, this->threadManager->getThreads().get());
			InteractionManagerAgent::initialize(this->threadManager->getThreads().get());
			this->reactions = new ReactionManager(agentResources, this->threadManager->getThreads().get(), this);
			this->users = new UserManager(agentResources, this->threadManager->getThreads().get(), this);
			this->messages = new MessageManager(agentResources, this->threadManager->getThreads().get(), this);
			this->roles = new RoleManager(agentResources, this->threadManager->getThreads().get(), this);
			this->guildMembers = new GuildMemberManager(agentResources, this->threadManager->getThreads().get(), this);
			this->channels = new ChannelManager(agentResources, this->threadManager->getThreads().get(), this);
			this->guilds = new GuildManager(agentResources, this->threadManager->getThreads().get(), (DiscordCoreClient*)this, this);
			this->currentUser = new User(this->users->fetchCurrentUserAsync().get().data, this);
			this->slashCommands = new SlashCommandManager(agentResources, this->threadManager->getThreads().get(), this->currentUser->data.id);
			DatabaseManagerAgent::initialize(this->currentUser->data.id, this->threadManager->getThreads().get()->at(10).scheduler);
			Button::initialize(this);
			this->discordUser = new DiscordUser(this->currentUser->data.username, this->currentUser->data.id);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			InputEventHandler::initialize(this->messages, this, agentResources, this->threadManager->getThreads().get()->at(9));
			this->discordUser->writeDataToDB();
			co_await mainThread;
		}

		task<Guild> createGuild(GuildData guildData) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			agentResources.botToken = this->botToken;
			Guild guild(agentResources, this->threadManager->getThreads().get(), guildData, (DiscordCoreClient*)this, this);
			DiscordGuild discordGuild(guild.data);
			discordGuild.writeDataToDB();
			if (DiscordCoreClient::guildMap.contains(guild.data.id)) {
				DiscordCoreClient::guildMap.erase(guild.data.id);
			}
			else {
				this->discordUser->data.guildCount += 1;
				this->discordUser->writeDataToDB();
			}
			DiscordCoreClient::guildMap.insert(make_pair(guild.data.id, discordGuild));
			co_return guild;
		}

		task<OnMessageCreationData> createMessage(MessageData messageData) {
			Message message(messageData, this);
			OnMessageCreationData messageCreationData(message);
			messageCreationData.message = message;
			co_return messageCreationData;
		}

		void run() {
			try {
				while (doWeQuit == false) {
					DiscordCoreInternal::WebSocketWorkload workload = receive(this->webSocketWorkCollectionBuffer, INFINITE);
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_CREATE:
					{
						ChannelData channelData;
						DiscordCoreInternal::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData, this);
						OnChannelCreationData channelCreationData;
						channelCreationData.channel = channel;
						this->eventManager->onChannelCreationEvent(channelCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_UPDATE:
					{
						OnChannelUpdateData channelUpdateData;
						Channel channel = this->channels->getChannelAsync({ .channelId = workload.payLoad.at("id") }).get();
						channelUpdateData.channelOld = channel;
						DiscordCoreInternal::parseObject(workload.payLoad, &channel.data);
						channelUpdateData.channelNew = channel;
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_DELETE:
					{
						ChannelData channelData;
						DiscordCoreInternal::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData, this);
						OnChannelDeletionData channelDeleteData;
						channelDeleteData.channel = channel;
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_CREATE:
					{
						GuildData guildData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
						Guild guild = createGuild(guildData).get();
						DiscordCoreAPI::OnGuildCreationData guildCreationData;
						guildCreationData.guild = guild;
						this->eventManager->onGuildCreationEvent(guildCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_UPDATE:
					{
						DiscordCoreAPI::OnGuildUpdateData guildUpdateData;
						Guild guild = this->guilds->getGuildAsync({ .guildId = workload.payLoad.at("id") }).get();
						guildUpdateData.guildOld = guild;
						DiscordCoreInternal::parseObject(workload.payLoad, &guild.data);
						guildUpdateData.guildNew = guild;
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_DELETE:
					{
						GuildData guildData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
						Guild guild = createGuild(guildData).get();
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData;
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_ADD:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, this);
						DiscordCoreAPI::OnGuildBanAddData guildBanAddData;
						guildBanAddData.user = user;
						guildBanAddData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanAddEvent(guildBanAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, this);
						DiscordCoreAPI::OnGuildBanRemoveData guildBanRemoveData;
						guildBanRemoveData.user = user;
						guildBanRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanRemoveEvent(guildBanRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_ADD:
					{
						GuildMemberData guildMemberData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMember(guildMemberData, this);
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData;
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, this);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData;
						guildMemberRemoveData.user = user;
						guildMemberRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberRemoveEvent(guildMemberRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_UPDATE:
					{
						DiscordCoreAPI::OnGuildMemberUpdateData guildMemberUpdateData;
						GuildMember guildMemberOld = this->guildMembers->getGuildMemberAsync({ .guildId = workload.payLoad.at("guild_id"), .guildMemberId = workload.payLoad.at("user").at("id") }).get();
						guildMemberUpdateData.guildMemberOld = guildMemberOld;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildMemberOld.data);
						guildMemberUpdateData.guildMemberNew = guildMemberOld;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_CREATE:
					{
						RoleData roleData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("role"), &roleData);
						Role role(roleData, this);
						DiscordCoreAPI::OnRoleCreationData roleCreationData;
						roleCreationData.role = role;
						roleCreationData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onRoleCreationEvent(roleCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_UPDATE:
					{
						DiscordCoreAPI::OnRoleUpdateData roleUpdateData;
						Role role = this->roles->getRoleAsync({ .guildId = workload.payLoad.at("guild_id"), .roleId = workload.payLoad.at("role").at("id") }).get();
						roleUpdateData.roleOld = role;
						DiscordCoreInternal::parseObject(workload.payLoad.at("role"), &role.data);
						roleUpdateData.roleNew = role;
						roleUpdateData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onRoleUpdateEvent(roleUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_DELETE:
					{
						DiscordCoreAPI::OnRoleDeletionData roleDeletionData;
						roleDeletionData.guildId = workload.payLoad.at("guild_id");
						roleDeletionData.roleOld = this->roles->getRoleAsync({ .guildId = roleDeletionData.guildId, .roleId = workload.payLoad.at("role_id") }).get();
						this->eventManager->onRoleDeletionEvent(roleDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INVITE_CREATE:
					{
						InviteData inviteData;
						DiscordCoreInternal::parseObject(workload.payLoad, &inviteData);
						OnInviteCreationData inviteCreationData;
						inviteCreationData.invite = inviteData;
						this->eventManager->onInviteCreationEvent(inviteCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INVITE_DELETE:
					{
						string channelId = workload.payLoad.at("channel_id");
						string guildId = workload.payLoad.at("guild_id");
						string code = workload.payLoad.at("code");
						OnInviteDeletionData inviteDeletionData;
						inviteDeletionData.channelId = channelId;
						inviteDeletionData.guildId = guildId;
						inviteDeletionData.code = code;
						this->eventManager->onInviteDeletionEvent(inviteDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE:
					{
						MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						Message message(messageData, this);
						OnMessageCreationData messageCreationData;
						messageCreationData.message = message;
						this->eventManager->onMessageCreationEvent(messageCreationData);
						InputEventData eventData;
						eventData.eventType = InputEventType::REGULAR_MESSAGE;
						eventData.messageData = messageData;
						eventData.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
						eventData.requesterId = messageData.author.id;
						eventData.discordCoreClient = this;
						OnInputEventCreationData eventCreationData;
						eventCreationData.eventData = eventData;
						this->eventManager->onInputEventCreationEvent(eventCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_UPDATE:
					{
						DiscordCoreAPI::OnMessageUpdateData messageUpdateData;
						Message message = this->messages->getMessageAsync({ .channelId = workload.payLoad.at("channel_id"), .id = workload.payLoad.at("id") }).get();
						messageUpdateData.messageOld = message;
						DiscordCoreInternal::parseObject(workload.payLoad, &message.data);
						messageUpdateData.messageNew = message;
						this->eventManager->onMessageUpdateEvent(messageUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_DELETE:
					{
						OnMessageDeletionData messageDeletionData;
						messageDeletionData.messageId = workload.payLoad.at("id");
						messageDeletionData.channelId = workload.payLoad.at("channel_id");
						messageDeletionData.guildId = workload.payLoad.at("guild_id");
						messageDeletionData.discordCoreClient = this;
						this->eventManager->onMessageDeletionEvent(messageDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_DELETE_BULK:
					{
						OnMessageDeleteBulkData messageDeleteBulkData;
						messageDeleteBulkData.channelId = workload.payLoad.at("channel_id");
						messageDeleteBulkData.guildId = workload.payLoad.at("guild_id");
						for (auto value : workload.payLoad.at("ids")) {
							messageDeleteBulkData.ids.push_back(value);
						}
						messageDeleteBulkData.discordCoreClient = this;
						this->eventManager->onMessageDeleteBulkEvent(messageDeleteBulkData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_ADD:
					{
						ReactionAddData reactionAddDataNew;
						DiscordCoreInternal::parseObject(workload.payLoad, &reactionAddDataNew);
						reactionAddDataNew.discordCoreClient = this;
						OnReactionAddData reactionAddData;
						reactionAddData.reactionAddData = reactionAddDataNew;
						this->eventManager->onReactionAddEvent(reactionAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_REMOVE:
					{
						ReactionRemoveData reactionRemoveDataNew;
						DiscordCoreInternal::parseObject(workload.payLoad, &reactionRemoveDataNew);
						reactionRemoveDataNew.discordCoreClient = this;
						OnReactionRemoveData reactionRemoveData;
						reactionRemoveData.reactionRemoveData = reactionRemoveDataNew;
						this->eventManager->onReactionRemoveEvent(reactionRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INTERACTION_CREATE:
					{
						InteractionData interactionData;
						CommandData commandData;
						DiscordCoreInternal::parseObject(workload.payLoad, &interactionData);
						InputEventData eventData;
						if (interactionData.type == InteractionType::ApplicationCommand) {
							eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
							eventData.inputEventResponseType = InputEventResponseType::UNSET;
							eventData.interactionData = interactionData;
							eventData.discordCoreClient = this;
							eventData.requesterId = interactionData.member.user.id;
							OnInputEventCreationData eventCreationData;
							eventCreationData.eventData = eventData;
							this->eventManager->onInputEventCreationEvent(eventCreationData);
						}
						else if (interactionData.type == InteractionType::MessageComponent) {
							eventData.eventType = InputEventType::BUTTON_INTERACTION;
							eventData.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
							eventData.interactionData = interactionData;
							eventData.discordCoreClient = this;
							eventData.requesterId = interactionData.member.user.id;
							OnInputEventCreationData eventCreationData;
							eventCreationData.eventData = eventData;
							this->eventManager->onInputEventCreationEvent(eventCreationData);
						}
						break;
					}
					default:
					{
						break;
					}
					}
				}
			}
			catch (const operation_timed_out& e) {
				done();
				cout << e.what() << endl;
				start();
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
			this->terminate();
		}
	};
	map<string, DiscordGuild> DiscordCoreClientBase::guildMap;
	map<string, DiscordGuildMember> DiscordCoreClientBase::guildMemberMap;
}
#include "Commands/CommandsList.hpp"
#endif
