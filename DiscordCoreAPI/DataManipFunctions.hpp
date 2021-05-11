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

		IAsyncAction checkRateLimitAndGetDataAsync(com_ptr<RestAPI> pRestAPI, RateLimitData* pRateLimitData, string relativePath, HTTPData* pGetDataStruct) {
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pGetDataStruct = pRestAPI->httpGETObjectDataAsync(relativePath, pRateLimitData).get();
					if (pGetDataStruct->data.contains("message") && !pGetDataStruct->data.at("message").is_null()) {
						string theValue = pGetDataStruct->data.at("message");
						cout << "httpGETObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
				else {
					float targetTime = static_cast<float>(pRateLimitData->timeStartedAt) + static_cast<float> (pRateLimitData->msRemain);
					float currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = targetTime - currentTime;
					}
					*pGetDataStruct = pRestAPI->httpGETObjectDataAsync(relativePath, pRateLimitData).get();
					if (pGetDataStruct->data.contains("message") && !pGetDataStruct->data.at("message").is_null()) {
						string theValue = pGetDataStruct->data.at("message");
						cout << "httpGETObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
			}
			catch (exception error) {
				if (error.what() == "Unknown exception") {
					co_return;
				}
				cout << "getObjectDataAsync() Issue: " << error.what() << endl;
				co_return;
			}
		}

		IAsyncAction checkRateLimitAndPostDataAsync(com_ptr<RestAPI> pRestAPI, RateLimitData* pRateLimitData, string relativePath, HTTPData* pPostDataStruct, string content) {
			try {
				if (pRateLimitData->getsRemaining > 0) {

					*pPostDataStruct = pRestAPI->httpPOSTObjectDataAsync(relativePath, content, pRateLimitData).get();
					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						cout << "httpPOSTObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
				else {
					float targetTime = static_cast<float>(pRateLimitData->timeStartedAt) + static_cast<float> (pRateLimitData->msRemain);
					float currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = targetTime - currentTime;
					}
					*pPostDataStruct = pRestAPI->httpPOSTObjectDataAsync(relativePath, content, pRateLimitData).get();
					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						cout << "httpPOSTObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
			}
			catch (exception error) {
				if (error.what() == "Unknown exception") {
					co_return;
				}
				cout << "postObjectDataAsync() Issue: " << error.what() << endl;
				co_return;
			}
		}

		IAsyncAction checkRateLimitAndPutDataAsync(com_ptr<RestAPI> pRestAPI, RateLimitData* pRateLimitData, string relativePath, HTTPData* pPutDataStruct, string content) {
			try {
					*pPutDataStruct = pRestAPI->httpPUTObjectDataAsync(relativePath, content, pRateLimitData).get();
				if (pRateLimitData->getsRemaining > 0) {
					if (pPutDataStruct->data.contains("message") && !pPutDataStruct->data.at("message").is_null()) {
						string theValue = pPutDataStruct->data.at("message");
						cout << "httpPUTObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
				else {
					float targetTime = static_cast<float>(pRateLimitData->timeStartedAt) + static_cast<float> (pRateLimitData->msRemain);
					float currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = targetTime - currentTime;
					}
					*pPutDataStruct = pRestAPI->httpPUTObjectDataAsync(relativePath, content, pRateLimitData).get();
					if (pPutDataStruct->data.contains("message") && !pPutDataStruct->data.at("message").is_null()) {
						string theValue = pPutDataStruct->data.at("message");
						cout << "httpPUTObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
			}
			catch (exception error) {
				if (error.what() == "Unknown exception") {
				}
					co_return;
				cout << "putObjectDataAsync() Issue: " << error.what() << endl;
				co_return;
			}
		}

		IAsyncAction checkRateLimitAndDeleteDataAsync(com_ptr<RestAPI> pRestAPI, RateLimitData* pRateLimitData, string relativePath, HTTPData* pDeleteDataStruct) {
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pDeleteDataStruct = pRestAPI->httpDELETEObjectDataAsync(relativePath, pRateLimitData).get();
					if (pDeleteDataStruct->data.contains("message") && !pDeleteDataStruct->data.at("message").is_null()) {
						string theValue = pDeleteDataStruct->data.at("message");
						cout << "httpDELETEObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
				else {
					float targetTime = static_cast<float>(pRateLimitData->timeStartedAt) + static_cast<float> (pRateLimitData->msRemain);
					float currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = targetTime - currentTime;
					}
					*pDeleteDataStruct = pRestAPI->httpDELETEObjectDataAsync(relativePath, pRateLimitData).get();
					if (pDeleteDataStruct->data.contains("message") && !pDeleteDataStruct->data.at("message").is_null()) {
						string theValue = pDeleteDataStruct->data.at("message");
						cout << "httpDELETEObjectDataAsync() Error: " << theValue << endl;
						exception error(theValue.c_str());
					}
					co_return;
				}
			}
			catch (exception error) {
				if (error.what() == "Unknown exception") {
					co_return;
				}
				cout << "deleteObjectDataAsync() Issue: " << error.what() << endl;
				co_return;
			}
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetSelfUserData getUserData) {
			ClientDataTypes::UserData userData = *getUserData.pDataStructure;
			string relativePath = "/users/@me";
			HTTPData getData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
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
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
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
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
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
		/*
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetChannelData getChannelData) {
			ClientDataTypes::ChannelData channelData = *getChannelData.pDataStructure;
			string relativePath = "/channels/" + getChannelData.id;
			HTTPData getData;
			RateLimitData* pChannelGetRateLimit = new RateLimitData;
			pChannelGetRateLimit->getsRemaining = 1;
			pChannelGetRateLimit->msRemain = 250;
			checkRateLimitAndGetDataAsync(pRestAPI, pChannelGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*getChannelData.pDataStructure = channelData;
			co_return;
		}
		*/
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetChannelData getChannelData) {
			ClientDataTypes::ChannelData channelData = *getChannelData.pDataStructure;
			string relativePath = "/channels/" + getChannelData.id;
			HTTPData getData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
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
			RateLimitData* pGuildMemberGetRateLimit = new RateLimitData;
			pGuildMemberGetRateLimit->getsRemaining = 1;
			pGuildMemberGetRateLimit->msRemain = 250;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildMemberGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*getGuildMemberData.pDataStructure = guildMemberData;
			co_return;
		}
		/*
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetMessageData getMessageData) {
			ClientDataTypes::MessageData messageData = *getMessageData.pDataStructure;
			string relativePath = "/channels/" + getMessageData.channelId + "/messages/" + getMessageData.id;
			HTTPData getData;
			RateLimitData* pMessageGetRateLimit = new RateLimitData;
			pMessageGetRateLimit->getsRemaining = 1;
			pMessageGetRateLimit->msRemain = 250; 
			checkRateLimitAndGetDataAsync(pRestAPI, pMessageGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*getMessageData.pDataStructure = messageData;
			co_return;
		}
		*/
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, GetMessageData getMessageData) {
			ClientDataTypes::MessageData messageData = *getMessageData.pDataStructure;
			string relativePath = "/channels/" + getMessageData.channelId + "/messages/" + getMessageData.id;
			HTTPData getData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(2).scheduler, buffer02, buffer01, pRestAPI);
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

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, RateLimitData* pRoleGetRateLimit, string id, vector<ClientDataTypes::RoleData>* pDataStructure) {
			vector<ClientDataTypes::RoleData> roleData = *pDataStructure;
			string relativePath = "/guilds/" + id + "/roles";
			HTTPData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pRoleGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
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
			*pDataStructure = roleData;
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, PostMessageData postMessageData) {
			ClientDataTypes::MessageData messageData = *postMessageData.pDataStructure;
			string relativePath = "/channels/" + postMessageData.channelId + "/messages";
			HTTPData postData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
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
			workloadData.rateLimitData.rateLimitType = RateLimitType::CREATE_MESSAGE;
			workloadData.content = postMessageData.content;
			send(buffer00, workloadData);
			postData = receive(buffer03);
			json jsonValue = postData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*postMessageData.pDataStructure = messageData;
			co_return;
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, PutEmojiData putEmojiData){
			string relativePath = "/channels/" + putEmojiData.channelId + "/messages/" + putEmojiData.messageId + "/reactions/" + putEmojiData.emoji + "/@me";
			HTTPData deleteData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
			unbounded_buffer<HttpAgents::WorkloadData> buffer01;
			unbounded_buffer<HTTPData> buffer02;
			unbounded_buffer<HTTPData> buffer03;
			com_ptr<HttpAgents::RequestSender> requestSender = make_self<HttpAgents::RequestSender>(pRestAPI->pSystemThreads->Threads.at(4).scheduler, buffer00, buffer01, buffer02, buffer03);
			com_ptr<HttpAgents::HTTPHandler> httpHandler = make_self<HttpAgents::HTTPHandler>(pRestAPI->pSystemThreads->Threads.at(4).scheduler, buffer02, buffer01, pRestAPI);
			requestSender->start();
			httpHandler->start();
			HttpAgents::WorkloadData workloadData;
			workloadData.relativeURL = relativePath;
			workloadData.workloadType = HttpAgents::WorkloadType::PUT;
			workloadData.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI,  DeleteMessageData deleteMessageData) {
			string relativePath = "/channels/" + deleteMessageData.channelId + "/messages/" + deleteMessageData.messageId;
			HTTPData getData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
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
			workloadData.rateLimitData.rateLimitType = RateLimitType::DELETE_MESSAGE;
			send(buffer00, workloadData);
			getData = receive(buffer03);
			json jsonValue = getData.data;
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, DeleteReactionData deleteReactionData){
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji + "/" + deleteReactionData.userId;
			HTTPData deleteData;
			unbounded_buffer <HttpAgents::WorkloadData> buffer00;
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
			workloadData.rateLimitData.rateLimitType = RateLimitType::REACTION_ADD_REMOVE;
			send(buffer00, workloadData);
			deleteData = receive(buffer03);
			co_return;
		}
	}
};
#endif