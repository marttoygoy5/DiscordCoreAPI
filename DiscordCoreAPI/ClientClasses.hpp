// ClientClasses.hpp - Header file for the "Client classes".
// Apr 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CLIENT_CLASSES_
#define _CLIENT_CLASSES_

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "MainAgent.hpp"
#include "JSONifier.hpp"

namespace CommanderNS {

	namespace ClientClasses {

		struct Client;

		struct ClientAgent;

		struct MainAgent;

		class Reaction {
		public:
			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			Reaction() {};
			Reaction(ClientDataTypes::ReactionData data, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->Data = data;
				this->Client = pClient;
				this->pMainAgent = pMainAgent;
			};
			ClientDataTypes::ReactionData Data;
		};

		class ReactionManager : map<string, Reaction> {
		public:

			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			ReactionManager() {};
			ReactionManager(com_ptr<RestAPI> pRestAPI, string channelId, string messageId, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->channelId = channelId;
				this->messageId = messageId;
				this->Client = pClient;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->pMainAgent = pMainAgent;
			};

			task<void> addReactionAsync(ClientDataTypes::CreateReactionData createReactionData) {
				co_await resume_foreground(*this->pRestAPI->pSystemThreads->Threads.at(2).threadQueue.get());
				string emoji;
				if (createReactionData.id != string()) {
					emoji += ":" + createReactionData.name + ":" + createReactionData.id;
				}
				else {
					emoji = createReactionData.name;
				}
				CURL* curl = curl_easy_init();
				char* output = nullptr;
				if (curl) {
					output = curl_easy_escape(curl, emoji.c_str(), 0);
				}
				string emojiEncoded = output;
				PutEmojiData putEmojiData;
				putEmojiData.channelId = this->channelId;
				putEmojiData.messageId = this->messageId;
				putEmojiData.emoji = emojiEncoded;
				this->pMainAgent->putObjectDataAsync(this->pRestAPI, putEmojiData).get();
				co_return;
			};

			task<void> deleteUserReactionAsync(ClientDataTypes::DeleteReactionData deleteReactionData) {
				string emoji;
				if (deleteReactionData.emojiId != string()) {
					emoji += ":" + deleteReactionData.emojiName + ":" + deleteReactionData.emojiId;
				}
				else {
					emoji = deleteReactionData.emojiName;
				}
				CURL* curl = curl_easy_init();
				char* output = nullptr;
				if (curl) {
					output = curl_easy_escape(curl, emoji.c_str(), 0);
				}
				string emojiEncoded = output;
				DeleteReactionData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				deleteReactionDataNew.userId = deleteReactionData.userId;
				deleteReactionDataNew.encodedEmoji = emojiEncoded;
				this->pMainAgent->deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
				co_return;
			}

			task<void> deleteAllReactionsByEmojiAsync(ClientDataTypes::DeleteAllReactionsByEmojiData deleteReactionData) {
				string emoji;
				if (deleteReactionData.emojiId != string()) {
					emoji += ":" + deleteReactionData.emojiName + ":" + deleteReactionData.emojiId;
				}
				else {
					emoji = deleteReactionData.emojiName;
				}
				CURL* curl = curl_easy_init();
				char* output = nullptr;
				if (curl) {
					output = curl_easy_escape(curl, emoji.c_str(), 0);
				}
				string emojiEncoded = output;
				DeleteAllReactionsByEmojiData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				deleteReactionDataNew.encodedEmoji = emojiEncoded;
				this->pMainAgent->deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
				co_return;
			}

			task<void> deleteOwnReactionAsync(ClientDataTypes::DeleteOwnReactionData deleteReactionData) {
				string emoji;
				if (deleteReactionData.emojiId != string()) {
					emoji += ":" + deleteReactionData.emojiName + ":" + deleteReactionData.emojiId;
				}
				else {
					emoji = deleteReactionData.emojiName;
				}
				CURL* curl = curl_easy_init();
				char* output = nullptr;
				if (curl) {
					output = curl_easy_escape(curl, emoji.c_str(), 0);
				}
				string emojiEncoded = output;
				DeleteOwnReactionData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				deleteReactionDataNew.encodedEmoji = emojiEncoded;
				this->pMainAgent->deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
				co_return;
			}

