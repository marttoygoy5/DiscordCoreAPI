// ClientClasses.hpp - Header file for the "Client classes".
// Apr 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "DataManipFunctions.hpp"

namespace CommanderNS {

	namespace ClientClasses {

		class Client;

		class GuildMember {
		public:
			GuildMember() {};
			GuildMember(ClientDataTypes::GuildMemberData data) {
				this->Data = data;
			}
			ClientDataTypes::GuildMemberData Data;
		};

		class GuildMemberManager: map<string, GuildMember> {
		public:
			GuildMemberManager() {};
			GuildMemberManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
			}

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
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data) {
				this->Data = data;
			};
			ClientDataTypes::ChannelData Data;
		};

		class ChannelManager: map<string, Channel> {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI) {
				this->pRestAPI = pRestAPI;
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
		};

		class Guild {
		public:
			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<RestAPI> pRestAPI) {
				this->Data = data;
				this->Channels = ChannelManager(pRestAPI);
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					Channel channel(data.channels.at(x));
					this->Channels.insert(std::make_pair(data.channels.at(x).id, channel));
				}
				this->Members = GuildMemberManager(pRestAPI);
				for (unsigned int x = 0; x < data.members.size(); x += 1) {
					GuildMember member(data.members.at(x));
					this->Members.insert(std::make_pair(data.members.at(x).user.id, member));
				}
			};
			ClientDataTypes::GuildData Data;
			ChannelManager Channels;
			GuildMemberManager Members;
		};

		class GuildManager: map<string, Guild> {
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
						Guild guild;
						return guild;
					}
					});
			};

		protected:
			friend struct WebSocket;
			FoundationClasses::RateLimitation guildGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
		};

		class User {

		public:
			User(ClientDataTypes::UserData data) {
				this->Data = data;
			};
			ClientDataTypes::UserData Data;
		};

		class UserManager {
		public:
			std::map<std::string, User> UserCache;
		};

		class Client {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI) {
				this->Guilds = GuildManager(pRestAPI);
			};
			UserManager Users;
			GuildManager Guilds;

		protected:

			friend class GuildManager;
			friend struct WebSocket;
		};

	};
}