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
#include "SystemThreads.hpp"
#include "HttpAgents.hpp"

namespace CommanderNS {

	namespace DataManipFunctions {

		IAsyncAction checkRateLimitAndGetDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pRateLimitData, string relativePath, httpGETData* pGetDataStruct) {
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
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
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

		IAsyncAction checkRateLimitAndPostDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pRateLimitData, string relativePath, httpPOSTData* pPostDataStruct, string content) {
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
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
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

		IAsyncAction checkRateLimitAndPutDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pRateLimitData, string relativePath, httpPUTData* pPutDataStruct, string content) {
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
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
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

		IAsyncAction checkRateLimitAndDeleteDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pRateLimitData, string relativePath, httpDELETEData* pDeleteDataStruct) {
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
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					while (timeRemaining > 0.0f) {
						currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
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

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pUserGetRateLimit, ClientDataTypes::UserData* pDataStructure) {
			ClientDataTypes::UserData userData = *pDataStructure;
			string relativePath = "/users/@me";
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pUserGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pUserGetRateLimit, string id, ClientDataTypes::UserData* pDataStructure) {
			ClientDataTypes::UserData userData = *pDataStructure;
			string relativePath = "/users/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pUserGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pGuildGetRateLimit, string id, ClientDataTypes::GuildData* pDataStructure) {
			ClientDataTypes::GuildData guildData = *pDataStructure;
			string relativePath = "/guilds/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*pDataStructure = guildData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pChannelGetRateLimit, string id, ClientDataTypes::ChannelData* pDataStructure) {
			ClientDataTypes::ChannelData channelData = *pDataStructure;
			string relativePath = "/channels/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pChannelGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pGuildMemberGetRateLimit, string guildId, string id, ClientDataTypes::GuildMemberData* pDataStructure) {
			ClientDataTypes::GuildMemberData guildMemberData = *pDataStructure;
			string relativePath = "/guilds/" + guildId + "/members/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildMemberGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*pDataStructure = guildMemberData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pMessageGetRateLimit, string channelId, string id, ClientDataTypes::MessageData* pDataStructure) {
			ClientDataTypes::MessageData messageData = *pDataStructure;
			string relativePath = "/channels/" + channelId + "/messages/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pMessageGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*pDataStructure = messageData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pRoleGetRateLimit, string id, vector<ClientDataTypes::RoleData>* pDataStructure) {
			vector<ClientDataTypes::RoleData> roleData = *pDataStructure;
			string relativePath = "/guilds/" + id + "/roles";
			httpGETData getData;
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

		IAsyncAction postObjectDataAsyncBACKUP(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pMessagePostRateLimit, string channelId, ClientDataTypes::MessageData* pDataStructure, string content) {
			ClientDataTypes::MessageData messageData = *pDataStructure;
			string relativePath = "/channels/" + channelId + "/messages";
			httpPOSTData postData;
			checkRateLimitAndPostDataAsync(pRestAPI, pMessagePostRateLimit, relativePath, &postData, content).get();
			json jsonValue = postData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*pDataStructure = messageData;
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, Scheduler* pScheduler, string channelId, ClientDataTypes::MessageData* pDataStructure, HttpAgents::WorkloadData workloadData) {
			string relativePath = "/channels/" + channelId + "/messages";
			ClientDataTypes::MessageData messageData = *pDataStructure;
			httpPOSTData postData;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			HttpAgents::WorkloadData workloadDataNew = workloadData;
			HttpAgents::RequestSender requestSender(pScheduler2, buffer1, buffer2);
			HttpAgents::HTTPHandler httpHandler(pScheduler2, pRestAPI, buffer2, buffer1);
			workloadDataNew.relativeURL = relativePath;
			requestSender.setWorkloadData(workloadDataNew);
			requestSender.start();
			httpHandler.start();
			agent::wait(&httpHandler);
			agent::wait(&requestSender);
			json jsonValue = requestSender.getData();
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*pDataStructure = messageData;
			co_return;
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler, Scheduler* pScheduler,  string channelId, string messageId, string emoji, HttpAgents::WorkloadData workloadData){
			string relativePath = "/channels/" + channelId + "/messages/" + messageId + "/reactions/" + emoji + "/@me";
			HttpAgents::WorkloadData workloadDataNew = workloadData;
			workloadDataNew.relativeURL = relativePath;
			workloadDataNew.workloadType = HttpAgents::WorkloadType::PUT;
			unbounded_buffer<HttpAgents::WorkloadData> buffer1;
			unbounded_buffer<HttpAgents::HTTPData> buffer2;
			Scheduler* pScheduler2 = pRestAPI->pSystemThreads->Threads.at(2).scheduler;
			HttpAgents::RequestSender requestSender(pScheduler2, buffer1, buffer2);
			HttpAgents::HTTPHandler httpHandler(pScheduler2, pRestAPI, buffer2, buffer1);
			requestSender.setWorkloadData(workloadDataNew);
			requestSender.start();
			httpHandler.start();
			agent::wait(&httpHandler);
			agent::wait(&requestSender);
			json jsonValue = requestSender.getData();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pMessageDeleteRateLimit, string channelId, string messageId) {
			string relativePath = "/channels/" + channelId + "/messages/" + messageId;
			httpDELETEData deleteData;
			checkRateLimitAndDeleteDataAsync(pRestAPI, pMessageDeleteRateLimit, relativePath, &deleteData).get();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pReactionDeleteRateLimit, ClientDataTypes::DeleteReactionData deleteReactionData){
			string relativePath = "/channels/" + deleteReactionData.channelId + "/messages/" + deleteReactionData.messageId + "/reactions/" + deleteReactionData.encodedEmoji+ "/" + deleteReactionData.userId;
			httpDELETEData deleteData;
			checkRateLimitAndDeleteDataAsync(pRestAPI, pReactionDeleteRateLimit, relativePath, &deleteData).get();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitData* pReactionDeleteRateLimit, ClientDataTypes::DeleteOwnReactionData deletionData) {
			string relativePath = "/channels/" + deletionData.channelId + "/messages/" + deletionData.messageId + "/reactions/" + deletionData.encodedEmoji + "/@me";
			httpDELETEData deleteData;
			checkRateLimitAndDeleteDataAsync(pRestAPI, pReactionDeleteRateLimit, relativePath, &deleteData).get();
			co_return;
		}
	}
};
#endif