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
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, deleteReactionData).get();
				co_return;
			}

			task<void> DeleteOwnReactionAsync(ClientDataTypes::DeleteOwnReactionData deleteReactionData) {
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
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, deleteReactionData).get();
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

			void deleteObjectDelegate(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &Message::messageDeleteRateLimit, this->Data.channelId, this->Data.id).get();
			}

			task<void> DeleteAfter(unsigned int delay, ThreadContext* thread) {
				co_await resume_foreground(*thread->threadQueue.get());
				thread->queueTimer.Interval(std::chrono::milliseconds(delay));
				thread->queueTimer.Tick({ this, &Message::deleteObjectDelegate });
				thread->queueTimer.Start();
				co_return;
			}

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

			task<Message> FetchAsync(string channelId, string messageId) {
				ClientClasses::Message message;
				if (this->contains(messageId)) {
					message = this->at(messageId);
					ClientDataTypes::MessageData messageData = message.Data;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, messageId, &messageData).get();
					Message message(messageData, this->pRestAPI, &this->messageDeleteRateLimit, this);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
				else {
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, messageId, &messageData).get();
					Message message(messageData, this->pRestAPI, &MessageManager::messageGetRateLimit, this);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
			};
			
			task<Message> GetMessage(string channelId, string messageId) {
				Message currentMessage;
				if (this->contains(messageId)) {
					currentMessage = this->at(messageId);
					co_return currentMessage;
				}
				else {
					currentMessage = this->FetchAsync(channelId, messageId).get();
					co_return currentMessage;
				}
			}

			task<Message> CreateMessageAsync(ClientDataTypes::CreateMessageData createMessageData) {
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

			task<GuildMember> FetchAsync(string guildMemberId) {
					ClientClasses::GuildMember guildMember;
					if (this->contains(guildMemberId)) {
						guildMember = this->at(guildMemberId);
						ClientDataTypes::GuildMemberData guildMemberData = guildMember.Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
					else {
						ClientDataTypes::GuildMemberData guildMemberData;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						co_return guildMember;
					}
			};

			task<GuildMember> GetGuildMemberAsync(string guildMemberId) {
					if (this->contains(guildMemberId)) {
						co_return this->at(guildMemberId);
					}
					else {
						cout << "Sorry, but they aren't here!" << endl;
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

			task<Channel> FetchAsync(string channelId) {
					ClientClasses::Channel channel;
					if (this->contains(channelId)) {
						channel = this->at(channelId);
						ClientDataTypes::ChannelData channelData = channel.Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}
					else {
						ClientDataTypes::ChannelData channelData;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}

			};

			task<Channel> GetChannelAsync(string channelId) {
				if (this->contains(channelId)) {
					co_return this->at(channelId);
				}
				else {
					cout << "GetChannel() Error: Sorry, but they aren't here!" << endl;
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

			task<Guild> FetchAsync(string guildId) {
				ClientClasses::Guild guild;
				if (this->contains(guildId)) {
					guild = this->at(guildId);
					ClientDataTypes::GuildData guildData = guild.Data;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI);
					this->insert(std::make_pair(guildId, guild));
					co_return guild;
				}
				else {
					ClientDataTypes::GuildData guildData;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI);
					co_return guild;
				}
			}

			task<Guild> GetGuildAsync(string guildId) {
					if (this->contains(guildId)) {
						co_return this->at(guildId);
					}
					else {
						cout << "GetGuild() Error: Sorry, but they aren't here! " << endl;
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

		class UserManager: map<string, User> {
		public:
			UserManager() {};
			UserManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			task<User> FetchAsync(string userId) {
					ClientClasses::User user;
					try {
						user = this->at(userId);
						ClientDataTypes::UserData userData = user.Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
					catch (std::exception error) {
						ClientDataTypes::UserData userData;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData);
						this->insert(std::make_pair(userId, user));
						co_return user;
					}
			};

			task<User> GetUserAsync(string userId) {
					if (this->contains(userId)) {
						co_return this->at(userId);
					}
					else {
						cout << "Sorry, but they aren't here!" << endl;
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
			};
			~Client() {};
			User User;
			GuildManager Guilds;
			UserManager Users;

		protected:
			task<void> GetCurrentUser() {
				ClientDataTypes::UserData userData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->Users.userGetRateLimit, &userData).get();
				ClientClasses::User user(userData);
				this->User = user;
				co_return;
			}

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
