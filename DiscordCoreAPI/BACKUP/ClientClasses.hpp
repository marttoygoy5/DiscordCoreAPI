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
#include "HTTPHandler.hpp"

namespace CommanderNS {

	namespace ClientClasses {

		struct Client;

		class MessageManager;

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
			ReactionManager(com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads, string channelId, string messageId) {
				this->channelId = channelId;
				this->pSystemThreads = pSystemThreads;
				this->messageId = messageId;
				this->pHttpHandler = pHttpHandler;
			};

			task<void> addReactionAsync(ClientDataTypes::CreateReactionData createReactionData){
				string emoji;
				if (createReactionData.emojiId != string()) {
					emoji += ":" + createReactionData.emojiName + ":" + createReactionData.emojiId;
				}
				else {
					emoji = createReactionData.emojiName;
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
				DataManipFunctions::putObjectDataAsync(this->pHttpHandler, putEmojiData).get();
				co_return;
			};

			task<void> deleteUserReactionAsync(DataManipFunctions::DeleteReactionData deleteReactionData) {
				deleteReactionData.messageId = this->messageId;
				deleteReactionData.channelId = this->channelId;
				
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
				deleteReactionData.encodedEmoji = emojiEncoded;
				DataManipFunctions::deleteObjectDataAsync(this->pHttpHandler,  deleteReactionData).get();
				co_return;
			}

			task<void> deleteOwnReactionAsync(DataManipFunctions::DeleteOwnReactionData deleteReactionData) {
				deleteReactionData.channelId = this->channelId;
				deleteReactionData.messageId = this->messageId;
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
				deleteReactionData.encodedEmoji = emojiEncoded;
				DataManipFunctions::deleteObjectDataAsync(this->pHttpHandler, deleteReactionData).get();
				co_return;
			}

		protected:
			friend class Message;
			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
			string channelId;
			string messageId;
		};

		class Message:winrt::Windows::Foundation::IUnknown{
		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<HTTPHandler> pHttpHandler, void* pMessageManager,  com_ptr<SystemThreads> pSystemThreads) {
				this->Data = data;
				this->pSystemThreads = pSystemThreads;
				this->pHttpHandler =pHttpHandler;
				this->Reactions = ReactionManager(pHttpHandler, this->pSystemThreads, this->Data.channelId, this->Data.id);
				this->messageManager = pMessageManager;
			}

			task<void> deleteMessageAsync(unsigned int timeDelay = 0) {
				DataManipFunctions::DeleteMessageData deleteMessageData;
				deleteMessageData.channelId = this->Data.channelId;
				deleteMessageData.messageId = this->Data.id;
				deleteMessageData.timeDelay = timeDelay;
				DataManipFunctions::deleteObjectDataAsync(this->pHttpHandler, deleteMessageData);
				co_return;
			}

			void* messageManager;
			ReactionManager Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			com_ptr<SystemThreads> pSystemThreads;
			com_ptr<HTTPHandler> pHttpHandler;
		};

		class MessageManager: public map<string, Message> {
		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pHttpHandler =pHttpHandler;
				this->pSystemThreads = pSystemThreads;
			};

			task<Message> fetchAsync(string messageId) {
				Message message;
				if (this->contains(messageId)) {
					message = this->at(messageId);
					ClientDataTypes::MessageData messageData = message.Data;
					DataManipFunctions::GetMessageData getMessageData;
					getMessageData.channelId = this->channelId;
					getMessageData.id = messageId;
					getMessageData.pDataStructure = &messageData;
					DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getMessageData).get();
					message = Message(messageData, this->pHttpHandler, this, this->pSystemThreads);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
				else {
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::GetMessageData getMessageData;
					getMessageData.channelId = this->channelId;
					getMessageData.id = messageId;
					getMessageData.pDataStructure = &messageData;
					DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getMessageData).get();
					message = Message(messageData, this->pHttpHandler, this, this->pSystemThreads);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
			};
			