			task<void> deleteAllReactionsAsync(ClientDataTypes::DeleteAllReactionsData deleteReactionData) {
				DeleteAllReactionsData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				this->pMainAgent->deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
				co_return;
			}

		protected:
			com_ptr<RestAPI> pRestAPI;
			string channelId;
			string messageId;
			string selfUserId;
		};

		class Message {

		public:
			com_ptr<MainAgent> pMainAgent;
			ReactionManager* Reactions;
			ClientDataTypes::MessageData Data;
			com_ptr<Client> Client;

			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient,  string channelId) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->Client = pClient;
				this->selfUserId = selfUserId;
				this->pMainAgent = pMainAgent;
				this->channelId = channelId;
				this->Reactions = new ReactionManager(pRestAPI, this->Data.channelId, this->Data.id, this->selfUserId, this->pMainAgent, this->Client);
			}

			task<void> deleteMessageAsync(int timeDelay = 0) {
				co_await resume_foreground(*this->pRestAPI->pSystemThreads->Threads.at(5).threadQueue.get());
				DeleteMessageData deleteMessageData;
				deleteMessageData.channelId = this->Data.channelId;
				deleteMessageData.messageId = this->Data.id;
				deleteMessageData.timeDelay = timeDelay;
				this->pMainAgent->deleteObjectDataAsync(this->pRestAPI, deleteMessageData).get();
				co_return;
			};

			task<Message> editMessageAsync(ClientDataTypes::EditMessageData editMessageData) {
				co_await resume_foreground(*this->pRestAPI->pSystemThreads->Threads.at(4).threadQueue.get());
				try {
					string createMessagePayload = JSONifier::getEditMessagePayload(editMessageData);
					PostMessageData postMessageData;
					ClientDataTypes::MessageData messageData;
					postMessageData.channelId = this->channelId;
					postMessageData.pDataStructure = &messageData;
					postMessageData.content = createMessagePayload;
					EditMessageData editMessageDataNew;
					editMessageDataNew.channelId = this->channelId;
					editMessageDataNew.messageId = this->Data.id;
					editMessageDataNew.content = createMessagePayload;
					this->pMainAgent->patchObjectDataAsync(this->pRestAPI, editMessageDataNew).get();
					editMessageDataNew.pMessageData = &this->Data;
					Message message(messageData, this->pRestAPI, this->selfUserId,this->pMainAgent, this->Client, this->channelId);
					co_return message;
				}
				catch (exception error) {
					cout << "editMessageAsync() Error: " << error.what() << endl;
				}
			}

		protected:
			string channelId;
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class MessageManager :public map<string, Message> {

		public:
			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->channelId = channelId;
				this->Client = pClient;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->pMainAgent = pMainAgent;
			};

