// Botinfo.hpp - Header for the "bot info" command.
// May 31, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BOT_INFO_
#define _BOT_INFO_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class BotInfo : public BaseFunction {
	public:
		BotInfo() {
			this->commandName = "botinfo";
            this->helpDescription = "__**Bot Info:**__ !botinfo = BOTNAME or /botinfo, to view info about this bot!";
		}

		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
            Channel channel = args->coreClient->channels->getChannelAsync({ args->message.data.channelId }).get();

            bool areWeInADm = areWeInADM(args->message, channel);

            if (areWeInADm == true) {
                co_return;
            }

            Guild guild = args->coreClient->guilds->getGuildAsync({ .guildId = args->message.data.guildId }).get();
            DiscordGuild discordGuild(guild.data);

            if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::SLASH_COMMAND_INTERACTION) {
                args->coreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
            }

            if (args->argumentsArray.size() == 0 || (args->argumentsArray.at(0)!= "janny" && args->argumentsArray.at(0)!=  "musichouse" && args->argumentsArray.at(0) !=  "gamehouse")) {
                string msgString = "------\n**Please, enter the name of a bot as the first argument! (!displayguildsdata = BOTNAME)**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Invalid Or Missing Arguments:**__");
                Message msg = args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed, }).get();
                args->coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 200000 }).get();
                co_return;
            }
            if (args->argumentsArray.at(0) != "gamehouse") {
                co_return;
            }

            vector<EmbedFieldData> fields;
            EmbedFieldData field1{ "__Bot Name:__", args->coreClient->discordUser->data.userName,true };
            fields.push_back(field1);
            EmbedFieldData field2{ "__Bot ID:__",args->coreClient->discordUser->data.userId, true };
            fields.push_back(field2);
            EmbedFieldData field3{ "__Guild Count:__", to_string(args->coreClient->discordUser->data.guildCount),true };
            fields.push_back(field3);
            EmbedFieldData field4{ "__Currency Name:__",args->coreClient->discordUser->data.currencyName,true };
            fields.push_back(field4);

            EmbedData msgEmbed2;
            msgEmbed2.setImage(args->coreClient->currentUser->data.getAvatarURL());
            msgEmbed2.setColor(discordGuild.data.borderColor);
            msgEmbed2.setTitle("__**Bot Info:**__");
            msgEmbed2.setTimeStamp(getTimeAndDate());
            msgEmbed2.fields = fields;
            args->coreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed2 }).get();
            co_return;
		}
	};
    BotInfo botInfo{};
}
#endif