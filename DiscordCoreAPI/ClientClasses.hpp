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
			ReactionManager(com_ptr<RestAPI> pRestAPI, string channelId, string messageId) {
				this->channelId = channelId;
				this->messageId = messageId;
				this->pRestAPI = pRestAPI;
			};

			task<void> AddReactionAsync(ClientDataTypes::CreateReactionData createReactionData){
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
				DataManipFunctions::putObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, this->channelId, this->messageId, emojiEncoded).get();
				co_return;
			};

			task<void> DeleteUserReactionAsync(ClientDataTypes::DeleteReactionData deleteReactionData) {
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
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, this->channelId, this->messageId, deleteReactionData.userId, emojiEncoded).get();
				co_return;
			}

			task<void> DeleteOwnReaction(ClientDataTypes::DeleteReactionData deleteReactionData) {
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
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, this->channelId, this->messageId, deleteReactionData.userId, emojiEncoded).get();
				co_return;
			}

		protected:
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitation reactionAddRemoveRateLimit;
			string channelId;
			string messageId;
		};

		class Message {
		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pMessageDeleteRateLimit, void* pMessageManager) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->Reactions = ReactionManager(pRestAPI, this->Data.channelId, this->Data.id);
				this->messageManager = pMessageManager;
			}

			IAsyncAction DeleteMessageAsync(int timeDelay = 1000) {
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &Message::messageDeleteRateLimit, Data.channelId, Data.id).get();
				co_return;
			};

			void* messageManager;
			ReactionManager Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitation messageDeleteRateLimit;
		};

		class MessageManager: map<string, Message> {
		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
			};
			
			task<Message> Fetch(string channelId, string messageId) {
				ClientDataTypes::MessageData messageData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, messageId, &messageData).get();
				Message message(messageData, this->pRestAPI, &MessageManager::messageGetRateLimit, this);
				this->insert(std::make_pair(messageId, message));
				co_return message;
			};

			task<Message> getMessage(string channelId, string messageId) {
				Message currentMessage;
				if (this->contains(messageId)) {
					currentMessage = this->at(messageId);
					co_return currentMessage;
				}
				else {
					currentMessage = this->Fetch(channelId, messageId).get();
					co_return currentMessage;
				}
			}

			task<ClientClasses::Message> CreateMessageAsync(ClientDataTypes::CreateMessageData createMessageData) {
				try {
					string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::postObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, &messageData, createMessagePayload).get();
					Message message(messageData, this->pRestAPI, &MessageManager::messageDeleteRateLimit, this);
					co_return message;
				}
				catch (exception error) {
					cout << "CreateMessage() Error: " << error.what() << endl;
				}
			};

		protected:
			friend struct WebSocket;
			friend class Message;
			string guildId;
			string channelId;
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitation messageGetRateLimit;
			static FoundationClasses::RateLimitation messageDeleteRateLimit;
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
			GuildMemberManager(com_ptr<RestAPI> pRestAPI, string guildId) {
				this->pRestAPI = pRestAPI;
				this->guildId = guildId;
			}

			task<GuildMember> Fetch(string guildMemberId) {
					ClientDataTypes::GuildMemberData guildMemberData;
					if (this->contains(guildMemberId)) {
						guildMemberData = this->at(guildMemberId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
					else {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
			};

			task<GuildMember> GetGuildMember(string guildMemberId) {
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
			static FoundationClasses::RateLimitation guildMemberGetRateLimit;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->messageManager = new MessageManager(this->Data.id, this->Data.guildId, this->pRestAPI);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager* messageManager;
		protected:
			com_ptr<RestAPI> pRestAPI;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<Channel> Fetch(string channelId) {
					ClientDataTypes::ChannelData channelData;
					if (this->contains(channelId)) {
						channelData = this->at(channelId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}
					else {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}

			};

			task<Channel> GetChannel(string channelId) {
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
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitation channelGetRateLimit;
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

		class GuildManager : map<string, Guild> {
		public:
			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<Guild> Fetch(string guildId) {
				ClientDataTypes::GuildData guildData;
				if (this->contains(guildId)) {
					guildData = this->at(guildId).Data;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI);
					this->insert(std::make_pair(guildId, guild));
					co_return guild;
				}
				else {
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI);
					co_return guild;
				}
			}

			task<Guild> GetGuild(string guildId) {
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
			friend struct Client;
			static FoundationClasses::RateLimitation guildGetRateLimit;
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

		class UserManager: map<string, User> {
		public:
			UserManager() {};
			UserManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<User> Fetch(string userId) {
					ClientDataTypes::UserData userData;
					try {
						userData = this->at(userId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
			};

			task<User> GetUser(string userId) {
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
			FoundationClasses::RateLimitation userGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
		};

		struct Client: implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Guilds = GuildManager(pRestAPI);
				this->Users = UserManager(pRestAPI);
				this->User = User;
				this->pRestAPI = pRestAPI;
				this->GetCurrentUser();
				cout << this->User.Data.avatarURL() << endl;
			};
			~Client() {};
			User User;
			UserManager Users;
			GuildManager Guilds;

			task<void> GetCurrentUser() {
				ClientDataTypes::UserData userData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->Users.userGetRateLimit, &userData).get();
				ClientClasses::User user(userData);
				this->User = user;
				co_return;
			}

		protected:
			com_ptr<RestAPI> pRestAPI;
			friend class GuildManager;
			friend struct WebSocket;
		};
		FoundationClasses::RateLimitation ReactionManager::reactionAddRemoveRateLimit;
		FoundationClasses::RateLimitation Message::messageDeleteRateLimit;
		FoundationClasses::RateLimitation GuildManager::guildGetRateLimit;
		FoundationClasses::RateLimitation ChannelManager::channelGetRateLimit;
		FoundationClasses::RateLimitation GuildMemberManager::guildMemberGetRateLimit;
		FoundationClasses::RateLimitation MessageManager::messageGetRateLimit;
		FoundationClasses::RateLimitation MessageManager::messageDeleteRateLimit;
	};
}
#endif