			task<Message> GetMessage(string messageId) {
				Message currentMessage;
				if (this->contains(messageId)) {
					co_return currentMessage;
					currentMessage = this->at(messageId);
				}
				else {
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
					postMessageData.content = createMessagePayload;
					postMessageData.pDataStructure = &messageData;
					DataManipFunctions::postObjectDataAsync(this->pHttpHandler, postMessageData).get();
					Message message(messageData, this->pHttpHandler, this, this->pSystemThreads);
					co_return message;
				}
				catch (exception error) {
					cout << "CreateMessage() Error: " << error.what() << endl;
					Message message;
					co_return message;
				}
			};

		protected:
			friend struct WebSocketAgent;
			friend struct WebSocket;
			friend class Message;
			string guildId;
			string channelId;
			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class GuildMember {
		public:
			GuildMember() {};
			GuildMember(ClientDataTypes::GuildMemberData data) {
				this->Data = data;
			}
			ClientDataTypes::GuildMemberData Data;
		};

		class GuildMemberManager : map<string, GuildMember> {
		public:
			GuildMemberManager() {};
			GuildMemberManager(com_ptr<HTTPHandler> pHttpHandler, string guildId) {
				this->pHttpHandler =pHttpHandler;
				this->guildId = guildId;
			}

			task<GuildMember> fetchAsync(string guildMemberId) {
					ClientClasses::GuildMember guildMember;
					if (this->contains(guildMemberId)) {
						guildMember = this->at(guildMemberId);
						ClientDataTypes::GuildMemberData guildMemberData = guildMember.Data;
						DataManipFunctions::GetGuildMemberData getGuildMemberData;
						getGuildMemberData.guildId = this->guildId;
						getGuildMemberData.id = guildMemberId;
						getGuildMemberData.pDataStructure = &guildMemberData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getGuildMemberData).get();
						guildMember = GuildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
					else {
						ClientDataTypes::GuildMemberData guildMemberData;
						DataManipFunctions::GetGuildMemberData getGuildMemberData;
						getGuildMemberData.guildId = this->guildId;
						getGuildMemberData.id = guildMemberId;
						getGuildMemberData.pDataStructure = &guildMemberData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getGuildMemberData).get();
						guildMember = GuildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
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
			com_ptr<HTTPHandler> pHttpHandler;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Data = data;
				this->pHttpHandler =pHttpHandler;
				this->pSystemThreads = pSystemThreads;
				this->messageManager = new MessageManager(this->Data.id, this->Data.guildId, this->pHttpHandler, this->pSystemThreads);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager* messageManager;
		protected:
			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads>pSystemThreads) {
				this->pHttpHandler =pHttpHandler;
				this->pSystemThreads = pSystemThreads;
			};

