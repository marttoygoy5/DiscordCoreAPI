// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELPER_FUNCTIONS_
#define _HELPER_FUNCTIONS_

#include "pch.h"
#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

    string getTimeAndDate() {
        const std::time_t now = std::time(nullptr);
        char charArray[32];
        std::tm time;
        localtime_s(&time, &now);
        strftime(charArray, 32, "%F %R", &time);
        return charArray;
    }

	bool areWeInADM(Message message, Channel channel) {
        auto currentChannelType = channel.data.type;

        if (currentChannelType == DiscordCoreInternal::ChannelType::DM) {
            string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
            DiscordCoreInternal::EmbedData msgEmbed;
            msgEmbed.setAuthor(message.data.author.username, message.data.author.getAvatarURL());
            msgEmbed.setColor(254, 254, 254);
            msgEmbed.setDescription(msgString);
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setTitle("__**Direct Message Issue:**__");
            ReplyMessageData messageReplyData;
            messageReplyData.replyingToMessageData = message.data;
            messageReplyData.embed = msgEmbed;
            Message msg = message.coreClient->messages->replyAsync(messageReplyData).get();
            message.coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 20000 }).get();
            return true;
        }
        return false;
	}

	bool checkIfAllowedGamingInChannel(Message message, DiscordGuildData guildData) {
        bool isItFound = true;
        if (guildData.gameChannelIds.size()> 0) {
            isItFound = false;
            string msgString = "------\n**Sorry, but please do that in one of the following channels:**\n------\n";
            DiscordCoreInternal::EmbedData msgEmbed;
            for (auto& value: guildData.gameChannelIds) {
                if (message.data.channelId ==  value) {
                    isItFound = true;
                    break;
                }
                else {
                    msgString += "<#" + value + ">\n";
                }
            }
            msgString += "------";
            if (isItFound == false) {
                msgEmbed
                    .setAuthor(message.data.author.username, message.data.author.getAvatarURL());
                msgEmbed.setColor(254, 254, 254);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTitle("__**Permissions Issue:**__");
                ReplyMessageData replyMessageData;
                replyMessageData.replyingToMessageData = message.data;
                replyMessageData.embed = msgEmbed;
                Message messageNew = message.coreClient->messages->replyAsync(replyMessageData).get();
                DeleteMessageData deleteMsgData;
                deleteMsgData.channelId = messageNew.data.channelId;
                deleteMsgData.messageId = messageNew.data.id;
                deleteMsgData.timeDelay = 20000;
                message.coreClient->messages->deleteMessageAsync(deleteMsgData).get();
                return isItFound;
            }
        }
        return isItFound;

	}
}
#endif
