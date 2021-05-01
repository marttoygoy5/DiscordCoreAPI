// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "HelperFunctions.hpp"
#include "ClientDataTypes.hpp"

namespace CommanderNS {

	namespace JSONifier {

		static std::string getIdentifyPayload(std::string botToken, int intents) {
			nlohmann::json data;
			data = {
					{"op", 2},
						{"d", {
						{"token", botToken},
						{"properties", {
						{"$os", "Windows"},
						{"$browser", "MyBotFW"},
						{"$device", "MyBotFW"}
					}},
					{"compress", false},
					{"large_threshold", 250},
					{"shard", {0, 1}},
					{"intents", intents}
					}}
			};
			return data.dump();
		};

		static std::string getHeartbeatPayload(int lastReceivedNumber) {
			nlohmann::json data;
			data = {
				{"d", lastReceivedNumber},
				{"op", 1}
			};
			return data.dump();
		};

		static std::string getResumePayload(std::string botToken, std::string sessionID, int lastReceivedNumber) {
			nlohmann::json data;
			data = {
				{"op", 6},
				{"d" , {
					{"token", botToken},
					{"session_id" , sessionID},
					{"seq" , lastReceivedNumber}
				}}
			};
			return data.dump();
		}
		
		std::string parseSocketPath(hstring initialPayload) {
			std::string finalString = "";
			nlohmann::json jsonVal;
			jsonVal = jsonVal.parse(winrt::to_string(initialPayload));
			if (jsonVal.contains("url")) {
				finalString = jsonVal.at("url");
			}
			return finalString;
		}

		int parseHeartBeatInterval(hstring initialPayload) {
			int finalValue = 0;
			nlohmann::json jsonVal;
			jsonVal = jsonVal.parse(winrt::to_string(initialPayload));
			if (jsonVal.contains("heartbeat_interval")) {
				finalValue = jsonVal.at("heartbeat_interval");
			}
			return finalValue;
		};

		std::string getCreateMessagePayload(ClientDataTypes::CreateMessageData createMessageData) {
			json data;
			//data = data.parse(createMessageData);

			return data.dump();
		}
	}
}
