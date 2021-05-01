// DataManipFunctions.hpp - Header for the data manipulation functions.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "RestAPI.hpp"
#include "FoundationClasses.hpp"
#include "DataParsingFunctions.hpp"
#include "ClientDataTypes.hpp"

namespace CommanderNS {

	namespace DataManipFunctions {

		IAsyncAction checkRateLimitAndGetDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pRateLimitData, std::string relativePath, httpGETData* pGetDataStruct) {
			co_await resume_background();
			try {
				if (pRateLimitData->getsRemaining > 0) {
					*pGetDataStruct = pRestAPI->httpGETObjectData(relativePath);
					if (pGetDataStruct->getsRemaining >= 0) {
						pRateLimitData->getsRemaining = pGetDataStruct->getsRemaining;
					}
					if (pGetDataStruct->msRemain >= 0) {
						pRateLimitData->msRemain = pGetDataStruct->msRemain;
					}
					pRateLimitData->currentMsTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
					if (pGetDataStruct->data.contains("message")) {
						std::string theValue = pGetDataStruct->data.at("message");
						std::exception error(theValue.c_str());
						throw error;
					}
				}
				else {
					int currentTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = (static_cast<float>(pRateLimitData->msRemain) - static_cast<float>(currentTime - pRateLimitData->currentMsTime)) / 1000;
					std::cout << "Time remaining until we can make another get request: " << timeRemaining << " seconds." << std::endl;
					if (timeRemaining <= 0) {
						pRateLimitData->getsRemaining = 1;
					}
					co_return;
				}
			}
			catch (std::exception error) {
				std::cout << "getObjectDataAsync() Issue: " << error.what() << std::endl;
				co_return;
			}
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pUserGetRateLimit, std::string id, ClientDataTypes::UserData* pDataStructure) {
			co_await resume_background();
			ClientDataTypes::UserData userData = *pDataStructure;
			std::string relativePath = "/users/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pUserGetRateLimit, relativePath, &getData).get();
			nlohmann::json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &userData);
			*pDataStructure = userData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pGuildGetRateLimit, std::string id, ClientDataTypes::GuildData* pDataStructure) {
			co_await resume_background();
			ClientDataTypes::GuildData guildData = *pDataStructure;
			std::string relativePath = "/guilds/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			nlohmann::json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildData);
			*pDataStructure = guildData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pGuildGetRateLimit, std::string id, ClientDataTypes::ChannelData* pDataStructure) {
			co_await resume_background();
			ClientDataTypes::ChannelData channelData = *pDataStructure;
			std::string relativePath = "/channels/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildGetRateLimit, relativePath, &getData).get();
			nlohmann::json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &channelData);
			*pDataStructure = channelData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pGuildMemberGetRateLimit, std::string guildId, std::string id, ClientDataTypes::GuildMemberData* pDataStructure) {
			co_await resume_background();
			ClientDataTypes::GuildMemberData guildMemberData = *pDataStructure;
			string relativePath = "/guilds/" + guildId + "/members/" + id;
			httpGETData getData;
			checkRateLimitAndGetDataAsync(pRestAPI, pGuildMemberGetRateLimit, relativePath, &getData).get();
			nlohmann::json jsonValue = getData.data;
			DataParsingFunctions::parseObject(jsonValue, &guildMemberData);
			*pDataStructure = guildMemberData;
			co_return;
		}

		IAsyncAction getObjectDataAsync(com_ptr<RestAPI> pRestAPI, FoundationClasses::RateLimitation* pRoleGetRateLimit, std::string id, std::vector<ClientDataTypes::RoleData>* pDataStructure) {
			co_await resume_background();
			ClientDataTypes::GuildData guildData;
			std::string relativePath;
			relativePath = "/guilds/" + id + "/roles";
			httpGETData getData;
			co_await checkRateLimitAndGetDataAsync(pRestAPI, pRoleGetRateLimit, relativePath, &getData);
			nlohmann::json jsonValue = getData.data;
			for (unsigned int x = 0; x < jsonValue.size(); x += 1) {
				bool isItFound = false;
				for (unsigned int y = 0; y < guildData.roles.size(); y += 1) {
					if (guildData.roles.at(y).id == jsonValue.at(x).at("id")) {
						isItFound = true;
						ClientDataTypes::RoleData roleData = guildData.roles.at(y);
						DataParsingFunctions::parseObject(jsonValue.at(x), &roleData);
						guildData.roles.erase(guildData.roles.begin() + y);
						guildData.roles.push_back(roleData);
					}
				}
				if (isItFound == false) {
					ClientDataTypes::RoleData roleData;
					DataParsingFunctions::parseObject(jsonValue.at(x), &roleData);
					guildData.roles.push_back(roleData);
				}
			}
			*pDataStructure = guildData.roles;
			co_return;
		}
	}
}
