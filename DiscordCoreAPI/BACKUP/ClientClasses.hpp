﻿// ClientClasses.hpp - Header file for the "Client classes".
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

		class Client;

		class MessageManager;

		class Reaction {
		public:
			Reaction() {};
			Reaction(string name, string messageId, string channelId, string userId) {
				this->Data.emoji.requireColons = true;
				this->Data.emoji.name = name;
				this->name = name;
				this->messageId = messageId;
				this->channelId = channelId;
				this->userId = userId;
			};
			ClientDataTypes::ReactionData Data;
		protected:
			string name;
			string messageId;
			string channelId;
			string userId;
		};

		class ReactionManager: map<string, Reaction> {
		public:
			ReactionManager() {};
			ReactionManager(com_ptr<RestAPI> pRestAPI, string channelId) {
				this->channelId = channelId;
				this->pRestAPI = pRestAPI;
				//this->messageId = messageId;
				ReactionManager::reactionAddRateLimit = make_shared<FoundationClasses::RateLimitation>();
			};
			ClientDataTypes::ReactionData reactionData;

			task<void> AddReactionAsync(ClientDataTypes::CreateReactionData createReactionData){
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
				DataManipFunctions::putObjectDataAsync(this->pRestAPI, ReactionManager::reactionAddRateLimit, this->channelId, createReactionData.messageId, emojiEncoded).get();
				co_return;
			};

		protected:
			static shared_ptr<FoundationClasses::RateLimitation> reactionAddRateLimit;
			com_ptr<RestAPI> pRestAPI;
			string channelId;
			//string messageId;
		};

		class Message {
		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pMessageDeleteRateLimit, void* pMessageManager) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				//this->Reactions = ReactionManager(pRestAPI, this->Data.channelId, this->Data.id, this->pQueueController);
				this->messageManager = pMessageManager;
				this->pMessageDeleteRateLimit = pMessageDeleteRateLimit;
			}

			IAsyncAction DeleteMessageAsync(int timeDelay = 1000) {
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, Message::pMessageDeleteRateLimit, Data.channelId, Data.id).get();
				co_return;
			};

			void* messageManager;
			//ReactionManager Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			com_ptr<RestAPI> pRestAPI;
			shared_ptr<FoundationClasses::RateLimitation> pMessageDeleteRateLimit;
		};

		class MessageManager {
		public:
			MessageManager() {};
			MessageManager(com_ptr<RestAPI> pRestAPI, string guildId, string channelId) {
				this->guildId = guildId;
				this->channelId = channelId;
				this->pRestAPI = pRestAPI;
				MessageManager::messageDeleteRateLimit = make_shared<FoundationClasses::RateLimitation>();
				MessageManager::messageGetRateLimit = make_shared<FoundationClasses::RateLimitation>();
			};

			task<ClientClasses::Message> CreateMessageAsync(ClientDataTypes::CreateMessageData createMessageData) {
				try {
					string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::postObjectDataAsync(this->pRestAPI, MessageManager::messageGetRateLimit, this->channelId, &messageData, createMessagePayload).get();
					Message message(messageData, this->pRestAPI, MessageManager::messageDeleteRateLimit, this);
					//co_await resume_foreground(queue);
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
			static shared_ptr<FoundationClasses::RateLimitation> messageGetRateLimit;
			static shared_ptr<FoundationClasses::RateLimitation> messageDeleteRateLimit;
		};

		class GuildMember {
		public:
			GuildMember() {};
			GuildMember(ClientDataTypes::GuildMemberData data, string guildId) {
				this->guildId = guildId;
				this->Data = data;
			}
			ClientDataTypes::GuildMemberData Data;
			string guildId;
		};

		class GuildMemberManager : map<string, GuildMember> {
		public:
			GuildMemberManager() {};
			GuildMemberManager(com_ptr<RestAPI> pRestAPI, string guildId) {
				this->pRestAPI = pRestAPI;
				this->guildId = guildId;
				GuildMemberManager::guildMemberGetRateLimit = make_shared<FoundationClasses::RateLimitation>();
			}

			task<GuildMember> Fetch(string guildMemberId) {
				return concurrency::create_task([this, guildMemberId] {
					ClientDataTypes::GuildMemberData guildMemberData;
					try {
						guildMemberData = this->at(guildMemberId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData, this->guildId);
						this->insert(std::make_pair(guildMemberId, guildMember));
						return guildMember;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, GuildMemberManager::guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData, this->guildId);
						return guildMember;
					}
					});
			};

			task<GuildMember> GetGuildMember(string guildMemberId) {
				return concurrency::create_task([this, guildMemberId] {
					try {
						return this->at(guildMemberId);
					}
					catch (exception error) {
						cout << "GetGuildMember() Error: " << error.what() << endl;
						GuildMember guildMember;
						return guildMember;
					}
					});
			};

		protected:
			friend struct WebSocket;
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			static shared_ptr<FoundationClasses::RateLimitation> guildMemberGetRateLimit;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->reactions = ReactionManager(pRestAPI, data.id);
				this->messageManager = MessageManager(pRestAPI, data.guildId, data.id);
			};
			ClientDataTypes::ChannelData Data;
			ReactionManager reactions;
			MessageManager messageManager;
		protected:
			com_ptr<RestAPI> pRestAPI;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
				ChannelManager::channelGetRateLimit = make_shared<FoundationClasses::RateLimitation>();
			};

			concurrency::task<Channel> Fetch(string channelId) {
				return concurrency::create_task([this, channelId] {
					ClientDataTypes::ChannelData channelData;
					try {
						channelData = this->at(channelId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						return channel;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						return channel;
					}
					});
			};

			concurrency::task<Channel> GetChannel(string channelId) {
				return concurrency::create_task([this, channelId] {
					try {
						return this->at(channelId);
					}
					catch (exception error) {
						cout << "GetChannel() Error: " << error.what() << endl;
						Channel channel;
						return channel;
					}
					});
			};

		protected:
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			static shared_ptr<FoundationClasses::RateLimitation> channelGetRateLimit;
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
					GuildMember member(data.members.at(x), this->Data.id);
					this->Members.insert(make_pair(data.members.at(x).user.id, member));
				}
			};
			ClientDataTypes::GuildData Data;
			ChannelManager Channels;
			GuildMemberManager Members;
		};

		class GuildManager: map<string, Guild>  {

		public:
			GuildManager() {};

			GuildManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
				GuildManager::guildGetRateLimit = make_shared<FoundationClasses::RateLimitation>();
			};

			concurrency::task<Guild> Fetch(string guildId) {
				return concurrency::create_task([this, guildId] {

					ClientDataTypes::GuildData guildData;
					try {
						guildData = this->at(guildId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, GuildManager::guildGetRateLimit, guildId, &guildData).get();
						Guild guild(guildData, this->pRestAPI);
						this->insert(std::make_pair(guildId, guild));
						return guild;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, GuildManager::guildGetRateLimit, guildId, &guildData).get();
						Guild guild(guildData, this->pRestAPI);
						return guild;
					}
					});
			};

			concurrency::task<Guild> GetGuild(string guildId) {
				return concurrency::create_task([this, guildId] {
					try {
						return this->at(guildId);
					}
					catch (exception error) {
						cout << "GetGuild() Error: " << error.what() << endl;
						ClientDataTypes::GuildData guildData;
						Guild guild;
						return guild;
					}
					});
			};

		protected:
			friend struct WebSocket;
			friend class Client;
			static shared_ptr<FoundationClasses::RateLimitation> guildGetRateLimit;
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
				UserManager::userGetRateLimit = make_shared<FoundationClasses::RateLimitation>();
			};

			concurrency::task<User> Fetch(string userId) {
				return concurrency::create_task([this, userId] {
					ClientDataTypes::UserData userData;
					try {
						userData = this->at(userId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, UserManager::userGetRateLimit, userId, &userData).get();
						User user(userData);
						this->insert(std::make_pair(userId, user));
						return user;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, UserManager::userGetRateLimit, userId, &userData).get();
						User user(userData);
						return user;
					}
					});
			};

			concurrency::task<User> GetUser(string userId) {
				return concurrency::create_task([this, userId] {
					try {
						return this->at(userId);
					}
					catch (exception error) {
						cout << "GetUser() Error: " << error.what() << endl;
						ClientDataTypes::UserData userData;
						User user(userData);
						return user;
					}
					});
			};

		protected:
			friend struct WebSocket;
			friend class Client;
			static shared_ptr<FoundationClasses::RateLimitation> userGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
		};

		class Client {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Guilds = GuildManager(pRestAPI);
				this->Users = UserManager(pRestAPI);
				this->User = User;
			};
			~Client() {};
			User User;
			UserManager Users;
			GuildManager Guilds;
			ChannelManager Channels;
			MessageManager Messages;
		protected:

			friend class GuildManager;
			friend struct WebSocket;
		};

		shared_ptr<FoundationClasses::RateLimitation> GuildMemberManager::guildMemberGetRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> MessageManager::messageDeleteRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> ReactionManager::reactionAddRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> MessageManager::messageGetRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> ChannelManager::channelGetRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> GuildManager::guildGetRateLimit;
		shared_ptr<FoundationClasses::RateLimitation> UserManager::userGetRateLimit;

	};
}
#endif