// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _JSONIFIER_
#define _JSONIFIER_

#include "pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	string getIdentifyPayload(string botToken, int intents) {
		json data;
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

	string getHeartbeatPayload(int lastReceivedNumber) {
		json data;
		data = {
			{"d", lastReceivedNumber},
			{"op", 1}
		};
		return data.dump();
	};

	string getResumePayload(string botToken, string sessionID, int lastReceivedNumber) {
		json data;
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

	string parseSocketPath(hstring initialPayload) {
		string finalString = "";
		json jsonVal;
		jsonVal = jsonVal.parse(to_string(initialPayload));
		if (jsonVal.contains("url")) {
			finalString = jsonVal.at("url");
		}
		return finalString;
	};

	int parseHeartBeatInterval(hstring initialPayload) {
		json jsonVal;
		int finalValue = 0;
		jsonVal = jsonVal.parse(to_string(initialPayload));
		if (jsonVal.contains("heartbeat_interval")) {
			finalValue = jsonVal.at("heartbeat_interval");
		}
		return finalValue;
	};

	string getCreateMessagePayload(CreateMessageData createMessageData) {
		
		auto componentsActionRow = json::array();

		for (auto& value : createMessageData.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.emoji.id == "") {
					json component = { {"custom_id", valueNew.customId},
					{"disabled", valueNew.disabled},
					{"emoji",{
						{"name", valueNew.emoji.name},
					{"animated", valueNew.emoji.animated}
				} },
					{"label", valueNew.label},
					{"style", valueNew.style},
					{"type", valueNew.type},
					{"url", valueNew.url}
					};
					components.push_back(component);
				}
				else {
					json component = { {"custom_id", valueNew.customId},
					{"disabled", valueNew.disabled},
					{"emoji",{
						{"name", valueNew.emoji.name},
						{"id", valueNew.emoji.id},
					{"animated", valueNew.emoji.animated}
				} },
					{"label", valueNew.label},
					{"style", valueNew.style},
					{"type", valueNew.type},
					{"url", valueNew.url}
					};
					components.push_back(component);
				}
			}
			json componentActionRow = { {"type", 1},{
				"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}
		
		auto fields = json::array();

		for (unsigned int x = 0; x < createMessageData.embed.fields.size(); x += 1) {
			json field = { {"inline", createMessageData.embed.fields.at(x).Inline},
							{"value", createMessageData.embed.fields.at(x).value},
							{"name", createMessageData.embed.fields.at(x).name} };
			fields.push_back(field);
		}

		int colorValue = createMessageData.embed.actualColor();
		if (createMessageData.embed.description != "" || createMessageData.embed.fields.size() != 0){
			if (createMessageData.messageReference.guildId != "") {
				json data = {
		{"allowed_mentions", {
			{"parse", createMessageData.allowedMentions.parse},
			{"replied_user", createMessageData.allowedMentions.repliedUser},
			{"roles", createMessageData.allowedMentions.roles},
			{"users", createMessageData.allowedMentions.users}
			}},
			{"message_reference",{{"message_id", createMessageData.messageReference.messageId},
			{"channel_id", createMessageData.messageReference.channelId},
			{"guild_id", createMessageData.messageReference.guildId},
			{"fail_if_not_exists", createMessageData.messageReference.failIfNotExists}
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
				{"color",colorValue},
					{"timestamp", createMessageData.embed.timestamp},
		}},{"components", componentsActionRow}
				};
				return data.dump();
			}
				else {
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
			{"color",colorValue},
					{"timestamp", createMessageData.embed.timestamp},

	}},{"components", componentsActionRow}
				};
				return data.dump();
			}
		}
		else {
			if (createMessageData.messageReference.guildId != "") {
				json data = {
			{"allowed_mentions", {
				{"parse", createMessageData.allowedMentions.parse},
				{"replied_user", createMessageData.allowedMentions.repliedUser},
				{"roles", createMessageData.allowedMentions.roles},
				{"users", createMessageData.allowedMentions.users}
				}},
				{"message_reference",{{"message_id", createMessageData.messageReference.messageId},
				{"channel_id", createMessageData.messageReference.channelId},
				{"guild_id", createMessageData.messageReference.guildId},
				{"fail_if_not_exists", createMessageData.messageReference.failIfNotExists}
					}},
			{"content", createMessageData.content},
			{"tts" , createMessageData.tts},
					{"components", componentsActionRow}
				};
				return data.dump();
			}
			else {
				json data = {
			{"allowed_mentions", {
				{"parse", createMessageData.allowedMentions.parse},
				{"replied_user", createMessageData.allowedMentions.repliedUser},
				{"roles", createMessageData.allowedMentions.roles},
				{"users", createMessageData.allowedMentions.users}
				}},
			{"content", createMessageData.content},
			{"tts" , createMessageData.tts},
				{"components", componentsActionRow}
				};
				return data.dump();
			}
			
		}
		
	}

	string getEditMessagePayload(EditMessageData editMessageData) {
		auto fields = json::array();

		for (unsigned int x = 0; x < editMessageData.embed.fields.size(); x += 1) {
			json field = { {"inline", editMessageData.embed.fields.at(x).Inline},
							{"value", editMessageData.embed.fields.at(x).value},
							{"name", editMessageData.embed.fields.at(x).name} };
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

		int colorValue = editMessageData.embed.actualColor();

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
				{"color",colorValue},
			{"timestamp", editMessageData.embed.timestamp}
		}}
		};

		return data.dump();
	}

	string getModifyRolePayload(UpdateRoleData modifyRoleData) {
		modifyRoleData.actualColor();

		json data = {
			{"color", modifyRoleData.color},
			{"hoist", modifyRoleData.hoist},
			{"mendtionable", modifyRoleData.mentionable},
			{"name", modifyRoleData.name},
			{"permissions", modifyRoleData.permissions}
		};

		return data.dump();
	}

	void addOptionsData(ApplicationCommandOptionData appCommandOptionData, json* pJSONData) {
		json newOption;
		newOption.emplace(make_pair("description", appCommandOptionData.description));
		newOption.emplace(make_pair("name", appCommandOptionData.name));
		newOption.emplace(make_pair("required", appCommandOptionData.required));
		newOption.emplace(make_pair("type", appCommandOptionData.type));
		json dataArray;
		newOption.emplace(make_pair("options", dataArray));
		newOption.emplace(make_pair("choices", dataArray));
		if (appCommandOptionData.choices.size() > 0) {
			for (unsigned int y = 0; y < appCommandOptionData.choices.size(); y += 1) {
				ApplicationCommandOptionChoiceData choiceData;
				choiceData.name = appCommandOptionData.choices.at(y).name;
				if (appCommandOptionData.choices.at(y).valueString != "") {
					choiceData.valueString = appCommandOptionData.choices.at(y).valueString;
					json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueString} };
					newOption.at("choices").emplace_back(jsonValue);
				}
				else if (appCommandOptionData.choices.at(y).valueInt != 0) {
					choiceData.valueInt = appCommandOptionData.choices.at(y).valueInt;
					json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueInt} };
					newOption.at("choices").emplace_back(jsonValue);
				}
			}
		}
		if (appCommandOptionData.options.size() > 0) {
			addOptionsData(appCommandOptionData.options.at(0), &newOption.at("options"));
		}
		pJSONData->emplace_back(newOption);
	}

	string getCreateApplicationCommandPayload(CreateApplicationCommandData createApplicationCommandData) {
		json data = {
			{"name",createApplicationCommandData.name },
			{"description", createApplicationCommandData.description},
			{"default_permission", createApplicationCommandData.defaultPermission}
		};

		json arrayValue;

		data.emplace(make_pair("options", arrayValue));

		if (createApplicationCommandData.options.size() > 0) {
			for (unsigned int x = 0; x < createApplicationCommandData.options.size(); x += 1) {
				addOptionsData(createApplicationCommandData.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}

	string getEditApplicationCommandPayload(EditApplicationCommandData editApplicationCommandData) {
		json data = {
			{"name",editApplicationCommandData.name },
			{"description", editApplicationCommandData.description},
			{"default_permission", editApplicationCommandData.defaultPermission}
		};

		json arrayValue;

		data.emplace(make_pair("options", arrayValue));

		if (editApplicationCommandData.options.size() > 0) {
			for (unsigned int x = 0; x < editApplicationCommandData.options.size(); x += 1) {
				addOptionsData(editApplicationCommandData.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}

	string getInteractionResponsePayload(InteractionResponseFullData interactionData) {

		auto parseArray = json::array();
		for (auto& value : interactionData.interactionResponseData.data.allowedMentions.parse) {
			parseArray.push_back(value);
		};

		auto rolesArray = json::array();
		for (auto& value : interactionData.interactionResponseData.data.allowedMentions.roles) {
			rolesArray.push_back(value);
		}

		auto usersArray = json::array();
		for (auto& value : interactionData.interactionResponseData.data.allowedMentions.users) {
			usersArray.push_back(value);
		}

		auto embedsArray = json::array();
		for (auto& value : interactionData.interactionResponseData.data.embeds) {

			auto fields = json::array();

			for (unsigned int x = 0; x < value.fields.size(); x += 1) {
				json field = { {"inline", value.fields.at(x).Inline},
								{"value", value.fields.at(x).value},
								{"name", value.fields.at(x).name} };
				fields.push_back(field);
			}

			int colorValue = value.actualColor();

			json embed = { "embed",
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",colorValue },
				{"timestamp", value.timestamp}} };

			embedsArray.push_back(embed);
		}

		json data = { { "type", interactionData.interactionResponseData.type }, { "data",{
			{"tts", interactionData.interactionResponseData.data.tts},
			{"content", interactionData.interactionResponseData.data.content},
			{"flags", interactionData.interactionResponseData.data.flags},
			{"embeds", embedsArray},
			{"allowed_mentions", {
				{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", interactionData.interactionResponseData.data.allowedMentions.repliedUser}
		}}}
		}

		};
		return data.dump();
	}		

		string getCreateInteractionPayload(InteractionResponseData interactionData) {
			auto embedsArray = json::array();

			for (auto& value : interactionData.data.embeds) {

				auto fields = json::array();

				for (auto& value2 : value.fields) {
					json field =
					{ {"inline", value2.Inline},
								{"value", value2.value},
								{"name", value2.name} };
					fields.push_back(field);
				}

				int colorValue = value.actualColor();

				json embed =
				{ { "author", {
			   {"icon_url", value.author.iconUrl},
			   {"name", value.author.name},
			   {"url", value.author.url },
			   {"proxy_icon_url", value.author.proxyIconUrl}
	   } },
				{ "image", {
					{"height", value.image.height},
					{"width", value.image.width},
					{"url", value.image.url},
					{"proxy_url",value.image.proxyUrl}
			} },
				{ "provider" , {
					{"name", value.provider.name},
					{"url", value.provider.url}
			} },
				{ "thumbnail", {
					{"height", value.thumbnail.height},
					{"width", value.thumbnail.width},
					{"url", value.thumbnail.url},
					{"proxy_url", value.thumbnail.proxyUrl}
				} },
				{ "footer", {
					{"icon_url", value.footer.iconUrl},
					{"proxy_icon_url", value.footer.proxyIconUrl},
					{"text", value.footer.text}
		} },
				{ "description" , value.description },
				{ "title", value.title },
				{ "fields", fields },
				{ "color",colorValue },
					{"timestamp", value.timestamp}
				};
				embedsArray.push_back(embed);
			}
				
			auto parseArray = json::array();
			for (auto& value : interactionData.data.allowedMentions.parse) {
				parseArray.push_back(value);
			};

			auto rolesArray = json::array();
			for (auto& value : interactionData.data.allowedMentions.roles) {
				rolesArray.push_back(value);
			}

			auto usersArray = json::array();
			for (auto& value : interactionData.data.allowedMentions.users) {
				usersArray.push_back(value);
			}

			auto componentsActionRow = json::array();

			for (auto& value : interactionData.data.components) {
				auto components = json::array();

				for (auto& valueNew : value.components) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				json componentActionRow = { {"type", 1},{
					"components", components} };
				componentsActionRow.push_back(componentActionRow);
			}

			if (interactionData.data.content == "") {
				json data = {
					{"type", interactionData.type},
					{"tts", interactionData.data.tts},
					{"flags", interactionData.data.flags},
				{ "embeds", embedsArray },
				{ "allowed_mentions", {
				{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", interactionData.data.allowedMentions.repliedUser},

		} }, {"components", componentsActionRow}
				};
				return data.dump();
			}
			else {
				json data = { {"content", interactionData.data.content},
					{"type", interactionData.type},
					{"tts", interactionData.data.tts},
					{"flags", interactionData.data.flags},
				{"embeds", embedsArray},
				{"allowed_mentions", {
				{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", interactionData.data.allowedMentions.repliedUser}

		} }, {"components", componentsActionRow} };
				return data.dump();
			};
		 }

		 string getCreateRolePayload(CreateRoleData createRoleData) {
			 unsigned int roleColorInt = stol(createRoleData.color, 0, 16);
			 stringstream stream;
			 stream << setbase(10) << roleColorInt;
			 string roleColorReal = stream.str();

			 json data = { {"color", roleColorReal

				 },
				 {"hoist", createRoleData.hoist},{"permissions", createRoleData.permissions},
				 {"mentionable", createRoleData.mentionable},
				 {"name", createRoleData.name }
			 };
			 return data.dump();
		 };
}
#endif
