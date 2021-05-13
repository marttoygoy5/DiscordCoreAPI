// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _JSONIFIER_
#define _JSONIFIER_

#include "pch.h"
#include "DiscordDataStructs.hpp"

namespace DiscordCoreInternal {

	namespace JSONifier {

		static string getIdentifyPayload(std::string botToken, int intents) {
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

		static string getHeartbeatPayload(int lastReceivedNumber) {
			nlohmann::json data;
			data = {
				{"d", lastReceivedNumber},
				{"op", 1}
			};
			return data.dump();
		};

		static string getResumePayload(std::string botToken, std::string sessionID, int lastReceivedNumber) {
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

		string getCreateMessagePayload(CreateMessageData createMessageData) {
			auto fields = json::array();
			
			for (unsigned int x = 0; x < createMessageData.embed.fields.size(); x += 1) {
				json field = { {"inline", createMessageData.embed.fields.at(x).Inline},
								{"value", createMessageData.embed.fields.at(x).value},
								{"name", createMessageData.embed.fields.at(x).name} };
				fields.push_back(field);
			}
			
			int colorValue = createMessageData.embed.actualColor();

			json data = {
				{"allowed_mentions", {
					{"parse", createMessageData.allowedMentions.parse},
					{"replied_user", createMessageData.allowedMentions.repliedUser},
					{"roles", createMessageData.allowedMentions.roles},
					{"users", createMessageData.allowedMentions.users}
					}},
				{"content", createMessageData.content},
				{"tts" , createMessageData.tts},
				{"embed" , 
							{{"author", {
							{"icon_url", createMessageData.embed.author.iconUrl},
							{"name", createMessageData.embed.author.name},
							{"url", createMessageData.embed.author.url },
							{"proxy_icon_url", createMessageData.embed.author.proxyIconUrl}
					}}, 
					{"image", {
						{"height", createMessageData.embed.image.height},
						{"width", createMessageData.embed.image.width},
						{"url", createMessageData.embed.image.url},
						{"proxy_url",createMessageData.embed.image.proxyUrl}
			}},
				{ "provider" , {
					{"name", createMessageData.embed.provider.name},
					{"url", createMessageData.embed.provider.url}

			}},
				{"thumbnail", {
					{"height", createMessageData.embed.thumbnail.height},
					{"width", createMessageData.embed.thumbnail.width},
					{"url", createMessageData.embed.thumbnail.url},
					{"proxy_url", createMessageData.embed.thumbnail.proxyUrl}
				}},

					
					{"footer", {
						{"icon_url", createMessageData.embed.footer.iconUrl},
						{"proxy_icon_url", createMessageData.embed.footer.proxyIconUrl},
						{"text", createMessageData.embed.footer.text}
			}},
					{"title", createMessageData.embed.title},
					{"description" , createMessageData.embed.description},
					{"fields", fields},
					{"color",colorValue}
			}} 
			};

			return data.dump();
		}

		string getEditMessagePayload(EditMessageData editMessageData) {
			auto fields = json::array();

			for (unsigned int x = 0; x < editMessageData.embed.fields.size(); x += 1) {
				json field = { {"inline", editMessageData.embed.fields.at(x).Inline},
								{"value", editMessageData.embed.fields.at(x).value},
								{"name", editMessageData.embed.fields.at(x).name} };
				cout << "THIS IS ANOTHER MESSAGE FIELD!" << endl;
				fields.push_back(field);
			}

			auto attachments = json::array();

			for (unsigned int x = 0; x < editMessageData.attachments.size(); x += 1) {
				json attachment = { {"content_type", editMessageData.attachments.at(x).contentType},
					{"file_name",editMessageData.attachments.at(x).filename},
					{"height",editMessageData.attachments.at(x).height},
					{"id",editMessageData.attachments.at(x).id},
					{"proxy_url",editMessageData.attachments.at(x).proxyUrl},
					{"size",editMessageData.attachments.at(x).size},
					{"url",editMessageData.attachments.at(x).url},
					{"width",editMessageData.attachments.at(x).width} };
				attachments.push_back(attachment);
			}

			int colorValue = editMessageData.embed.actualColorVal;

			json data = {
				{"flags", editMessageData.flags},
				{"attachments", attachments},
				{"allowed_mentions", {
					{"parse", editMessageData.allowedMentions.parse},
					{"replied_user", editMessageData.allowedMentions.repliedUser},
					{"roles", editMessageData.allowedMentions.roles},
					{"users", editMessageData.allowedMentions.users}
					}},
				{"content", editMessageData.content},
				{"embed" ,
							{{"author", {
							{"icon_url", editMessageData.embed.author.iconUrl},
							{"name", editMessageData.embed.author.name},
							{"url", editMessageData.embed.author.url },
							{"proxy_icon_url", editMessageData.embed.author.proxyIconUrl}
					}},
					{"image", {
						{"height", editMessageData.embed.image.height},
						{"width", editMessageData.embed.image.width},
						{"url", editMessageData.embed.image.url},
						{"proxy_url",editMessageData.embed.image.proxyUrl}
			}},
				{ "provider" , {
					{"name", editMessageData.embed.provider.name},
					{"url", editMessageData.embed.provider.url}

			}},
				{"thumbnail", {
					{"height", editMessageData.embed.thumbnail.height},
					{"width", editMessageData.embed.thumbnail.width},
					{"url", editMessageData.embed.thumbnail.url},
					{"proxy_url", editMessageData.embed.thumbnail.proxyUrl}
				}},


					{"footer", {
						{"icon_url", editMessageData.embed.footer.iconUrl},
						{"proxy_icon_url", editMessageData.embed.footer.proxyIconUrl},
						{"text", editMessageData.embed.footer.text}
			}},
					{"description" , editMessageData.embed.description},
					{"title", editMessageData.embed.title},
					{"fields", fields},
					{"color",colorValue}
			}}
			};

			return data.dump();
		}
	}
}
#endif