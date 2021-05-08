// DataManipFunctions.hpp - Header for the data manipulation functions.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATA_MANIP_FUNCTIONS_
#define _DATA_MANIP_FUNCTIONS_

#include "pch.h"
#include "FoundationClasses.hpp"
#include "DataParsingFunctions.hpp"
#include "ClientDataTypes.hpp"
#include "SystemThreads.hpp"
#include "HttpHandler.hpp"

namespace CommanderNS {

	namespace DataManipFunctions {

		struct DeleteReactionData {
			string channelId;
			string messageId;
			string encodedEmoji;
			string emojiName;
			string emojiId;
			string userId;
		};

		struct DeleteOwnReactionData {
			string channelId;
			string messageId;
			string encodedEmoji;
			string emojiName;
			string emojiId;
		};

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

		struct GetChannelData {
			ClientDataTypes::ChannelData* pDataStructure;
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
			vector<ClientDataTypes::RoleData>* pDataStructure;
			string guildId;
		};

		struct PostMessageData {
			ClientDataTypes::MessageData* pDataStructure;
			string content;
			string channelId;
		};

		struct PutEmojiData {
			string channelId;
			string messageId; 
			string emoji;
		};

		struct DeleteMessageData {
			string channelId;
			string messageId;
			unsigned int timeDelay;
		};

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetSelfUserData getUserSelfData) {
			ClientDataTypes::UserData userData = *getUserSelfData.pDataStructure;
			string relativePath = "/users/@me";
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::USER_SELF_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserSelfData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetUserData getUserData) {
			ClientDataTypes::UserData userData = *getUserData.pDataStructure;
			string relativePath = "/users/" + getUserData.id;
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::USER_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildData getGuildData){
			ClientDataTypes::GuildData guildData = *getGuildData.pDataStructure;
			string relativePath = "/guilds/" + getGuildData.id;
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GUILD_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*getGuildData.pDataStructure = guildData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetChannelData getChannelData) {
			ClientDataTypes::ChannelData channelData = *getChannelData.pDataStructure;
			string relativePath = "/channels/" + getChannelData.id;
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::CHANNEL_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*getChannelData.pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildMemberData getGuildMemberData) {
			ClientDataTypes::GuildMemberData guildMemberData = *getGuildMemberData.pDataStructure;
			string relativePath = "/guilds/" + getGuildMemberData.guildId + "/members/" + getGuildMemberData.id;
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GUILD_MEMBER_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*getGuildMemberData.pDataStructure = guildMemberData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetMessageData getMessageData) {
			ClientDataTypes::MessageData messageData = *getMessageData.pDataStructure;
			string relativePath = "/channels/" + getMessageData.channelId + "/messages/" + getMessageData.id;
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::MESSAGE_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*getMessageData.pDataStructure = messageData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetRolesData getRolesData) {
			vector<ClientDataTypes::RoleData> roleData = *getRolesData.pDataStructure;
			string relativePath = "/guilds/" + getRolesData.guildId + "/roles";
			WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::ROLES_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				bool isItFound = false;
				for (unsigned int y = 0; y < roleData.size(); y += 1) {
					if (roleData.at(y).id == jsonValue.at(x).at("id")) {
						isItFound = true;
						ClientDataTypes::RoleData roleDataNew = roleData.at(y);
						DataParsingFunctions::parseObject(jsonValue.at(x), &roleDataNew);
						roleData.erase(roleData.begin() + y);
						roleData.push_back(roleDataNew);
					}
				}
				if (isItFound == false) {
					ClientDataTypes::RoleData roleDataNew;
					DataParsingFunctions::parseObject(jsonValue.at(x), &roleDataNew);
					roleData.push_back(roleDataNew);
				}
			}
			*getRolesData.pDataStructure = roleData;
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<HTTPHandler> pHttpHandler, PostMessageData postMessageData) {
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			httpPOSTData postData;
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.content = postMessageData.content;
			workloadDataNew.workloadType = WorkloadType::POST;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::MESSAGE_CREATE;
			json jsonValue = pHttpHandler->getPOSTData(workloadDataNew);
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
			/*
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			httpPOSTData postData;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(3).scheduler;
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.content = postMessageData.content;
			workloadDataNew.workloadType = WorkloadType::POST;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::MESSAGE_CREATE;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
			*/
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, PutEmojiData putEmojiData){
			string relativePath = "/channels/" + putEmojiData.channelId + "/messages/" + putEmojiData.messageId + "/reactions/" + putEmojiData.emoji + "/@me";
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = WorkloadType::PUT;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(4).scheduler;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteMessageData deleteMessageData) {
			string relativePath = "/channels/" + deleteMessageData.channelId + "/messages/" + deleteMessageData.messageId;
			httpDELETEData deleteData;
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::MESSAGE_DELETE;
			if (deleteMessageData.timeDelay > 0) {
				unbounded_buffer<int> buffer0;
				timer<int> timer(deleteMessageData.timeDelay, 1, &buffer0, false);
				unbounded_buffer<WorkloadData> buffer1;
				unbounded_buffer<HTTPData> buffer2;
				Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
				com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
				com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
				requestSender->setWorkloadData(workloadDataNew);
				requestSender->start();
				HttpHandler->start();
				agent::wait(HttpHandler.get());
				agent::wait(requestSender.get());
				json jsonValue = requestSender->getData();
			}
			else {
				unbounded_buffer<WorkloadData> buffer1;
				unbounded_buffer<HTTPData> buffer2;
				Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
				com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
				com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
				requestSender->setWorkloadData(workloadDataNew);;
				requestSender->start();
				HttpHandler->start();
				agent::wait(HttpHandler.get());
				agent::wait(requestSender.get());
				json jsonValue = requestSender->getData();
			}
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI,  DeleteReactionData deleteReactionData){
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji + "/" + deleteReactionData.userId;
			httpDELETEData deleteData;
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI,  DeleteOwnReactionData deleteOwnReactionData) {
			string relativePath = "/channels/" + deleteOwnReactionData.channelId + "/messages/" + deleteOwnReactionData.messageId + "/reactions/" + deleteOwnReactionData.encodedEmoji + "/@me";
			httpDELETEData deleteData;
			WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			unbounded_buffer<WorkloadData> buffer1;
			unbounded_buffer<HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HttpHandler> HttpHandler = make_self<HttpAgents::HttpHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			HttpHandler->start();
			agent::wait(HttpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			co_return;
		}
	}
};
#endif
