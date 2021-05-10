﻿// DataManipFunctions.hpp - Header for the data manipulation functions.
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
#include "SystemThreads.hpp"
#include "HttpAgents.hpp"

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
			unsigned int timeDelay = 0;
		};

		IAsyncAction getObjectDataAsync(com_ptr<HTTPController> pHttpController, GetSelfUserData getUserSelfData) {
			string relativePath = "/users/@me";
			ClientDataTypes::UserData userData = *getUserSelfData.pDataStructure;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::GET;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::USER_SELF_GET;
			com_ptr<HTTPController> httpController = make_self<HTTPController>(pHttpController->pSystemThreads);
			send(&httpController->getBuffer00, workloadDataNew);
			exception e;
			if (httpController->getRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (httpController->getHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			agent::wait(httpController->getRequestSender.get());
			agent::wait(httpController->getHTTPHandler.get());
			json jsonValue = receive(&httpController->getBuffer03).data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserSelfData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<HTTPController> pHttpController, GetGuildData getGuildData) {
			string relativePath = "/guilds/" + getGuildData.id;
			ClientDataTypes::GuildData guildData = *getGuildData.pDataStructure;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::GUILD_GET;
			com_ptr<HTTPController> httpController = make_self<HTTPController>(pHttpController->pSystemThreads);
			send(&httpController->getBuffer00, workloadData);
			exception e;
			if (httpController->getRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (httpController->getHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			agent::wait(httpController->getRequestSender.get());
			agent::wait(httpController->getHTTPHandler.get());
			json jsonValue = receive(httpController->getBuffer03).data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*getGuildData.pDataStructure = guildData;
			co_return;
		}

		/*
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetUserData getUserData) {
			ClientDataTypes::UserData userData = *getUserData.pDataStructure;
			string relativePath = "/users/" + getUserData.id;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::USER_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*getUserData.pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildData getGuildData){
			ClientDataTypes::GuildData guildData = *getGuildData.pDataStructure;
			string relativePath = "/guilds/" + getGuildData.id;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::GUILD_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*getGuildData.pDataStructure = guildData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetChannelData getChannelData) {
			ClientDataTypes::ChannelData channelData = *getChannelData.pDataStructure;
			string relativePath = "/channels/" + getChannelData.id;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::CHANNEL_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*getChannelData.pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetGuildMemberData getGuildMemberData) {
			ClientDataTypes::GuildMemberData guildMemberData = *getGuildMemberData.pDataStructure;
			string relativePath = "/guilds/" + getGuildMemberData.guildId + "/members/" + getGuildMemberData.id;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::GUILD_MEMBER_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*getGuildMemberData.pDataStructure = guildMemberData;
			co_return;
		}
		*/
		IAsyncAction getObjectDataAsync(com_ptr<HTTPController> pHttpController, GetMessageData getMessageData) {
			string relativePath = "/channels/" + getMessageData.channelId + "/messages/" + getMessageData.id;
			ClientDataTypes::MessageData messageData = *getMessageData.pDataStructure;
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = RateLimitType::MESSAGE_GET;
			send(&pHttpController->getBuffer00, workloadData);
			exception e;
			if (pHttpController->getRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (pHttpController->getHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			json jsonValue = receive(&pHttpController->getBuffer03).data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*getMessageData.pDataStructure = messageData;
			co_return;
		}
		/*
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetRolesData getRolesData) {
			vector<ClientDataTypes::RoleData> roleData = *getRolesData.pDataStructure;
			string relativePath = "/guilds/" + getRolesData.guildId + "/roles";
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::GET;
			workloadData.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::ROLES_GET;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadData);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
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
			co_return;
			}
			*getRolesData.pDataStructure = roleData
		}
		*/

		IAsyncAction postObjectDataAsync(com_ptr<HTTPController> pHttpController, PostMessageData postMessageData) {
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.content = postMessageData.content;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::POST;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::MESSAGE_CREATE;
			send(&pHttpController->postBuffer00, workloadDataNew);
			exception e;
			if (pHttpController->postRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (pHttpController->postHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			json jsonValue = receive(&pHttpController->postBuffer03).data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
		}

		/*
		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, PostMessageData postMessageData) {
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			httpPOSTData postData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(3).scheduler;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.content = postMessageData.content;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::POST;
			workloadDataNew.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::MESSAGE_CREATE;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, PostMessageData postMessageData) {
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			httpPOSTData postData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(3).scheduler;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.content = postMessageData.content;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::POST;
			workloadDataNew.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::MESSAGE_CREATE;
			com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender->setWorkloadData(workloadDataNew);
			requestSender->start();
			httpHandler->start();
			agent::wait(httpHandler.get());
			agent::wait(requestSender.get());
			json jsonValue = requestSender->getData();
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
		}
		*/
		IAsyncAction putObjectDataAsync(com_ptr<HTTPController> pHttpController, PutEmojiData putEmojiData){
			string relativePath = "/channels/" + putEmojiData.channelId + "/messages/" + putEmojiData.messageId + "/reactions/" + putEmojiData.emoji + "/@me";
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::PUT;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			com_ptr<HTTPController> httpController = make_self<HTTPController>(pHttpController->pSystemThreads);
			send(&httpController->putBuffer00, workloadDataNew);
			exception e;
			if (httpController->putRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (httpController->putHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			agent::wait(httpController->putRequestSender.get());
			agent::wait(httpController->putHTTPHandler.get());
			json jsonValue = receive(httpController->putBuffer03).data;
			co_return;
		}
		/*
		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteMessageData deleteMessageData) {
			string relativePath = "/channels/" + deleteMessageData.channelId + "/messages/" + deleteMessageData.messageId;
			httpDELETEData deleteData;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = FoundationClasses::RateLimitType::MESSAGE_DELETE;
			if (deleteMessageData.timeDelay > 0) {
				unbounded_buffer<int> buffer0;
				timer<int> timer(deleteMessageData.timeDelay, 1, &buffer0, false);
				unbounded_buffer<HttpAgents::WorkloadData> buffer1;
				unbounded_buffer<HttpAgents::HTTPData> buffer2;
				Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
				com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
				com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
				requestSender->setWorkloadData(workloadDataNew);
				requestSender->start();
				httpHandler->start();
				agent::wait(httpHandler.get());
				agent::wait(requestSender.get());
				json jsonValue = requestSender->getData();
			}
			else {
				unbounded_buffer<HttpAgents::WorkloadData> buffer1;
				unbounded_buffer<HttpAgents::HTTPData> buffer2;
				Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(5).scheduler;
				com_ptr<HttpAgents::RequestSender>requestSender = make_self<HttpAgents::RequestSender>(pScheduler2, buffer1, buffer2);
				com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pScheduler2, pRestAPI, buffer2, buffer1);
				requestSender->setWorkloadData(workloadDataNew);;
				requestSender->start();
				httpHandler->start();
				agent::wait(httpHandler.get());
				agent::wait(requestSender.get());
				json jsonValue = requestSender->getData();
			}
			co_return;
		}
		*/
		IAsyncAction deleteObjectDataAsync(com_ptr<HTTPController> pHttpController,  DeleteReactionData deleteReactionData){
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji + "/" + deleteReactionData.userId;
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			com_ptr<HTTPController> httpController = make_self<HTTPController>(pHttpController->pSystemThreads);
			send(&httpController->deleteBuffer00, workloadDataNew);
			exception e;
			if (httpController->deleteRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (httpController->deleteHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			agent::wait(httpController->deleteRequestSender.get());
			agent::wait(httpController->deleteHTTPHandler.get());
			json jsonValue = receive(httpController->deleteBuffer03).data;
			co_return;
		}
		
		IAsyncAction deleteObjectDataAsync(com_ptr<HTTPController> pHttpController,  DeleteOwnReactionData deleteOwnReactionData) {
			string relativePath = "/channels/" + deleteOwnReactionData.channelId + "/messages/" + deleteOwnReactionData.messageId + "/reactions/" + deleteOwnReactionData.encodedEmoji + "/@me";
			HttpAgents::WorkloadData workloadDataNew;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::DELETED;
			workloadDataNew.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			com_ptr<HTTPController> httpController = make_self<HTTPController>(pHttpController->pSystemThreads);
			send(&httpController->deleteBuffer00, workloadDataNew);
			exception e;
			if (httpController->deleteRequestSender->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			if (httpController->deleteHTTPHandler->getError(e)) {
				cout << "Agent Error: " << e.what() << endl;
			}
			agent::wait(httpController->deleteRequestSender.get());
			agent::wait(httpController->deleteHTTPHandler.get());
			json jsonValue = receive(httpController->deleteBuffer03).data;
			co_return;
		}
	};
		
};
#endif