			task<Message*> fetchAsync(string messageId) {
				if (this->contains(messageId)) {
					Message* message = &this->at(messageId);
					this->erase(messageId);
					GetMessageData getMessageData;
					getMessageData.pDataStructure = &message->Data;
					getMessageData.id = messageId;
					getMessageData.channelId = this->channelId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getMessageData).get();
					this->insert(std::make_pair(messageId, *message));
					co_return message;
				}
				else {
					ClientDataTypes::MessageData messageData;
					GetMessageData getMessageData;
					getMessageData.pDataStructure = &messageData;
					getMessageData.id = messageId;
					getMessageData.channelId = this->channelId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getMessageData).get();
					Message message(messageData, this->pRestAPI, this->selfUserId, this->pMainAgent, this->Client, this->channelId);
					this->insert(std::make_pair(messageId, message));
					co_return &this->at(messageId);
				}
			};

			task<Message*> getMessageAsync(string messageId) {
				if (this->contains(messageId)) {
					co_return &this->at(messageId);
				}
				else {
					cout << "getMessageAsync() Error: Sorry, but they aren't here!" << endl;
				}
			}

			task<Message> createMessageAsync(ClientDataTypes::CreateMessageData createMessageData) {
				co_await resume_foreground(*this->pRestAPI->pSystemThreads->Threads.at(3).threadQueue.get());
				try {
					string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
					ClientDataTypes::MessageData messageData;
					PostMessageData postMessageData;
					postMessageData.channelId = this->channelId;
					postMessageData.pDataStructure = &messageData;
					postMessageData.content = createMessagePayload;
					this->pMainAgent->postObjectDataAsync(this->pRestAPI, postMessageData).get();
					Message message(messageData, this->pRestAPI, this->selfUserId, this->pMainAgent, this->Client, this->channelId);
					this->insert(make_pair(messageData.id, message));
					co_return message;
				}
				catch (exception error) {
					cout << "createMessageAsync() Error: " << error.what() << endl;
				}
			};

		protected:
			friend struct WebSocketConnection;
			friend struct WebSocketReceiver;
			friend class Message;
			string guildId;
			string channelId;
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class GuildMember {
		public:
			com_ptr<MainAgent> pMainAgent;
			ClientDataTypes::GuildMemberData Data;
			com_ptr<Client> Client;

			GuildMember() {};
			GuildMember(ClientDataTypes::GuildMemberData data, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->Data = data;
				this->pMainAgent = pMainAgent;
				this->Client = pClient;
			}
		};

		class GuildMemberManager :public map<string, GuildMember> {
		public:
			com_ptr<MainAgent> pMainAgent;
			map<string, ClientDataTypes::GuildMemberData> guildMemberData;
			com_ptr<Client> Client;

			GuildMemberManager() {};
			GuildMemberManager(com_ptr<RestAPI> pRestAPI, string guildId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->pRestAPI = pRestAPI;
				this->pMainAgent = pMainAgent;
				this->Client = pClient;
				this->guildId = guildId;
			}

			task<GuildMember*> fetchAsync(string guildMemberId) {
				if (this->contains(guildMemberId)) {
					GuildMember* guildMember = &this->at(guildMemberId);
					this->erase(guildMemberId);
					GetGuildMemberData getGuildMemberData;
					getGuildMemberData.pDataStructure = &guildMember->Data;
					getGuildMemberData.id = guildMemberId;
					getGuildMemberData.guildId = this->guildId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getGuildMemberData).get();
					this->insert(std::make_pair(guildMemberId, *guildMember));
					co_return guildMember;
				}
				else {
					ClientDataTypes::GuildMemberData guildMemberData;
					GetGuildMemberData getGuildMemberData;
					getGuildMemberData.pDataStructure = &guildMemberData;
					getGuildMemberData.id = guildMemberId;
					getGuildMemberData.guildId = this->guildId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getGuildMemberData).get();
					GuildMember guildMember(guildMemberData,this->pMainAgent, this->Client);
					this->insert(std::make_pair(guildMemberId, guildMember));
					co_return &this->at(guildMemberId);
				}
			};

			task<GuildMember*> getGuildMemberAsync(string guildMemberId) {
					if (this->contains(guildMemberId)) {
						co_return &this->at(guildMemberId);
					}
					else {
						cout << "getGuildMemberAsync() Error: Sorry, but they aren't here!" << endl;
					}
			};

			task<void> getGuildMembersAsync() {
				GetGuildMembersData getGuildMembersData;
				map<string, ClientDataTypes::GuildMemberData> guildMemberDataMap;
				for (auto const& [key, value]: *this) {
					guildMemberDataMap.insert(make_pair(key, value.Data));
				}
				getGuildMembersData.guildId = this->guildId;
				getGuildMembersData.pGuildMemberDataMap = &guildMemberDataMap;
				this->pMainAgent->getObjectDataAsync(this->pRestAPI, getGuildMembersData).get();
				for (auto const& [key, value] : *getGuildMembersData.pGuildMemberDataMap) {
					if (this->contains(key)) {
						this->erase(key);
						GuildMember guildMember(value, this->pMainAgent, this->Client);
						this->insert(make_pair(key, guildMember));
					}
					else {
						GuildMember guildMember(value, this->pMainAgent, this->Client);
						this->insert(make_pair(key, guildMember));
					}
				}
				co_return;
			}
		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			string guildId;
		};

		class Channel {
		public:
			ClientDataTypes::ChannelData Data;
			MessageManager* Messages;
			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->Client = pClient;
				this->pMainAgent = pMainAgent;
				this->Messages = new MessageManager(this->Data.id,this->Data.guildId, this->pRestAPI, this->selfUserId, this->pMainAgent, this->Client);
			};
			
			ClientDataTypes::OverWriteData getPermissionOverwrites(string roleOrUserId) {
				if (this->Data.permissionOverwrites.contains(roleOrUserId)) {
					return this->Data.permissionOverwrites.at(roleOrUserId);
				}
				else {
					ClientDataTypes::OverWriteData overWriteData;
					overWriteData.id = roleOrUserId;
					return overWriteData;
				}
			}

		protected:
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->Client = pClient;
				this->pMainAgent = pMainAgent;
			};

			task<Channel*> fetchAsync(string channelId) {
				if (this->contains(channelId)) {
					Channel* channel = &this->at(channelId);
					this->erase(channelId);
					GetChannelData getChannelData;
					getChannelData.pDataStructure = &channel->Data;
					getChannelData.id = channelId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getChannelData).get();
					this->insert(std::make_pair(channelId, *channel));
					co_return channel;
				}
				else {
					ClientDataTypes::ChannelData channelData;
					GetChannelData getChannelData;
					getChannelData.pDataStructure = &channelData;
					getChannelData.id = channelId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getChannelData).get();
					Channel channel(channelData, this->pRestAPI,this->selfUserId, this->pMainAgent, this->Client);
					this->insert(std::make_pair(channelId, channel));
					co_return &this->at(channelId);
				}
			};

			task<Channel*> getChannelAsync(string channelId) {
				if (this->contains(channelId)) {
					co_return &this->at(channelId);
				}
				else {
					cout << "getChannelAsync() Error: Sorry, but they aren't here!" << endl;
				}
			};

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class Role {
		public:
			com_ptr<MainAgent> pMainAgent{ nullptr };
			com_ptr<Client> Client{ nullptr };
			ClientDataTypes::RoleData Data;

			Role() {};
			Role(ClientDataTypes::RoleData data, com_ptr<MainAgent> pMainAgent, com_ptr<RestAPI> pRestAPI, com_ptr<ClientClasses::Client> pClient) {
				this->pRestAPI = pRestAPI;
				this->pMainAgent = pMainAgent;
				this->Data = data;
				this->Client = pClient;
			}

		protected:
			com_ptr<RestAPI> pRestAPI{ nullptr };
		};

		class RoleManager : public map<string, Role> {
		public:
			com_ptr<MainAgent> pMainAgent;
			com_ptr<ClientClasses::Client> Client;
			RoleManager() {};
			RoleManager(com_ptr<RestAPI> pRestAPI, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient, string guildId) {
				this->pRestAPI = pRestAPI;
				this->pMainAgent = pMainAgent;
				this->guildId = guildId;
				this->Client = pClient;
			}

			task<void> getRolesAsync() {
				GetRolesData getRolesData;;
				map<string, ClientDataTypes::RoleData> pRoleDataMap;
				for (const auto& [key, value] : *this) {
					pRoleDataMap.insert(make_pair(key, value.Data));
				}
				getRolesData.guildId = this->guildId;
				getRolesData.pDataStructure = &pRoleDataMap;
				this->pMainAgent->getObjectDataAsync(this->pRestAPI, getRolesData);
				for (auto const& [key, value] : *getRolesData.pDataStructure) {
					if (this->contains(key)) {
						this->erase(key);
						Role role(value, this->pMainAgent, this->pRestAPI, this->Client);
						this->insert(make_pair(key, role));
					}
					else {
						Role role(value, this->pMainAgent, this->pRestAPI, this->Client);
						this->insert(make_pair(key, role));
					}
				}
				co_return;
			}

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI{ nullptr };
			string guildId;

		};

		class Guild {
		public:
			ClientDataTypes::GuildData Data;
			RoleManager* Roles;
			GuildMemberManager* Members;
			ChannelManager* Channels;
			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;

			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<ClientClasses::Client> pClient) {
				this->Data = data;
				this->selfUserId = selfUserId;
				this->pMainAgent = pMainAgent;
				this->Client = pClient;
				this->Roles = new RoleManager(pRestAPI, this->pMainAgent, this->Client, this->Data.id);
				for (auto const& [key, value] : data.roles) {
					Role role(value, this->pMainAgent, pRestAPI, this->Client);
					this->Roles->insert(make_pair(key, role));
				}
				this->Channels = new ChannelManager(pRestAPI, this->selfUserId, this->pMainAgent, this->Client);
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					Channel channel(data.channels.at(x), pRestAPI, this->selfUserId, this->pMainAgent, this->Client);
					this->Channels->insert(make_pair(data.channels.at(x).id, channel));
				}
				this->Members = new GuildMemberManager(pRestAPI, this->Data.id, this->pMainAgent, this->Client);
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					GuildMember member(data.members.at(x), this->pMainAgent, this->Client);
					this->Members->insert(make_pair(data.members.at(x).user.id, member));
				}
			};

		protected:
			string selfUserId;
		};

		class GuildManager : public map<string, Guild> {

		public:
			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI, string selfUserId, com_ptr<MainAgent> pMainAgent, com_ptr<Client> pClient) {
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->Client = pClient;
				this->pMainAgent = pMainAgent;
			};

			com_ptr<MainAgent> pMainAgent;
			com_ptr<Client> Client;
			task<Guild*> fetchAsync(string guildId) {
				if (this->contains(guildId)) {
					Guild* guild = &this->at(guildId);
					this->erase(guildId);
					GetGuildData getGuildData;
					getGuildData.pDataStructure = &guild->Data;
					getGuildData.id = guildId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getGuildData).get();
					this->insert(std::make_pair(guildId, *guild));
					co_return guild;
				}
				else {
					ClientDataTypes::GuildData guildData;
					GetGuildData getGuildData;
					getGuildData.pDataStructure = &guildData;
					getGuildData.id = guildId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getGuildData).get();
					Guild guild(guildData, this->pRestAPI, this->selfUserId, this->pMainAgent, this->Client);
					this->insert(std::make_pair(guildId, guild));
					co_return &this->at(guildId);
				}
			};

			task<Guild*> getGuildAsync(string guildId) {
				if (this->contains(guildId)) {
					co_return &this->at(guildId);
				}
				else {
					cout << "getGuildAsync() Error: Sorry, but they aren't here!" << endl;
				}
			};

			task<Guild> collectBackupValue(string guildId) {
				co_return this->guilds.at(guildId);
			}

			task<void> getCurrentUserGuilds() {
				GetCurrentUserGuildsData getCurrentUserGuildsData;
				map<string, ClientDataTypes::GuildData> guildDataMap;
				for (auto const& [key, val] : this->guilds) {
					guildDataMap.insert(make_pair(key, val.Data));
				}
				getCurrentUserGuildsData.pGuildDataMap = &guildDataMap;
				this->pMainAgent->getObjectDataAsync(this->pRestAPI, getCurrentUserGuildsData).get();
				for (auto const& [key, val] : guildDataMap) {
					Guild guild(val, this->pRestAPI, this->selfUserId, this->pMainAgent, this->Client);
					if (this->guilds.contains(key)) {
						this->guilds.erase(key);
					}
					this->guilds.insert(make_pair(key, guild));
				}
				co_return;
			};
			
		protected:
			friend struct WebSocketConnection;
			friend struct WebSocketReceiver;
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
			map<string, Guild> guilds;		
			string selfUserId;
		};

		class User {

		public:
			User() {};
			User(ClientDataTypes::UserData data, com_ptr<MainAgent> pMainAgent) {
				this->Data = data;
				this->pMainAgent = pMainAgent;
			};

			string getAvatarURL() {
				string avatarURL;
				avatarURL = "https://cdn.discordapp.com/avatars/" + this->Data.id + "/" + this->Data.avatar;
				return avatarURL;
			}

			ClientDataTypes::UserData Data;
			com_ptr<MainAgent> pMainAgent;
		};

		class UserManager : public map<string, User> {
		public:
			UserManager() {};
			UserManager(com_ptr<RestAPI> pRestAPI, com_ptr<MainAgent> pMainAgent) {
				this->pRestAPI = pRestAPI;
				this->pMainAgent = pMainAgent;
			};

			task<User*> fetchAsync(string userId) {
				if (this->contains(userId)){
					User* user = &this->at(userId);
					this->erase(userId);
					GetUserData getUserData;
					getUserData.pDataStructure = &user->Data;
					getUserData.id = userId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getUserData).get();
					this->insert(std::make_pair(userId, *user));
					co_return user;
				}
				else {
					ClientDataTypes::UserData userData;
					GetUserData getUserData;
					getUserData.pDataStructure = &userData;
					getUserData.id = userId;
					this->pMainAgent->getObjectDataAsync(this->pRestAPI, getUserData).get();
					User user(userData, this->pMainAgent);
					this->insert(std::make_pair(userId, user));
					co_return &this->at(userId);
				}
			};

			task<User*> getUserAsync(string userId) {
					if (this->contains(userId)) {
						co_return &this->at(userId);
					}
					else {
						cout << "getUserAsync() Error: Sorry, but they aren't here!" << endl;
					}
			};

			com_ptr<MainAgent> pMainAgent;

		protected:
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
		};

		struct Client : implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			com_ptr<ClientAgent> pClientAgent{ nullptr };
			com_ptr<MainAgent> pMainAgent{ nullptr };
			GuildManager* Guilds;
			UserManager* Users;
			User User;
			
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				com_ptr<Client> pClient{ nullptr };
				pClient.attach(this);
				this->pRestAPI = pRestAPI;
				this->Users = new UserManager(pRestAPI, this->pMainAgent);
				this->User = User;
				this->getUserSelf();
				this->Guilds = new GuildManager(pRestAPI, this->User.Data.id, this->pMainAgent, pClient);
				this->Guilds->getCurrentUserGuilds().get();
			};

			void initialize(com_ptr<ClientAgent> pClientAgent, com_ptr<MainAgent> pMainAgent) {
				this->pClientAgent = pClientAgent;
				this->pMainAgent = pMainAgent;
			}

			task<void> getUserSelf() {
				apartment_context mainThread;
				co_await resume_foreground(*this->pRestAPI->pSystemThreads->Threads.at(1).threadQueue.get());
				ClientDataTypes::UserData userData;
				GetSelfUserData getUserData;
				getUserData.pDataStructure = &userData;
				this->pMainAgent->getObjectDataAsync(this->pRestAPI, getUserData).get();
				com_ptr<Client> pClient{ nullptr };
				pClient.attach(this);
				ClientClasses::User userNew(userData, this->pMainAgent);
				this->User = userNew;
				co_await mainThread;
				co_return;
			};

		protected:
			friend struct WebSocketReceiver;
			friend class GuildManager;
			friend struct WebSocketConnection;
			com_ptr<RestAPI> pRestAPI{ nullptr };
		};

		struct ClientAgent :public concurrency::agent, implements<ClientAgent, winrt::Windows::Foundation::IInspectable> {
		public:
			unbounded_buffer<bool> source00;
			unbounded_buffer<com_ptr<Client>> target00;

			ClientAgent(Scheduler* pScheduler, com_ptr<Client> pClient)
				:_source00(source00),
				_target00(target00),
				agent(*pScheduler)
			{
				this->pClient = pClient;
			}
			
			bool doWeQuit = false;

		protected:
			friend struct WebSocketReceiver;
			friend struct WebSocketConnection;
			com_ptr<Client> pClient;
			com_ptr<ClientClasses::MainAgent> pMainAgent{ nullptr };
			ISource<bool>& _source00;
			ITarget<com_ptr<ClientClasses::Client>>& _target00;

			void run() {
				receive(_source00);
				com_ptr<Client> pClient = this->pClient;
				send(_target00, pClient);
				done();
				}
				
			};
			
		};

	
};
#endif