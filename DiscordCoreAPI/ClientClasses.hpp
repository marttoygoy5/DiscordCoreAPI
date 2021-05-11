// ClientClasses.hpp - Header file for the "Client classes".
// Apr 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CLIENT_CLASSES_
#define _CLIENT_CLASSES_

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "DataManipFunctions.hpp"
#include "JSONifier.hpp"

namespace CommanderNS {

	namespace ClientClasses {

		class Reaction {
		public:
			Reaction() {};
			Reaction(ClientDataTypes::ReactionData data) {
				this->Data = data;
			};
			ClientDataTypes::ReactionData Data;
		};

		class ReactionManager : map<string, Reaction> {
		public:

			ReactionManager() {};
			ReactionManager(com_ptr<RestAPI> pRestAPI, string channelId, string messageId, string selfUserId) {
				this->channelId = channelId;
				this->messageId = messageId;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
			};

			task<void> addReactionAsync(ClientDataTypes::CreateReactionData createReactionData) {
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
				DataManipFunctions::PutEmojiData putEmojiData;
				putEmojiData.channelId = this->channelId;
				putEmojiData.messageId = this->messageId;
				putEmojiData.emoji = emojiEncoded;
				DataManipFunctions::putObjectDataAsync(this->pRestAPI, putEmojiData).get();
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
				DataManipFunctions::DeleteReactionData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				deleteReactionDataNew.userId = deleteReactionData.userId;
				deleteReactionDataNew.encodedEmoji = emojiEncoded;
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
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
				DataManipFunctions::DeleteOwnReactionData deleteReactionDataNew;
				deleteReactionDataNew.channelId = this->channelId;
				deleteReactionDataNew.messageId = this->messageId;
				deleteReactionDataNew.encodedEmoji = emojiEncoded;
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, deleteReactionDataNew).get();
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
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->Reactions = new ReactionManager(pRestAPI, this->Data.channelId, this->Data.id, this->selfUserId);
			}

			task<void> deleteMessageAsync(int timeDelay = 0) {
				DataManipFunctions::DeleteMessageData deleteMessageData;
				deleteMessageData.channelId = this->Data.channelId;
				deleteMessageData.messageId = this->Data.id;
				deleteMessageData.timeDelay = timeDelay;
				co_await DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, deleteMessageData);
				co_return;
			};

			ReactionManager* Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class MessageManager :public map<string, Message> {

		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
			};

			task<Message*> fetchAsync(string messageId) {
				if (this->contains(messageId)) {
					this->erase(messageId);
					Message* message = &this->at(messageId);
					DataManipFunctions::GetMessageData getMessageData;
					getMessageData.pDataStructure = &message->Data;
					getMessageData.id = messageId;
					getMessageData.channelId = this->channelId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getMessageData).get();
					this->insert(std::make_pair(messageId, *message));
					co_return message;
				}
				else {
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::GetMessageData getMessageData;
					getMessageData.pDataStructure = &messageData;
					getMessageData.id = messageId;
					getMessageData.channelId = this->channelId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getMessageData).get();
					Message message(messageData, this->pRestAPI, this->selfUserId);
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
				try {
					string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::PostMessageData postMessageData;
					postMessageData.channelId = this->channelId;
					postMessageData.pDataStructure = &messageData;
					postMessageData.content = createMessagePayload;
					DataManipFunctions::postObjectDataAsync(this->pRestAPI, postMessageData).get();
					Message message(messageData, this->pRestAPI, this->selfUserId);
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
			GuildMember() {};
			GuildMember(ClientDataTypes::GuildMemberData data) {
				this->Data = data;
			}
			ClientDataTypes::GuildMemberData Data;
		};

		class GuildMemberManager :public map<string, GuildMember> {
		public:
			GuildMemberManager() {};
			GuildMemberManager(com_ptr<RestAPI> pRestAPI, string guildId) {
				this->pRestAPI = pRestAPI;
				this->guildId = guildId;
			}

			task<GuildMember*> fetchAsync(string guildMemberId) {
				if (this->contains(guildMemberId)) {
					this->erase(guildMemberId);
					GuildMember* guildMember = &this->at(guildMemberId);
					DataManipFunctions::GetGuildMemberData getGuildMemberData;
					getGuildMemberData.pDataStructure = &guildMember->Data;
					getGuildMemberData.id = guildMemberId;
					getGuildMemberData.guildId = this->guildId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildMemberData).get();
					this->insert(std::make_pair(guildMemberId, *guildMember));
					co_return guildMember;
				}
				else {
					ClientDataTypes::GuildMemberData guildMemberData;
					DataManipFunctions::GetGuildMemberData getGuildMemberData;
					getGuildMemberData.pDataStructure = &guildMemberData;
					getGuildMemberData.id = guildMemberId;
					getGuildMemberData.guildId = this->guildId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildMemberData).get();
					GuildMember guildMember(guildMemberData);
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

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
				this->Messages = new MessageManager(this->Data.id, this->Data.guildId, this->pRestAPI, this->selfUserId);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager* Messages;
		protected:
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
			};

