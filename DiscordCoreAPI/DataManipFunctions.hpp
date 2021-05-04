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

namespace CommanderNS {

	namespace DataManipFunctions {

		IAsyncAction checkRateLimitAndGetDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpGETData* pGetDataStruct) {
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
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;

					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
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

		IAsyncAction checkRateLimitAndPostDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData, string relativePath, httpPOSTData* pPostDataStruct, string content) {
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
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
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
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
					while (timeRemaining > 0) {

						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
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
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
					while (timeRemaining > 0) {
						currentTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(static_cast<float>(currentTime) - static_cast<float>(pRateLimitData->currentMsTime))) / 1000;
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
			critical_section critSection;
			critSection.try_lock_for(pUserGetRateLimit->msRemain + 50);
			ClientDataTypes::UserData userData = *pDataStructure;
			string relativePath = "/users/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pUserGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*pDataStructure = userData;
			critSection.unlock();
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pGuildGetRateLimit, string id, ClientDataTypes::GuildData* pDataStructure) {
			critical_section critSection;
			critSection.try_lock_for(pGuildGetRateLimit->msRemain + 50);
			ClientDataTypes::GuildData guildData = *pDataStructure;
			string relativePath = "/guilds/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*pDataStructure = guildData;
			critSection.unlock();
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pChannelGetRateLimit, string id, ClientDataTypes::ChannelData* pDataStructure) {
			critical_section critSection;
			critSection.try_lock_for(pChannelGetRateLimit->msRemain + 50);
			ClientDataTypes::ChannelData channelData = *pDataStructure; 
			string relativePath = "/channels/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pChannelGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*pDataStructure = channelData;
			critSection.unlock();
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pGuildMemberGetRateLimit, string guildId, string id, ClientDataTypes::GuildMemberData* pDataStructure) {
			critical_section critSection;
			critSection.try_lock_for(pGuildMemberGetRateLimit->msRemain + 50);
			ClientDataTypes::GuildMemberData guildMemberData = *pDataStructure;
			string relativePath = "/guilds/" + guildId + "/members/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildMemberGetRateLimit, relativePath, &getData).get();
			json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*pDataStructure = guildMemberData;
			critSection.unlock();
			co_return;
		}
		
		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pRoleGetRateLimit, string id, vector<ClientDataTypes::RoleData>* pDataStructure) {
			critical_section critSection;
			critSection.try_lock_for(pRoleGetRateLimit->msRemain + 50);
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
			critSection.unlock();
			co_return;
		}

		IAsyncAction postObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pMessagePostRateLimit, string channelId, ClientDataTypes::MessageData* pDataStructure, string content) {
			critical_section critSection;
			critSection.try_lock_for(pMessagePostRateLimit->msRemain + 50);
			ClientDataTypes::MessageData messageData = *pDataStructure;
			string relativePath = "/channels/" + channelId + "/messages";
			httpPOSTData postData;
			checkRateLimitAndPostDataAsync(pRestAPI, pMessagePostRateLimit, relativePath, &postData, content).get();
			cout << pMessagePostRateLimit->currentMsTime << endl;
			cout << pMessagePostRateLimit->msRemain << endl;
			cout << pMessagePostRateLimit->getsRemaining << endl;
			json jsonValue = postData.data;
			DataParsingFunctions::parseObject(jsonValue, &messageData);
			*pDataStructure = messageData;
			critSection.unlock();
			co_return;
		}

		IAsyncAction putObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pReactionPostRateLimit, string channelId, string messageId, string emoji) {
			critical_section critSection;
			critSection.try_lock_for(pReactionPostRateLimit->msRemain + 50);
			string relativePath = "/channels/" + channelId + "/messages/" + messageId + "/reactions/" + emoji + "/@me";
			httpPUTData putData;
			checkRateLimitAndPutDataAsync(pRestAPI, pReactionPostRateLimit, relativePath, &putData, emoji).get();
			critSection.unlock();
			co_return;
		}

		IAsyncAction deleteObjectDataAsync(com_ptr<RestAPI> pRestAPI, shared_ptr<FoundationClasses::RateLimitation> pMessageDeleteRateLimit, string channelId, string messageId) {
			critical_section critSection;
			critSection.try_lock_for(pMessageDeleteRateLimit->msRemain + 50);
			string relativePath = "/channels/" + channelId + "/messages/" + messageId;
			httpDELETEData deleteData;
			checkRateLimitAndDeleteDataAsync(pRestAPI, pMessageDeleteRateLimit, relativePath, &deleteData).get();
			critSection.unlock();
			co_return;
		}
	}
};
#endif