// DataManipFunctions.hpp - Header for the data manipulation functions.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATA_MANIP_FUNCTIONS_
#define _DATA_MANIP_FUNCTIONS_

#include "pch.h"
#include "RestAPI.hpp"
#include "FoundationClasses.hpp"
#include "DataParsingFunctions.hpp"
#include "ClientDataTypes.hpp"
#include "ClientClasses.hpp"
#include "HttpAgents.hpp"

namespace CommanderNS {
	/*
	namespace DataManipFunctions {

	
		struct GetSelfUserData {
			ClientDataTypes::UserData* pDataStructure;
		};

		struct GetUserData {
			ClientDataTypes::UserData* pDataStructure;
			string id;
		};

		struct GetGuildData {
			ClientDataTypes::GuildData* pDataStructure;
			string id;
		};

		

		struct GetGuildMemberData {
			ClientDataTypes::GuildMemberData* pDataStructure;
			string guildId;
			string id;
		};

		struct GetMessageData {
			ClientDataTypes::MessageData* pDataStructure;
			string channelId;
			string id;
		};

		struct GetRolesData {
			map<string, ClientDataTypes::RoleData>* pDataStructure;
			string guildId;
		};

		struct PostMessageData {
			ClientDataTypes::MessageData* pDataStructure;
			string content;
			string channelId;
		};

		

		

		struct GetCurrentUserGuildsData {
			map<string, ClientDataTypes::GuildData>* pGuildDataMap;
		};

		struct GetGuildMembersData {
			string guildId;
			map<string, ClientDataTypes::GuildMemberData>* pGuildMemberDataMap;
		};

		struct EditMessageData {
			string content;
			string channelId;
			string messageId;
			ClientDataTypes::MessageData* pMessageData;
		};

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildMembersData getGuildMembersData) {
			string relativePath = "/guilds/" + getGuildMembersData.guildId + "/members?limit=1000";
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_GUILD_MEMBERS;
			send(buffer00, workloadData);
			HTTPData getData = receive(buffer03);
			json jsonValue = getData.data;
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				json jsonGuildValue = jsonValue.at(x);
				if (getGuildMembersData.pGuildMemberDataMap->contains(jsonGuildValue.at("user").at("id"))) {
					ClientDataTypes::GuildMemberData guildMemberData = getGuildMembersData.pGuildMemberDataMap->at(jsonGuildValue.at("user").at("id"));
					DataParsingFunctions::parseObject(jsonGuildValue, &guildMemberData);
					getGuildMembersData.pGuildMemberDataMap->erase(jsonGuildValue.at("user").at("id"));
					getGuildMembersData.pGuildMemberDataMap->insert(make_pair(guildMemberData.user.id, guildMemberData));
				}
				else {
					ClientDataTypes::GuildMemberData guildMemberData;
					DataParsingFunctions::parseObject(jsonGuildValue, &guildMemberData);
					getGuildMembersData.pGuildMemberDataMap->insert(make_pair(guildMemberData.user.id, guildMemberData));
				}
			}
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetCurrentUserGuildsData getCurrentUserGuildsData) {
			string relativePath = "/users/@me/guilds";
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_USER_GUILDS;
			send(buffer00, workloadData);
			HTTPData getData;
			getData = receive(buffer03);
			json jsonValue = getData.data;
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				json jsonGuildValue = jsonValue.at(x);
				if (getCurrentUserGuildsData.pGuildDataMap->contains(jsonGuildValue.at("id"))) {
					ClientDataTypes::GuildData guildData = getCurrentUserGuildsData.pGuildDataMap->at(guildData.id);
					DataParsingFunctions::parseObject(jsonGuildValue, &guildData);
					getCurrentUserGuildsData.pGuildDataMap->erase(guildData.id);
					getCurrentUserGuildsData.pGuildDataMap->insert(make_pair(guildData.id, guildData));
				}
				else {
					ClientDataTypes::GuildData guildData;
					DataParsingFunctions::parseObject(jsonGuildValue, &guildData);
					getCurrentUserGuildsData.pGuildDataMap->insert(make_pair(guildData.id, guildData));
				}				
			}
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetSelfUserData getUserData) {
			ClientDataTypes::UserData userData = *getUserData.pDataStructure;
			string relativePath = "/users/@me";
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_USER_SELF;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetUserData getUserData) {
			ClientDataTypes::UserData userData = *getUserData.pDataStructure;
			string relativePath = "/users/" + getUserData.id;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_USER;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildData getGuildData) {
			ClientDataTypes::GuildData guildData = *getGuildData.pDataStructure;
			string relativePath = "/guilds/" + getGuildData.id;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_GUILD;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*getGuildData.pDataStructure = guildData;
			co_return;
		}
		
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetChannelData getChannelData) {
			ClientDataTypes::ChannelData channelData = *getChannelData.pDataStructure;
			string relativePath = "/channels/" + getChannelData.id;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_CHANNEL;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*getChannelData.pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildMemberData getGuildMemberData) {
			ClientDataTypes::GuildMemberData guildMemberData = *getGuildMemberData.pDataStructure;
			string relativePath = "/guilds/" + getGuildMemberData.guildId + "/members/" + getGuildMemberData.id;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_GUILD_MEMBER;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*getGuildMemberData.pDataStructure = guildMemberData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetMessageData getMessageData) {
			ClientDataTypes::MessageData messageData = *getMessageData.pDataStructure;
			string relativePath = "/channels/" + getMessageData.channelId + "/messages/" + getMessageData.id;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_MESSAGE;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*getMessageData.pDataStructure = messageData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetRolesData getRolesData) {
			map<string, ClientDataTypes::RoleData> roleData = *getRolesData.pDataStructure;
			string relativePath = "/guilds/" + getRolesData.guildId + "/roles";
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(1).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GET_ROLES;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				json jsonGuildValue = jsonValue.at(x);
				if (getRolesData.pDataStructure->contains(jsonGuildValue.at("id"))) {
					ClientDataTypes::RoleData roleData = getRolesData.pDataStructure->at(jsonGuildValue.at("id"));
					DataParsingFunctions::parseObject(jsonGuildValue, &roleData);
					getRolesData.pDataStructure->erase(roleData.id);
					getRolesData.pDataStructure->insert(make_pair(roleData.id, roleData));
				}
				else {
					ClientDataTypes::RoleData roleData;
					DataParsingFunctions::parseObject(jsonGuildValue, &roleData);
					getRolesData.pDataStructure->insert(make_pair(roleData.id, roleData));
				}
			}
			co_return;
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, PutEmojiData putEmojiData) {
			string relativePath = "/channels/" + putEmojiData.channelId + "/messages/" + putEmojiData.messageId + "/reactions/" + putEmojiData.emoji + "/@me";
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::PUT;
			workloadData.rateLimitData.rateLimitType = RateLimitType::PUT_REACTION;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, PostMessageData postMessageData) {
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			HTTPData postData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(3).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(3).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::POST;
			workloadData.rateLimitData.rateLimitType = RateLimitType::POST_MESSAGE;
			workloadData.content = postMessageData.content;
			send(buffer00, workloadData);
			postData = receive(buffer03);
			json jsonValue = postData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
		}

		IAsyncAction patchObjectDataAsync(com_ptr<RestAPI> pRestAPI, EditMessageData editMessageData) {
			string relativePath = "/channels/" + editMessageData.channelId + "/messages/" + editMessageData.messageId;
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(4).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(4).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.content = editMessageData.content;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::PATCH;
			workloadData.rateLimitData.rateLimitType = RateLimitType::PATCH_MESSAGE;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI,  DeleteMessageData deleteMessageData) {
			string relativePath = "/channels/" + deleteMessageData.channelId + "/messages/" + deleteMessageData.messageId;
			HTTPData getData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::TimedRequestSender> requestSender = make_self<HttpAgents::TimedRequestSender>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer00, buffer01, buffer02, buffer03, deleteMessageData.timeDelay);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_MESSAGE;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteOwnReactionData deleteReactionData) {
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji + "/@me";
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_REACTION;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteReactionData deleteReactionData){
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji + "/" + deleteReactionData.userId;
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_REACTION;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteAllReactionsByEmojiData deleteReactionData) {
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji;
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_REACTION;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteAllReactionsData deleteReactionData) {
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions";
			HTTPData deleteData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(5).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_REACTION;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

	}
	*/
};
#endif