			task<Channel> fetchAsync(string channelId) {
					ClientClasses::Channel channel;
					if (this->contains(channelId)) {
						channel = this->at(channelId);
						ClientDataTypes::ChannelData channelData = channel.Data;
						DataManipFunctions::GetChannelData getChannelData;
						getChannelData.id = channelId;
						getChannelData.pDataStructure = &channelData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getChannelData).get();
						channel = Channel(channelData, this->pHttpHandler, this->pSystemThreads);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}
					else {
						ClientDataTypes::ChannelData channelData;
						DataManipFunctions::GetChannelData getChannelData;
						getChannelData.id = channelId;
						getChannelData.pDataStructure = &channelData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getChannelData).get();
						channel = Channel(channelData, this->pHttpHandler, this->pSystemThreads);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}

			};

			task<Channel> getChannelAsync(string channelId) {
				if (this->contains(channelId)) {
					co_return this->at(channelId);
				}
				else {
					cout << "GetChannel() Error: Sorry, but they aren't here!" << endl;
					Channel channel;
					co_return channel;
				}
			};

		protected:
			friend class Guild;
			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class Guild {
		public:
			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Data = data;
				this->pSystemThreads = pSystemThreads;
				this->Channels = ChannelManager(pHttpHandler, pSystemThreads);
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					Channel channel(data.channels.at(x),pHttpHandler, pSystemThreads);
					this->Channels.insert(make_pair(data.channels.at(x).id, channel));
				}
				this->Members = GuildMemberManager(pHttpHandler, this->Data.id);
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					GuildMember member(data.members.at(x));
					this->Members.insert(make_pair(data.members.at(x).user.id, member));
				}
			};
			ClientDataTypes::GuildData Data;
			ChannelManager Channels;
			GuildMemberManager Members;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class GuildManager :public map<string, Guild> {
		public:
			GuildManager() {};
			GuildManager(com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->pHttpHandler =pHttpHandler;
				this->pSystemThreads = pSystemThreads;
			};

			task<Guild> fetchAsync(string guildId) {
				ClientClasses::Guild guild;
				if (this->contains(guildId)) {
					guild = this->at(guildId);
					ClientDataTypes::GuildData guildData = guild.Data;
					DataManipFunctions::GetGuildData getGuildData;
					getGuildData.id = guildId;
					getGuildData.pDataStructure = &guildData;
					DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getGuildData).get();
					guild = Guild(guildData, this->pHttpHandler, this->pSystemThreads);
					this->insert(std::make_pair(guildId, guild));
					co_return guild;
				}
				else {
					ClientDataTypes::GuildData guildData;
					DataManipFunctions::GetGuildData getGuildData;
					getGuildData.id = guildId;
					getGuildData.pDataStructure = &guildData;
					DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getGuildData).get();
					guild = Guild(guildData, this->pHttpHandler, this->pSystemThreads);
					co_return guild;
				}
			}

			task<Guild> getGuildAsync(string guildId) {
				if (this->contains(guildId)) {
					co_return this->at(guildId);
				}
				else {
					cout << "GetGuild() Error: Sorry, but they aren't here! " << endl;
					Guild guild;
					co_return guild;
				}
			};

		protected:
			friend struct WebSocket;
			friend struct WebSocketAgent;
			friend struct Client;
			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class User {

		public:
			User() {};
			User(ClientDataTypes::UserData data) {
				this->Data = data;
				this->Data.avatarURL = this->avatarURL();
			};
			ClientDataTypes::UserData Data;

		protected:
			string avatarURL() {
				string urlString = "https://cdn.discordapp.com/avatars/" +
					this->Data.id + "/" + this->Data.avatar + ".png";
				return urlString;
			}
		};

		class UserManager: public map<string, User> {
		public:
			UserManager() {};
			UserManager(com_ptr<HTTPHandler> pHttpHandler, Client* pClient) {
				this->pHttpHandler =pHttpHandler;
				this->pClient = pClient;
			};

			task<User> fetchAsync(string userId) {
					ClientClasses::User user;
					try {
						user = this->at(userId);
						ClientDataTypes::UserData userData = user.Data;
						DataManipFunctions::GetUserData getUserData;
						getUserData.id = userId;
						getUserData.pDataStructure = &userData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getUserData).get();
						user = User(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
					catch (std::exception error) {
						ClientDataTypes::UserData userData;
						DataManipFunctions::GetUserData getUserData;
						getUserData.id = userId;
						getUserData.pDataStructure = &userData;
						DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getUserData).get();
						user = User(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
			};

			task<User> getUserAsync(string userId) {
				if (this->contains(userId)) {
					co_return this->at(userId);
				}
				else {
					cout << "Sorry, but they aren't here!" << endl;
					User user;
					co_return user;
				}
			};

		protected:
			friend struct WebSocketAgent;
			friend struct WebSocket;
			friend struct Client;
			com_ptr<HTTPHandler> pHttpHandler;
			Client* pClient;
		};

		struct Client : implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			Client() {};
			Client(com_ptr<HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Users = UserManager(pHttpHandler, this);
				this->User = User;
				this->pHttpHandler =pHttpHandler;
				this->getCurrentUser();
				this->pSystemThreads = pSystemThreads;
				this->Guilds = GuildManager(pHttpHandler, this->pSystemThreads);
			};
			~Client() {};
			User User;
			GuildManager Guilds;
			UserManager Users;

		protected:
			friend struct WebSocketAgent;
			friend class GuildManager;
			friend struct WebSocket;
			friend class Guild;
			friend class ChannelManager;
			friend class MessageManager;

			com_ptr<HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;

			task<void> getCurrentUser() {
				ClientDataTypes::UserData userData;
				DataManipFunctions::GetSelfUserData getSelfUserData;
				getSelfUserData.pDataStructure = &userData;
				DataManipFunctions::getObjectDataAsync(this->pHttpHandler, getSelfUserData).get();
				ClientClasses::User user(userData);
				this->User = user;
				co_return;
			}
		};
	};
}
#endif