			task<Channel*> fetchAsync(string channelId) {
				if (this->contains(channelId)) {
					this->erase(channelId);
					Channel* channel = &this->at(channelId);
					DataManipFunctions::GetChannelData getChannelData;
					getChannelData.pDataStructure = &channel->Data;
					getChannelData.id = channelId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getChannelData).get();
					this->insert(std::make_pair(channelId, *channel));
					co_return channel;
				}
				else {
					ClientDataTypes::ChannelData channelData;
					DataManipFunctions::GetChannelData getChannelData;
					getChannelData.pDataStructure = &channelData;
					getChannelData.id = channelId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getChannelData).get();
					Channel channel(channelData, this->pRestAPI, this->selfUserId);
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
					Channel* channel;
					co_return channel;
				}
			};

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			string selfUserId;
		};

		class Guild {
		public:

			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->Data = data;
				this->selfUserId = selfUserId;
				this->Channels = new ChannelManager(pRestAPI, this->selfUserId);
				cout << "GUILD ID: " << data.id << endl;
				cout << "GUILD NAME: " << data.name << endl;
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					cout << "CHANNEL ID: " << data.channels.at(x).id << endl;
					Channel channel(data.channels.at(x), pRestAPI, this->selfUserId);
					this->Channels->insert(make_pair(data.channels.at(x).id, channel));
				}
				this->Members = new GuildMemberManager(pRestAPI, this->Data.id);
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					GuildMember member(data.members.at(x));
					this->Members->insert(make_pair(data.members.at(x).user.id, member));
				}
			};

			ClientDataTypes::GuildData Data;
			ChannelManager* Channels;
			GuildMemberManager* Members;
		protected:
			string selfUserId;
		};

		class GuildManager : public map<string, Guild> {

		public:
			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI, string selfUserId) {
				this->pRestAPI = pRestAPI;
				this->selfUserId = selfUserId;
			};

			task<Guild*> fetchAsync(string guildId) {
				if (this->contains(guildId)) {
					this->erase(guildId);
					Guild* guild = &this->at(guildId);
					DataManipFunctions::GetGuildData getGuildData;
					getGuildData.pDataStructure = &guild->Data;
					getGuildData.id = guildId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildData).get();
					this->insert(std::make_pair(guildId, *guild));
					co_return guild;
				}
				else {
					ClientDataTypes::GuildData guildData;
					DataManipFunctions::GetGuildData getGuildData;
					getGuildData.pDataStructure = &guildData;
					getGuildData.id = guildId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildData).get();
					Guild guild(guildData, this->pRestAPI, this->selfUserId);
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
				DataManipFunctions::GetCurrentUserGuildsData getCurrentUserGuildsData;
				map<string, ClientDataTypes::GuildData> guildDataMap;
				for (auto const& [key, val] : this->guilds) {
					guildDataMap.insert(make_pair(key, val.Data));
				}
				getCurrentUserGuildsData.pGuildDataMap = &guildDataMap;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getCurrentUserGuildsData).get();
				for (auto const& [key, val] : guildDataMap) {
					Guild guild(val, this->pRestAPI, this->selfUserId);
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
			User(ClientDataTypes::UserData data) {
				this->Data = data;
			};
			ClientDataTypes::UserData Data;
		};

		class UserManager : public map<string, User> {
		public:
			UserManager() {};
			UserManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<User*> fetchAsync(string userId) {
				if (this->contains(userId)){
					this->erase(userId);
					User* user = &this->at(userId);
					DataManipFunctions::GetUserData getUserData;
					getUserData.pDataStructure = &user->Data;
					getUserData.id = userId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getUserData).get();
					this->insert(std::make_pair(userId, *user));
					co_return user;
				}
				else {
					ClientDataTypes::UserData userData;
					DataManipFunctions::GetUserData getUserData;
					getUserData.pDataStructure = &userData;
					getUserData.id = userId;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, getUserData).get();
					User user(userData);
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

		protected:
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
		};

		struct Client : implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Users = new UserManager(pRestAPI);
				this->User = User;
				this->pRestAPI = pRestAPI;
				this->getUserSelf();
				this->Guilds = new GuildManager(pRestAPI, this->User.Data.id);
				this->Guilds->getCurrentUserGuilds().get();
			};

			void getUserSelf() {
				ClientDataTypes::UserData userData;
				DataManipFunctions::GetSelfUserData getUserData;
				getUserData.pDataStructure = &userData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getUserData).get();
				ClientClasses::User  user(userData);
				this->User = user;
			};

			~Client() {};
			User User;
			UserManager* Users;
			GuildManager* Guilds;

		protected:
			friend struct WebSocketReceiver;
			friend struct WebSocketConnection;
			friend class GuildManager;
			com_ptr<RestAPI> pRestAPI;
		};

		struct ClientAgent :public concurrency::agent, implements<ClientAgent, winrt::Windows::Foundation::IInspectable> {
		public:
			unbounded_buffer<bool> source00;
			unbounded_buffer<com_ptr<Client>> target00;

			ClientAgent(Scheduler* pScheduler, com_ptr<Client> pClient)
				:_source(source00),
				_target(target00),
				agent(*pScheduler)
			{
				this->pClient = pClient;
			}

			~ClientAgent() {};
			
		protected:
			com_ptr<Client> pClient;
			ISource<bool>& _source;
			ITarget<com_ptr<Client>>& _target;

			void run() {
				bool startVal = receive(_source);
				send(_target, this->pClient);
				done();
			};
		};
	};
}
#endif
