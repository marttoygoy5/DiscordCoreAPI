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

namespace CommanderNS {

	namespace DataManipFunctions {

		IAsyncAction checkRateLimitAndGetDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpGETData* pGetDataStruct) {
			//co_await resume_background();
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pGetDataStruct = pRestAPI->httpGETObjectData(relativePath, pRateLimitData);

					if (pGetDataStruct->data.contains("message") && !pGetDataStruct->data.at("message").is_null()) {
						string theValue = pGetDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
				else {
					int currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;

					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					}
					*pGetDataStruct = pRestAPI->httpGETObjectData(relativePath, pRateLimitData);
					if (pGetDataStruct->data.contains("message") && !pGetDataStruct->data.at("message").is_null()) {
						string theValue = pGetDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
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

		IAsyncAction checkRateLimitAndPutDataAsyncTest(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpPUTData* pPostDataStruct, string content) {
			try {
				if (pRateLimitData->getsRemaining > 0) {
					
					cout << "THREAD ID 00: " << this_thread::get_id() << endl;
					RestAPI::theTask(pRestAPI.get(), relativePath, content, pRateLimitData).then([pRateLimitData](httpPUTData putData) ->void {
						pRateLimitData->currentMsTime = putData.currentMsTime;
						pRateLimitData->getsRemaining = putData.getsRemaining;
						pRateLimitData->msRemain = putData.msRemain;
						cout << "THREAD ID 03: " << this_thread::get_id() << endl;
						}, task_continuation_context::use_default());
					cout << "THREAD ID 04: " << this_thread::get_id() << endl;
					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
				else {
					int currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;

					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					}
					RestAPI::theTask(pRestAPI.get(), relativePath, content, pRateLimitData).then([pRateLimitData](httpPUTData putData) ->void {
						pRateLimitData->currentMsTime = putData.currentMsTime;
						pRateLimitData->getsRemaining = putData.getsRemaining;
						pRateLimitData->msRemain = putData.msRemain;
						}, task_continuation_context::use_default());
					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
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

		IAsyncAction checkRateLimitAndPostDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpPOSTData* pPostDataStruct, string content) {
			//co_await resume_background();
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pPostDataStruct = pRestAPI->httpPOSTObjectData(relativePath, content, pRateLimitData);

					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
				else {
					int currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;

					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					}
					*pPostDataStruct = pRestAPI->httpPOSTObjectData(relativePath, content, pRateLimitData);
					if (pPostDataStruct->data.contains("message") && !pPostDataStruct->data.at("message").is_null()) {
						string theValue = pPostDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
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

		IAsyncAction checkRateLimitAndPutDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpPUTData* pPutDataStruct, string content) {
			//co_await resume_background();
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pPutDataStruct = pRestAPI->httpPUTObjectData(relativePath, content, pRateLimitData);

					if (pPutDataStruct->data.contains("message") && !pPutDataStruct->data.at("message").is_null()) {
						string theValue = pPutDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
				else {
					int currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					}
					*pPutDataStruct = pRestAPI->httpPUTObjectData(relativePath, content, pRateLimitData);
					if (pPutDataStruct->data.contains("message") && !pPutDataStruct->data.at("message").is_null()) {
						string theValue = pPutDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
			}
			catch (exception error) {
				if (error.what() == "Unknown exception") {
					co_return;
				}
				cout << "putObjectDataAsync() Issue: " << error.what() << endl;
				co_return;
			}
		}

		IAsyncAction checkRateLimitAndDeleteDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpDELETEData* pDeleteDataStruct) {
			//co_await resume_background();
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pDeleteDataStruct = pRestAPI->httpDELETEObjectData(relativePath, pRateLimitData);
					if (pDeleteDataStruct->data.contains("message") && !pDeleteDataStruct->data.at("message").is_null()) {
						string theValue = pDeleteDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
					}
					co_return;
				}
				else {
					int currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					
					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					}
					*pDeleteDataStruct = pRestAPI->httpDELETEObjectData(relativePath, pRateLimitData);
					if (pDeleteDataStruct->data.contains("message") && !pDeleteDataStruct->data.at("message").is_null()) {
						string theValue = pDeleteDataStruct->data.at("message");
						exception error(theValue.c_str());
						throw error;
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

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pUserGetRateLimit, string id, ClientDataTypes::UserData* pDataStructure) {
			//co_await resume_background();
			ClientDataTypes::UserData userData = *pDataStructure;
			string relativePath = "/users/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pUserGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pGuildGetRateLimit, string id, ClientDataTypes::GuildData* pDataStructure) {
			//co_await resume_background();
			ClientDataTypes::GuildData guildData = *pDataStructure;
			string relativePath = "/guilds/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*pDataStructure = guildData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pGuildGetRateLimit, string id, ClientDataTypes::ChannelData* pDataStructure) {
			//co_await resume_background();
			ClientDataTypes::ChannelData channelData = *pDataStructure;
			string relativePath = "/channels/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pGuildMemberGetRateLimit, string guildId, string id, ClientDataTypes::GuildMemberData* pDataStructure) {
			//co_await resume_background();
			ClientDataTypes::GuildMemberData guildMemberData = *pDataStructure;
			string relativePath = "/guilds/" + guildId + "/members/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildMemberGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*pDataStructure = guildMemberData;
			co_return;

		}
		
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRoleGetRateLimit, string id, vector<ClientDataTypes::RoleData>* pDataStructure) {
			//co_await resume_background();
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

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pMessagePostRateLimit, string channelId, ClientDataTypes::MessageData* pDataStructure, string content) {
			//co_await resume_background();
			ClientDataTypes::MessageData messageData = *pDataStructure;
			string relativePath = "/channels/" + channelId + "/messages";
			httpPOSTData postData;
			checkRateLimitAndPostDataAsync(pRestAPI, pMessagePostRateLimit, relativePath, &postData, content).get();
			json jsonValue = postData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*pDataStructure = messageData;
			co_return;
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pReactionPostRateLimit, string channelId, string messageId, string emoji) {
			//co_await resume_background();
			string relativePath = "/channels/" + channelId + "/messages/" + messageId + "/reactions/" + emoji + "/@me";
			httpPUTData putData;
			checkRateLimitAndPutDataAsync(pRestAPI, pReactionPostRateLimit, relativePath, &putData, emoji).get();
			co_return;
		}

		IAsyncAction putObjectDataAsyncTest(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pReactionPostRateLimit, string channelId, string messageId, string emoji) {
			//co_await resume_background();
			string relativePath = "/channels/" + channelId + "/messages/" + messageId + "/reactions/" + emoji + "/@me";
			httpPUTData putData;
			checkRateLimitAndPutDataAsyncTest(pRestAPI, pReactionPostRateLimit, relativePath, &putData, emoji).get();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pMessageDeleteRateLimit, string channelId, string messageId) {
			//co_await resume_background();
			string relativePath = "/channels/" + channelId + "/messages/" + messageId;
			httpDELETEData deleteData;
			checkRateLimitAndDeleteDataAsync(pRestAPI, pMessageDeleteRateLimit, relativePath, &deleteData).get();
			co_return;
		}
	}
};
#endif