// ClientClasses.hpp - Header file for the "Client classes".
// Apr 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "DataManipFunctions.hpp"
#include "JSONifier.hpp"

namespace CommanderNS {

	namespace ClientClasses {

		class Client;

		class Message {
		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, void* pMessageManager) {
				this->Data = data;
				this->messageManager = pMessageManager;
			}
			ClientDataTypes::MessageData Data;
			void* messageManager;

		};

		class MessageManager {
		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
			};

			concurrency::task<Message> CreateMessage(ClientDataTypes::CreateMessageData createMessageData) {
				return concurrency::create_task([this, createMessageData] {
					try {
						cout << "HERE HERE HERE" << endl;
						string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
						ClientDataTypes::MessageData messageData;
						DataManipFunctions::postObjectDataAsync(this->pRestAPI, &this->messageGetRateLimit, this->channelId, &messageData, createMessagePayload).get();
						ClientClasses::Message message(messageData, this->pRestAPI, this);
						return message;
					}
					catch (exception error) {
						cout << "CreateMessage() Error: " << error.what() << endl;
					}

					});
			};

		protected:
			string guildId;
			string channelId;
			com_ptr<RestAPI> pRestAPI;
			FoundationClasses::RateLimitation messageGetRateLimit;
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

			concurrency::task<GuildMember> Fetch(string guildMemberId) {
				return concurrency::create_task([this, guildMemberId] {
					ClientDataTypes::GuildMemberData guildMemberData;
					try {
						guildMemberData = this->at(guildMemberId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						this->insert(std::make_pair(guildMemberId, guildMember));
						return guildMember;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->guildMemberGetRateLimit, this->guildId, guildMemberId, &guildMemberData).get();
						GuildMember guildMember(guildMemberData);
						return guildMember;
					}
					});
			};

			concurrency::task<GuildMember> GetGuildMember(string guildMemberId) {
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
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			FoundationClasses::RateLimitation guildMemberGetRateLimit;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->messageManager = MessageManager(this->Data.id, this->Data.guildId, this->pRestAPI);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager messageManager;
		protected:
			com_ptr<RestAPI> pRestAPI;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			concurrency::task<Channel> Fetch(string channelId) {
				return concurrency::create_task([this, channelId] {
					ClientDataTypes::ChannelData channelData;
					try {
						channelData = this->at(channelId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI);
						this->insert(std::make_pair(channelId, channel));
						return channel;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->channelGetRateLimit, channelId, &channelData).get();
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
			FoundationClasses::RateLimitation channelGetRateLimit;
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

		class GuildManager: map<string, Guild>  {
		public:

			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			};

			concurrency::task<Guild> Fetch(string guildId) {
				return concurrency::create_task([this, guildId] {

					ClientDataTypes::GuildData guildData;
					try {
						guildData = this->at(guildId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->guildGetRateLimit, guildId, &guildData).get();
						Guild guild(guildData, this->pRestAPI);
						this->insert(std::make_pair(guildId, guild));
						return guild;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->guildGetRateLimit, guildId, &guildData).get();
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
			FoundationClasses::RateLimitation guildGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
		};

		class User {

		public:
			User() {};
			User(ClientDataTypes::UserData data, com_ptr<RestAPI> pRestAPI) {
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

			concurrency::task<User> Fetch(string userId) {
				return concurrency::create_task([this, userId] {
					ClientDataTypes::UserData userData;
					try {
						userData = this->at(userId).Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData, pRestAPI);
						this->insert(std::make_pair(userId, user));
						return user;
					}
					catch (std::exception error) {
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->userGetRateLimit, userId, &userData).get();
						User user(userData, pRestAPI);
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
						User user(userData, this->pRestAPI);
						return user;
					}
					});
			};

		protected:
			friend struct WebSocket;
			friend class Client;
			FoundationClasses::RateLimitation userGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
		};

		class Client {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Guilds = GuildManager(pRestAPI);
				this->Users = UserManager(pRestAPI);
			};
			~Client() {};
			UserManager Users;
			GuildManager Guilds;

		protected:

			friend class GuildManager;
			friend struct WebSocket;
		};

	};
}