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

    string convertToLowerCase(string stringToConvert) {
        string newString;
        for (auto& value : stringToConvert) {
            if (isupper(value)) {
                newString += (char)tolower(value);
            }
            else {
                newString += value;
            }
        }
        return newString;
    }

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
            msgEmbed.setAuthor(message.data.interaction.user.username, message.data.interaction.user.getAvatarURL());
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
        if (guildData.gameChannelIds.size() > 0) {
            isItFound = false;
            string msgString = "------\n**Sorry, but please do that in one of the following channels:**\n------\n";
            DiscordCoreInternal::EmbedData msgEmbed;
            for (auto& value : guildData.gameChannelIds) {
                if (message.data.channelId == value) {
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

    class PermissionsConverter {
    public:

        static bool checkForPresence(DiscordCoreInternal::Permissions permission, string permissionString) {
            if (permissionString == "") {
                return false;
            }
            __int64 permissionsInteger = stoll(permissionString);
            if ((permissionsInteger & (__int64)permission) == (__int64)permission) {
                return true;
            }
            else {
                return false;
            }
        }

        static string addPermissionsToString(string originalPermissionString, DiscordCoreInternal::Permissions permissionsToAdd[], int quantityOfPermsToAdd) {
            if (originalPermissionString == "") {
                originalPermissionString = "0";
            }
            __int64 permissionsInteger = stoll(originalPermissionString);
            for (unsigned int x = 0; x < (unsigned int)quantityOfPermsToAdd; x += 1) {
                permissionsInteger = permissionsInteger | (__int64)permissionsToAdd[x];
            }
            stringstream sstream;
            sstream << permissionsInteger;
            return sstream.str();
        }

        static string removePermissionsFromString(string originalPermissionString, DiscordCoreInternal::Permissions permissionsToRemove[], int quantityOfPermsToRemove) {
            if (originalPermissionString == "") {
                originalPermissionString = "0";
            }
            __int64 permissionsInteger = stoll(originalPermissionString);
            for (unsigned int x = 0; x < (unsigned int)quantityOfPermsToRemove; x += 1) {
                permissionsInteger = permissionsInteger & ~(__int64)permissionsToRemove[x];
            }
            stringstream sstream;
            sstream << permissionsInteger;
            return sstream.str();
        }

        static void displayPermissions(string permissionString) {
            if (permissionString == "") {
                permissionString = "0";
            }
            __int64 permissionsInteger = stoll(permissionString);
            stringstream sstream;
            if (permissionsInteger & (1 << 0)) {
                sstream << "CREATE_INSTANT_INVITE" << endl;
            }
            if (permissionsInteger & (1 << 1)) {
                sstream << "KICK_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 2)) {
                sstream << "BAN_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 3)) {
                sstream << "ADMINISTRATOR" << endl;
            }
            if (permissionsInteger & (1 << 4)) {
                sstream << "MANAGE_CHANNELS" << endl;
            }
            if (permissionsInteger & (1 << 5)) {
                sstream << "MANAGE_GUILD" << endl;
            }
            if (permissionsInteger & (1 << 6)) {
                sstream << "ADD_REACTIONS" << endl;
            }
            if (permissionsInteger & (1 << 7)) {
                sstream << "VIEW_AUDIT_LOG" << endl;
            }
            if (permissionsInteger & (1 << 8)) {
                sstream << "PRIORITY_SPEAKER" << endl;
            }
            if (permissionsInteger & (1 << 9)) {
                sstream << "STREAM" << endl;
            }
            if (permissionsInteger & (1 << 10)) {
                sstream << "VIEW_CHANNEL" << endl;
            }
            if (permissionsInteger & (1 << 11)) {
                sstream << "SEND_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 12)) {
                sstream << "SEND_TTS_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 13)) {
                sstream << "MANAGE_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 14)) {
                sstream << "EMBED_LINKS" << endl;
            }
            if (permissionsInteger & (1 << 15)) {
                sstream << "ATTACH_FILES" << endl;
            }
            if (permissionsInteger & (1 << 16)) {
                sstream << "READ_MESSAGE_HISTORY" << endl;
            }
            if (permissionsInteger & (1 << 17)) {
                sstream << "MENTION_EVERYONE" << endl;
            }
            if (permissionsInteger & (1 << 18)) {
                sstream << "USE_EXTERNAL_EMOJI" << endl;
            }
            if (permissionsInteger & (1 << 19)) {
                sstream << "VIEW_GUILD_INSIGHTS" << endl;
            }
            if (permissionsInteger & (1 << 20)) {
                sstream << "CONNECT" << endl;
            }
            if (permissionsInteger & (1 << 21)) {
                sstream << "SPEAK" << endl;
            }
            if (permissionsInteger & (1 << 22)) {
                sstream << "MUTE_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 23)) {
                sstream << "DEAFEN_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 24)) {
                sstream << "MOVE_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 25)) {
                sstream << "USE_VAD" << endl;
            }
            if (permissionsInteger & (1 << 26)) {
                sstream << "CHANGE_NICKNAME" << endl;
            }
            if (permissionsInteger & (1 << 27)) {
                sstream << "MANAGE_NICKNAMES" << endl;
            }
            if (permissionsInteger & (1 << 28)) {
                sstream << "MANAGE_ROLES" << endl;
            }
            if (permissionsInteger & (1 << 29)) {
                sstream << "MANAGE_WEBHOOKS" << endl;
            }
            if (permissionsInteger & (1 << 30)) {
                sstream << "MAANGE_EMOJIS" << endl;
            }
            if (permissionsInteger & (1 << 31)) {
                sstream << "USE_SLASH_COMMANDS" << endl;
            }
            if (permissionsInteger & (1ull << 32)) {
                sstream << "REQUEST_TO_SPEAK" << endl;
            }
            if (permissionsInteger & (1ull << 34)) {
                sstream << "MANAGE_THREADS" << endl;
            }
            if (permissionsInteger & (1ull << 35)) {
                sstream << "USE_PUBLIC_THREADS" << endl;
            }
            if (permissionsInteger & (1ull << 36)) {
                sstream << "USE_PRIVATE_THREADS" << endl;
            }
            cout << "PERMISSIONS: " << endl << sstream.str() << endl;
        }

        static string allPermissions() {
            __int64 allPerms;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ADD_REACTIONS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ATTACH_FILES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::BAN_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CHANGE_NICKNAME;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CONNECT;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CREATE_INSTANT_INVITE;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::DEAFEN_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::EMBED_LINKS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::KICK_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_CHANNELS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_EMOJIS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_GUILD;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_NICKNAMES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_ROLES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_WEBHOOKS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MENTION_EVERYONE;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MOVE_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MUTE_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::PRIORITY_SPEAKER;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::READ_MESSAGE_HISTORY;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::REQUEST_TO_SPEAK;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SEND_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SEND_TTS_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SPEAK;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::STREAM;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_EXTERNAL_EMOJIS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_PRIVATE_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_PUBLIC_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_SLASH_COMMANDS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_VAD;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_AUDIT_LOG;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_CHANNEL;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_GUILD_INSIGHTS;
            stringstream stream;
            stream << allPerms;
            return stream.str();
        }

        static string computeBasePermissions(GuildMember guildMember, GuildManager guilds, RoleManager roles) {
            Guild guild = guilds.getGuildAsync({ guildMember.data.guildId }).get();

            if (guild.data.ownerID == guildMember.data.user.id) {
                return allPermissions();
            }

            Role everyone = roles.getRoleAsync({ .guildId = guild.data.id, .roleId = guild.data.id }).get();
            string permissionsString = everyone.data.permissions;
            __int64 permissionsInt = stoll(permissionsString);

            for (auto& role : guildMember.data.roles) {
                Role currentRole = roles.getRoleAsync({ .guildId = guild.data.id, .roleId = role }).get();
                permissionsInt |= stoll(currentRole.data.permissions);
            }

            if ((permissionsInt & (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) == (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) {
                return allPermissions();
            }
            stringstream stream;
            stream << permissionsInt;
            return stream.str();
        }

        static string  computeOverwrites(string basePermissions, GuildMember guildMember, Channel channel) {
            __int64 permissionsInt = stoll(basePermissions);
            if ((permissionsInt & (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) == (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) {
                return allPermissions();
            }

            Guild guild = guildMember.coreClient->guilds->getGuildAsync({ .guildId = guildMember.data.guildId }).get();
                
            DiscordCoreInternal::OverWriteData overwriteEveryone = channel.data.permissionOverwrites.at(guild.data.id);

            if (overwriteEveryone.id != "") {
                permissionsInt &= ~stoll(overwriteEveryone.deny);
                permissionsInt |= stoll(overwriteEveryone.allow);
                }

            map<string, DiscordCoreInternal::OverWriteData> overWrites = channel.data.permissionOverwrites;
            __int64 allow = 0;
            __int64 deny = 0;
            for (auto& role : guildMember.data.roles) {
                Role currentRole = guildMember.coreClient->roles->getRoleAsync({ .guildId = guildMember.data.guildId, .roleId = role }).get();
                if (overWrites.contains(currentRole.data.id)) {
                    allow |= stoll(overWrites.at(currentRole.data.id).allow);
                    deny |= stoll(overWrites.at(currentRole.data.id).deny);
                }
            }
            permissionsInt &= ~deny;
            permissionsInt |= allow;
            
            if (overWrites.contains(guildMember.data.user.id)) {
                permissionsInt &= ~stoll(overWrites.at(guildMember.data.user.id).deny);
                permissionsInt |= stoll(overWrites.at(guildMember.data.user.id).allow);
            }

            stringstream stream;
            stream << permissionsInt;
            return stream.str();
        } 

        static string computePermissions(GuildMember guildMember, Channel channel) {
            string permissions;
            permissions = computeBasePermissions(guildMember, *guildMember.coreClient->guilds, *guildMember.coreClient->roles);
            permissions = computeOverwrites(permissions, guildMember, channel);
            return permissions;
        }

        static bool checkForPermission(GuildMember guildMember, Channel channel, DiscordCoreInternal::Permissions permission) {
            string permissionsString = computePermissions(guildMember, channel);
            if ((stoll(permissionsString) & (__int64)permission) == (__int64)permission) {
                return true;
            }
            else {
                return false;
            }
        }

    };

    bool checkForBotCommanderStatus(GuildMember guildMember, DiscordUser discordUser) {
        bool areWeACommander;
        for (auto& value : discordUser.data.botCommanders) {
            if (guildMember.data.user.id == value) {
                areWeACommander = true;
                return areWeACommander;
                break;
            }
        }
        return false;
    }

    bool doWeHaveAdminPermissions(Message message) {
        Guild guild = message.coreClient->guilds->getGuildAsync({ .guildId = message.data.guildId }).get();
        DiscordGuild discordGuild = message.coreClient->getDiscordGuild(guild.data);

        GuildMember guildMember = message.coreClient->guildMembers->getGuildMemberAsync({ .guildId = message.data.guildId, .guildMemberId = message.data.author.id }).get();

        Channel channel = message.coreClient->channels->getChannelAsync({ message.data.channelId }).get();

        bool doWeHaveAdmin = PermissionsConverter::checkForPermission(guildMember, channel, DiscordCoreInternal::Permissions::ADMINISTRATOR);

        if (doWeHaveAdmin) {
            return true;
        }

        bool areWeACommander = checkForBotCommanderStatus(guildMember, *message.coreClient->discordUser);

        if (areWeACommander) {
            return true;
        }

        string msgString = "------\n**Sorry, but you don't have the permissions required for that!**\n------";
        DiscordCoreInternal::EmbedData msgEmbed;
        msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
        msgEmbed.setColor(discordGuild.data.borderColor[0], discordGuild.data.borderColor[1], discordGuild.data.borderColor[2]);
        msgEmbed.setDescription(msgString);
        msgEmbed.setTimeStamp(getTimeAndDate());
        msgEmbed.setTitle("__**Permissions Issue:**__");
        Message msg = message.coreClient->messages->replyAsync({ .replyingToMessageData = message.data, .embed = msgEmbed }).get();
        message.coreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id,.timeDelay = 20000 }).get();
        return false;
    }
}
#endif