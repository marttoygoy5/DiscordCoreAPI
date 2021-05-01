// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
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
		};

		std::string parseSocketPath(hstring initialPayload) {
			std::string finalString = "";
			nlohmann::json jsonVal;
			jsonVal = jsonVal.parse(winrt::to_string(initialPayload));
			if (jsonVal.contains("url")) {
				finalString = jsonVal.at("url");
			}
			return finalString;
		};

		int parseHeartBeatInterval(hstring initialPayload) {
			nlohmann::json jsonVal;
			int finalValue = 0;
			jsonVal = jsonVal.parse(winrt::to_string(initialPayload));
			if (jsonVal.contains("heartbeat_interval")) {
				finalValue = jsonVal.at("heartbeat_interval");
			}
			return finalValue;
		};

		std::string getCreateMessagePayload(ClientDataTypes::CreateMessageData createMessageData) {
			json data;
			auto fields = json::array();
			
			for (unsigned int x = 0; x < createMessageData.embed.fields.size(); x += 1) {
				json object = { {"inline", createMessageData.embed.fields.at(x).Inline},
								{"value", createMessageData.embed.fields.at(x).value},
								{"name", createMessageData.embed.fields.at(x).name} };
				fields.push_back(object);
			}

			data = {
				{"content", createMessageData.content},
				{"tts" , false},
				{"embed" , {
					{"title", "Hello, Embed!"},
				{"description" , "This is an embedded message."},
				{"fields", fields},
				{"color",createMessageData.embed.actualColor()}
			}}
			};

			return data.dump();
		}
	}
}
