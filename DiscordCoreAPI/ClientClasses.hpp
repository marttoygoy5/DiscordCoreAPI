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
#include "HttpAgents.hpp"

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
			ReactionManager(com_ptr<RestAPI> pRestAPI, com_ptr<SystemThreads> pSystemThreads, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, string channelId, string messageId) {
				this->channelId = channelId;
				this->pSystemThreads = pSystemThreads;
				this->messageId = messageId;
				this->pHttpHandler = pHttpHandler;
				this->pRestAPI = pRestAPI;
			};

			task<void> AddReactionAsync(ClientDataTypes::CreateReactionData createReactionData){
				critical_section critSection;
				scoped_lock lock(critSection);
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
				HttpAgents::WorkloadData workload;
				workload.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::REACTION_ADD_REMOVE;
				DataManipFunctions::putObjectDataAsync(this->pRestAPI, this->pHttpHandler, this->pSystemThreads->Threads.at(2).scheduler, this->channelId, this->messageId, emojiEncoded, workload).get();
				co_return;
			};

			task<void> DeleteUserReactionAsync(ClientDataTypes::DeleteReactionData deleteReactionData) {
				critical_section critSection;
				scoped_lock lock(critSection);
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
				critical_section critSection;
				scoped_lock lock(critSection);
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
			friend class Message;
			friend class HttpAgents::HTTPHandler;
			com_ptr<RestAPI> pRestAPI;
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
			static FoundationClasses::RateLimitData reactionAddRemoveRateLimit;
			string channelId;
			string messageId;
		};

		class Message:winrt::Windows::Foundation::IUnknown{
		public:
			Message() {};
			Message(ClientDataTypes::MessageData data, com_ptr<RestAPI> pRestAPI, void* pMessageManager, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Data = data;
				this->pSystemThreads = pSystemThreads;
				this->pRestAPI = pRestAPI;
				this->pHttpHandler = pHttpHandler;
				this->Reactions = ReactionManager(pRestAPI, this->pSystemThreads, pHttpHandler, this->Data.channelId, this->Data.id);
				this->messageManager = pMessageManager;
			}

			task<void> deleteObjectDelegate(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args){
				DataManipFunctions::deleteObjectDataAsync(this->pRestAPI, &ReactionManager::reactionAddRemoveRateLimit, this->Data.channelId, this->Data.id).get();
				co_return;
			}

			task<void> DeleteAfter(unsigned int delay, ThreadContext* thread) {
				thread->queueTimer.Interval(std::chrono::milliseconds(delay));
				thread->queueTimer.Tick({ this, &Message::deleteObjectDelegate });
				thread->queueTimer.Start();
				co_return;
			}

			void* messageManager;
			ReactionManager Reactions;
			ClientDataTypes::MessageData Data;
		protected:
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
			friend class HttpAgents::HTTPHandler;
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitData messageDeleteRateLimit;
		};

		class MessageManager: map<string, Message> {
		public:
			MessageManager() {};
			MessageManager(string channelId, string guildId, com_ptr<RestAPI> pRestAPI, com_ptr<SystemThreads> pSystemThreads, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
				this->channelId = channelId;
				this->guildId = guildId;
				this->pRestAPI = pRestAPI;
				this->pSystemThreads = pSystemThreads;
				this->pHttpHandler = pHttpHandler;
			};

			task<Message> FetchAsync(string messageId) {
				ClientClasses::Message message;
				if (this->contains(messageId)) {
					message = this->at(messageId);
					ClientDataTypes::MessageData messageData = message.Data;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, messageId, &messageData).get();
					Message message(messageData, this->pRestAPI, this, this->pHttpHandler, this->pSystemThreads);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
				else {
					ClientDataTypes::MessageData messageData;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &MessageManager::messageGetRateLimit, this->channelId, messageId, &messageData).get();
					Message message(messageData, this->pRestAPI, this, this->pHttpHandler, this->pSystemThreads);
					this->insert(std::make_pair(messageId, message));
					co_return message;
				}
			};
			
			task<Message> GetMessage(string channelId, string messageId) {
				Message currentMessage;
				if (this->contains(messageId)) {
					co_return currentMessage;
					currentMessage = this->at(messageId);
				}
				else {
					currentMessage = this->FetchAsync(messageId).get();
					co_return currentMessage;
				}
			}

			task<Message> CreateMessageAsync(ClientDataTypes::CreateMessageData createMessageData) {
				try {
					string createMessagePayload = JSONifier::getCreateMessagePayload(createMessageData);
					ClientDataTypes::MessageData messageData;
					HttpAgents::WorkloadData workloadData;
					workloadData.content = createMessagePayload;
					workloadData.workloadType = HttpAgents::WorkloadType::POST;
					workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::MESSAGE_CREATE;
					DataManipFunctions::postObjectDataAsync(this->pRestAPI, this->pHttpHandler, this->pSystemThreads->Threads.at(2).scheduler, this->channelId, &messageData, workloadData).get();
					string bucketValue = HttpAgents::HTTPHandler::rateLimitDataBucketValues.at(FoundationClasses::RateLimitType::MESSAGE_CREATE);
					Message message(messageData, this->pRestAPI, this, this->pHttpHandler, this->pSystemThreads);
					co_return message;
				}
				catch (exception error) {
					cout << "CreateMessage() Error: " << error.what() << endl;
					Message message;
					co_return message;
				}
			};

		protected:
			friend struct WebSocket;
			friend class HttpAgents::HTTPHandler;
			friend class Message;
			string guildId;
			string channelId;
			com_ptr<RestAPI> pRestAPI;
			com_ptr<SystemThreads> pSystemThreads;
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			static FoundationClasses::RateLimitData messageGetRateLimit;
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
						GuildMember guildMember;
						co_return guildMember;
					}
			};

		protected:
			friend struct WebSocket;
			friend class HttpAgents::HTTPHandler;
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			static FoundationClasses::RateLimitData guildMemberGetRateLimit;
			string guildId;
		};

		class Channel {
		public:
			Channel() {};
			Channel(ClientDataTypes::ChannelData data, com_ptr<RestAPI> pRestAPI, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Data = data;
				this->pRestAPI = pRestAPI;
				this->pSystemThreads = pSystemThreads;
				this->pHttpHandler = pHttpHandler;
				this->messageManager = new MessageManager(this->Data.id, this->Data.guildId, this->pRestAPI, this->pSystemThreads, this->pHttpHandler);
			};
			ClientDataTypes::ChannelData Data;
			MessageManager* messageManager;
		protected:
			com_ptr<RestAPI> pRestAPI;
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			com_ptr<SystemThreads> pSystemThreads;
		};

		class ChannelManager: map<string, Channel>  {
		public:

			ChannelManager() {};
			ChannelManager(com_ptr<RestAPI> pRestAPI, com_ptr<SystemThreads>pSystemThreads, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
				this->pRestAPI = pRestAPI;
				this->pHttpHandler = pHttpHandler;
				this->pSystemThreads = pSystemThreads;
			};

			task<Channel> FetchAsync(string channelId) {
					ClientClasses::Channel channel;
					if (this->contains(channelId)) {
						channel = this->at(channelId);
						ClientDataTypes::ChannelData channelData = channel.Data;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI, this->pHttpHandler, this->pSystemThreads);
						this->insert(std::make_pair(channelId, channel));
						co_return channel;
					}
					else {
						ClientDataTypes::ChannelData channelData;
						DataManipFunctions::getObjectDataAsync(this->pRestAPI, &ChannelManager::channelGetRateLimit, channelId, &channelData).get();
						Channel channel(channelData, this->pRestAPI, this->pHttpHandler, this->pSystemThreads);
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
					Channel channel;
					co_return channel;
				}
			};

		protected:
			friend class HttpAgents::HTTPHandler;
			friend class Guild;
			com_ptr<RestAPI> pRestAPI;
			com_ptr<SystemThreads> pSystemThreads;
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			static FoundationClasses::RateLimitData channelGetRateLimit;
		};

		class Guild {
		public:
			Guild() {};
			Guild(ClientDataTypes::GuildData data, com_ptr<RestAPI> pRestAPI, com_ptr<SystemThreads> pSystemThreads, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
				this->Data = data;
				this->pSystemThreads = pSystemThreads;
				this->pHttpHandler = pHttpHandler;
				this->Channels = ChannelManager(pRestAPI,pSystemThreads, pHttpHandler);
				for (unsigned int x = 0; x < data.channels.size(); x += 1) {
					Channel channel(data.channels.at(x), pRestAPI, pHttpHandler, pSystemThreads);
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
			com_ptr<SystemThreads> pSystemThreads;
			shared_ptr < HttpAgents::HTTPHandler> pHttpHandler;
		};

		class GuildManager : map<string, Guild> {
		public:
			GuildManager() {};
			GuildManager(com_ptr<RestAPI> pRestAPI, com_ptr<SystemThreads> pSystemThreads, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
				this->pRestAPI = pRestAPI;
				this->pHttpHandler = pHttpHandler;
				this->pSystemThreads = pSystemThreads;
			};

			task<Guild> FetchAsync(string guildId) {
				ClientClasses::Guild guild;
				if (this->contains(guildId)) {
					guild = this->at(guildId);
					ClientDataTypes::GuildData guildData = guild.Data;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI, this->pSystemThreads, this->pHttpHandler);
					this->insert(std::make_pair(guildId, guild));
					co_return guild;
				}
				else {
					ClientDataTypes::GuildData guildData;
					DataManipFunctions::getObjectDataAsync(this->pRestAPI, &GuildManager::guildGetRateLimit, guildId, &guildData).get();
					Guild guild(guildData, this->pRestAPI, this->pSystemThreads, this->pHttpHandler);
					co_return guild;
				}
			}

			task<Guild> GetGuildAsync(string guildId) {
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
			friend class HttpAgents::HTTPHandler;
			friend struct Client;
			static FoundationClasses::RateLimitData guildGetRateLimit;
			com_ptr<RestAPI> pRestAPI;
			com_ptr<SystemThreads> pSystemThreads;
			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
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
			UserManager(com_ptr<RestAPI> pRestAPI, Client* pClient) {
				this->pRestAPI = pRestAPI;
				this->pClient = pClient;
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
						User user;
						co_return user;
					}
			};

			static FoundationClasses::RateLimitData userGetRateLimit;

		protected:
			friend class HttpAgents::HTTPHandler;
			friend struct WebSocket;
			friend struct Client;
			com_ptr<RestAPI> pRestAPI;
			Client* pClient;
		};

		struct Client: implements<Client, winrt::Windows::Foundation::IInspectable> {
		public:
			Client() {};
			Client(com_ptr<RestAPI> pRestAPI, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, com_ptr<SystemThreads> pSystemThreads) {
				this->Users = UserManager(pRestAPI, this);
				this->User = User;
				this->pRestAPI = pRestAPI;
				this->pHttpHandler = pHttpHandler;
				this->GetCurrentUser();
				this->pSystemThreads = pSystemThreads;
				this->Guilds = GuildManager(pRestAPI, this->pSystemThreads, this->pHttpHandler);
			};
			~Client() {};
			User User;
			GuildManager Guilds;
			UserManager Users;

		protected:
			friend class GuildManager;
			friend struct WebSocket;
			friend class Guild;
			friend class ChannelManager;
			friend class MessageManager;

			shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
			com_ptr<RestAPI> pRestAPI;
			com_ptr<SystemThreads> pSystemThreads;

			task<void> GetCurrentUser() {
				ClientDataTypes::UserData userData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, &this->Users.userGetRateLimit, &userData).get();
				ClientClasses::User user(userData);
				this->User = user;
				co_return;
			}
		};
		FoundationClasses::RateLimitData ReactionManager::reactionAddRemoveRateLimit;
		FoundationClasses::RateLimitData Message::messageDeleteRateLimit;
		FoundationClasses::RateLimitData GuildManager::guildGetRateLimit;
		FoundationClasses::RateLimitData ChannelManager::channelGetRateLimit;
		FoundationClasses::RateLimitData GuildMemberManager::guildMemberGetRateLimit;
		FoundationClasses::RateLimitData MessageManager::messageGetRateLimit;
		FoundationClasses::RateLimitData UserManager::userGetRateLimit;
	};
}
#endif
