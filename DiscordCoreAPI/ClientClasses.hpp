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

		class ReactionManager: map<string, Reaction> {
		public:

			ReactionManager() {};
			ReactionManager(com_ptr<RestAPI> pRestAPI, string channelId, string messageId) {
				this->channelId = channelId;
				this->messageId = messageId;
				this->pRestAPI = pRestAPI;
			};

			task<void> addReactionAsync(ClientDataTypes::CreateReactionData createReactionData){
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

		protected:
			com_ptr<RestAPI> pRestAPI;
			string channelId;
			string messageId;
		};
		class Message {

		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, void* pMessageManager) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->Reactions = ReactionManager(pRestAPI, this->Data.channelId, this->Data.id);
				this->messageManager = pMessageManager;
			}

			task<void> deleteMessageAsync(int timeDelay = 1000) {
				DataManipFunctions::DeleteMessageData deleteMessageData;
				deleteMessageData.channelId = this->Data.channelId;
				deleteMessageData.messageId = this->Data.id;
				deleteMessageData.timeDelay = timeDelay;
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, deleteMessageData);
				co_return;
			};

			void* messageManager;
			ReactionManager Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			com_ptr<RestAPI> pRestAPI;
		};

		class MessageManager :public map<string, Message> {
		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
			};
			
			task<Message> fetchAsync(string messageId) {
				ClientDataTypes::MessageData messageData;
				if (this->contains(messageId)) {
					messageData = this->at(messageId).Data;
				}
				DataManipFunctions::GetMessageData getMessageData;
				getMessageData.pDataStructure = &messageData;
				getMessageData.id = messageId;
				getMessageData.channelId = this->channelId;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getMessageData).get();
				Message message(messageData, this->pRestAPI, this);
				if (this->size() >= 1000) {
					this->erase(this->end());
				}
				this->insert(std::make_pair(messageId, message));
				co_return message;
			};

			task<Message> getMessageAsync(string messageId) {
				Message currentMessage;
				if (this->contains(messageId)) {
					currentMessage = this->at(messageId);
					co_return currentMessage;
				}
				else {
					cout << "Sorry, but they aren't here!" << endl;
					currentMessage = this->fetchAsync(messageId).get();
					co_return currentMessage;
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
					Message message(messageData, this->pRestAPI, this);
					cout << "MESSAGE ID: " << messageData.id << endl;
					this->insert(make_pair(messageData.id, message));
					cout << "MESSAGE ID: " << this->at(messageData.id).Data.id << endl;
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

			task<GuildMember> fetchAsync(string guildMemberId) {
				ClientDataTypes::GuildMemberData guildMemberData;
				if (this->contains(guildMemberId)) {
					guildMemberData = this->at(guildMemberId).Data;
				}
				DataManipFunctions::GetGuildMemberData getGuildMemberData;
				getGuildMemberData.pDataStructure = &guildMemberData;
				getGuildMemberData.id = guildMemberId;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildMemberData).get();
				GuildMember guildMember(guildMemberData);
				this->insert(std::make_pair(guildMemberId, guildMember));
				co_return guildMember;
			};

			task<GuildMember> getGuildMemberAsync(string guildMemberId) {
					if (this->contains(guildMemberId)) {
						co_return this->at(guildMemberId);
					}
					else {
						cout << "Sorry, but they aren't here!" << endl;
						GuildMember guildMember;
						co_return guildMember;
					}
			};

		protected:
			friend struct WebSocket;
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->Messages = MessageManager(this->Data.id, this->Data.guildId, this->pRestAPI);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager Messages;
		protected:
			com_ptr<RestAPI> pRestAPI;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<Channel> fetchAsync(string channelId) {
				ClientDataTypes::ChannelData channelData;
				if (this->contains(channelId)) {
					channelData = this->at(channelId).Data;
				}
				DataManipFunctions::GetChannelData getChannelData;
				getChannelData.pDataStructure = &channelData;
				getChannelData.id = channelId;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getChannelData).get();
				Channel channel(channelData, this->pRestAPI);
				this->insert(std::make_pair(channelId, channel));
				co_return channel;

			};

			task<Channel> getChannelAsync(string channelId) {
				if (this->contains(channelId)) {
					co_return this->at(channelId);
				}
				else {
					cout << "Sorry, but they aren't here!" << endl;
					Channel channel;
					co_return channel;
				}
			};

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
		};

		class Guild {
		public:
			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->Channels = ChannelManager(pRestAPI);
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					Channel channel(data.channels.at(x), pRestAPI);
					this->Channels.insert(make_pair(data.channels.at(x).id, channel));
				}
				this->Members = GuildMemberManager(pRestAPI, this->Data.id);
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					GuildMember member(data.members.at(x));
					this->Members.insert(make_pair(data.members.at(x).user.id, member));
				}
			};
			ClientDataTypes::GuildData Data;
			ChannelManager Channels;
			GuildMemberManager Members;
		};

		class GuildManager : public map<string, Guild> {

		public:
			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<Guild> fetchAsync(string guildId) {
				ClientDataTypes::GuildData guildData;
				if (this->contains(guildId)) {
					guildData = this->at(guildId).Data;
				}
				DataManipFunctions::GetGuildData getGuildData;
				getGuildData.pDataStructure = &guildData;
				getGuildData.id = guildId;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getGuildData).get();
				Guild guild(guildData, this->pRestAPI);
				this->insert(std::make_pair(guildId, guild));
				co_return guild;
			}

			task<Guild> getGuildAsync(string guildId) {
				if (this->contains(guildId)) {
					co_return this->at(guildId);
				}
				else {
					cout << "Sorry, but they aren't here! " << endl;
					Guild guild;
					co_return guild;
				}
			};

		protected:
			friend struct WebSocketConnection;
			friend struct WebSocketReceiver;
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
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

			task<User> fetchAsync(string userId) {
				ClientDataTypes::UserData userData;
				if (this->contains(userId)) {
					userData = this->at(userId).Data;
				}
				DataManipFunctions::GetUserData getUserData;
				getUserData.pDataStructure = &userData;
				getUserData.id = userId;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, getUserData).get();
				User user(userData);
				this->insert(std::make_pair(userId, user));
				co_return user;
			};

			task<User> getUserAsync(string userId) {
					if (this->contains(userId)) {
						co_return this->at(userId);
					}
					else {
						cout << "Sorry, but they aren't here!" << endl;
						ClientDataTypes::UserData userData;
						User user(userData);
						co_return user;
					}
			};

		protected:
			friend struct WebSocket;
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
		};

		struct Client : implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Guilds = GuildManager(pRestAPI);
				this->Users = UserManager(pRestAPI);
				this->User = User;
				this->pRestAPI = pRestAPI;
				this->getUserSelf();
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
			UserManager Users;
			GuildManager Guilds;

		protected:

			friend class GuildManager;
			friend struct WebSocket;
			com_ptr<RestAPI> pRestAPI;
		};
	};
}
#endif
