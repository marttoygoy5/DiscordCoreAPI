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

    bool areWeInADM(InputEventData eventData, Channel channel, DiscordGuild discordGuild) {
        auto currentChannelType = channel.data.type;

        if (currentChannelType == ChannelType::DM) {
            string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
            EmbedData msgEmbed;
            msgEmbed.setAuthor(eventData.messageData.interaction.user.username, eventData.messageData.author.getAvatarURL());
            msgEmbed.setColor(discordGuild.data.borderColor);
            msgEmbed.setDescription(msgString);
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setTitle("__**Direct Message Issue:**__");
            if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
                responseData.channelId = eventData.getChannelId();
                responseData.messageId = eventData.getMessageId();
                responseData.embeds.push_back(msgEmbed);
                InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
                InputEventHandler::deleteInputEventResponse(event01,20000).get();
            }
            else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                InputEventData event(eventData.messageData, eventData.interactionData, InputEventType::SLASH_COMMAND_INTERACTION);
                InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
                responseData.applicationId = eventData.interactionData.applicationId;
                responseData.embeds.push_back(msgEmbed);
                responseData.interactionId = eventData.interactionData.id;
                responseData.interactionToken = eventData.interactionData.token;
                responseData.type = InteractionCallbackType::ChannelMessage;
                InputEventHandler::respondToEvent(responseData).get();
                InputEventHandler::deleteInputEventResponse(eventData, 20000).get();
            }
            return true;
        }
        return false;
    }

    bool checkIfAllowedGamingInChannel(DiscordCoreAPI::InputEventData eventData,  DiscordGuild discordGuild) {
        bool isItFound = true;
        if (discordGuild.data.gameChannelIds.size() > 0) {
            isItFound = false;
            string msgString = "------\n**Sorry, but please do that in one of the following channels:**\n------\n";
            EmbedData msgEmbed;
            for (auto& value : discordGuild.data.gameChannelIds) {
                if (eventData.getChannelId() == value) {
                    isItFound = true;
                    break;
                }
                else {
                    msgString += "<#" + value + ">\n";
                }
            }
            msgString += "------";
            if (isItFound == false) {
                msgEmbed.setAuthor(eventData.messageData.author.username, eventData.messageData.author.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTitle("__**Permissions Issue:**__");
                ReplyMessageData replyMessageData;
                replyMessageData.replyingToMessageData = eventData.messageData;
                replyMessageData.embed = msgEmbed;
                if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
                    responseData.channelId = eventData.messageData.channelId;
                    responseData.messageId = eventData.messageData.id;
                    responseData.embeds.push_back(msgEmbed);
                    InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
                    InputEventHandler::deleteInputEventResponse(event01, 20000).get();
                }
                else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
                    responseData.applicationId = eventData.interactionData.applicationId;
                    responseData.embeds.push_back(msgEmbed);
                    responseData.interactionId = eventData.interactionData.id;
                    responseData.interactionToken = eventData.interactionData.token;
                    responseData.type = InteractionCallbackType::ChannelMessage;
                    InputEventHandler::respondToEvent(responseData).get();
                    InputEventHandler::deleteInputEventResponse(eventData, 20000).get();
                }
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

        static string addPermissionsToString(string originalPermissionString, vector<Permissions> permissionsToAdd) {
            if (originalPermissionString == "") {
                originalPermissionString = "0";
            }
            __int64 permissionsInteger = stoll(originalPermissionString);
            for (unsigned int x = 0; x < permissionsToAdd.size(); x += 1) {
                permissionsInteger = permissionsInteger | (__int64)permissionsToAdd.at(x);
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
                sstream << "MANAGE_EMOJIS" << endl;
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

            Guild guild = guildMember.discordCoreClient->guilds->getGuildAsync({ .guildId = guildMember.data.guildId }).get();

            DiscordCoreInternal::OverWriteData overwriteEveryone = channel.data.permissionOverwrites.at(guild.data.id);

            if (overwriteEveryone.id != "") {
                permissionsInt &= ~stoll(overwriteEveryone.deny);
                permissionsInt |= stoll(overwriteEveryone.allow);
            }

            map<string, OverWriteData> overWrites = channel.data.permissionOverwrites;
            __int64 allow = 0;
            __int64 deny = 0;
            for (auto& role : guildMember.data.roles) {
                Role currentRole = guildMember.discordCoreClient->roles->getRoleAsync({ .guildId = guildMember.data.guildId, .roleId = role }).get();
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
            permissions = computeBasePermissions(guildMember, *guildMember.discordCoreClient->guilds, *guildMember.discordCoreClient->roles);
            permissions = computeOverwrites(permissions, guildMember, channel);
            return permissions;
        }

        static bool checkForPermission(GuildMember guildMember, Channel channel, Permissions permission) {
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

    bool doWeHaveAdminPermissions(InputEventData eventData,DiscordGuild discordGuild, Channel channel, GuildMember guildMember) {
        bool doWeHaveAdmin = PermissionsConverter::checkForPermission(guildMember, channel, Permissions::ADMINISTRATOR);

        if (doWeHaveAdmin) {
            return true;
        }

        bool areWeACommander = checkForBotCommanderStatus(guildMember, *eventData.discordCoreClient->discordUser);

        if (areWeACommander) {
            return true;
        }

        string msgString = "------\n**Sorry, but you don't have the permissions required for that!**\n------";
        EmbedData msgEmbed;
        msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
        msgEmbed.setColor(discordGuild.data.borderColor);
        msgEmbed.setDescription(msgString);
        msgEmbed.setTimeStamp(getTimeAndDate());
        msgEmbed.setTitle("__**Permissions Issue:**__");
        if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
            InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
            responseData.channelId = eventData.messageData.channelId;
            responseData.messageId = eventData.messageData.id;
            responseData.embeds.push_back(msgEmbed);
            InputEventData event01 = InputEventHandler::respondToEvent(responseData).get();
            InputEventHandler::deleteInputEventResponse(event01, 20000).get();
        }
        else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
            InputEventData event(eventData.messageData, eventData.interactionData, InputEventType::SLASH_COMMAND_INTERACTION);
            InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
            responseData.applicationId = eventData.interactionData.applicationId;
            responseData.embeds.push_back(msgEmbed);
            responseData.interactionId = eventData.interactionData.id;
            responseData.interactionToken = eventData.interactionData.token;
            responseData.type = InteractionCallbackType::ChannelMessage;
            event = InputEventHandler::respondToEvent(responseData).get();
            event.interactionData.applicationId = eventData.interactionData.applicationId;
            event.interactionData.token = eventData.interactionData.token;
            InputEventHandler::deleteInputEventResponse(event, 20000).get();
        }
        return false;
    }

    /**
    * Recurses through a succession of messages.
    */
    task<void> recurseThroughMessagePages(string userID, InputEventData originalEvent, unsigned int currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter) {
        apartment_context mainThread;
        co_await resume_background();
        unsigned int newCurrentPageIndex = currentPageIndex;
        try {

            vector<ActionRowData> actionRowDataVector;
            ActionRowData actionRowData;
            ComponentData component01;
            component01.customId = "backwards";
            component01.disabled = false;
            component01.emoji.name = "◀️";
            component01.label = "Back";
            component01.style = ButtonStyle::Primary;
            component01.type = ComponentType::Button;
            actionRowData.components.push_back(component01);
            ComponentData component02;
            component02.customId = "forwards";
            component02.disabled = false;
            component02.emoji.name = "▶️";
            component02.label = "Forward";
            component02.style = ButtonStyle::Primary;
            component02.type = ComponentType::Button;
            actionRowData.components.push_back(component02);

            ComponentData component03;
            component03.customId = "exit";
            component03.disabled = false;
            component03.emoji.name = "❌";
            component03.label = "Exit";
            component03.type = ComponentType::Button;
            component03.style = ButtonStyle::Primary;
            actionRowData.components.push_back(component03);
            actionRowDataVector.push_back(actionRowData);
            bool doWeQuit = false;
            InputEventData event01;
            InputEventResponseData responseDataRegularMessage(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
            InputEventResponseData responseDataInteraction(InputEventResponseType::INTERACTION_RESPONSE);
            if (originalEvent.eventType == InputEventType::REGULAR_MESSAGE) {
                responseDataRegularMessage.channelId = originalEvent.messageData.channelId;
                responseDataRegularMessage.messageId = originalEvent.messageData.id;
                responseDataRegularMessage.embeds.push_back(messageEmbeds[currentPageIndex]);
                responseDataRegularMessage.components = actionRowDataVector;
                responseDataRegularMessage.requesterId = originalEvent.requesterId;
                responseDataRegularMessage.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
                event01 = InputEventHandler::respondToEvent(responseDataRegularMessage).get();
            }
            else if (originalEvent.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                responseDataInteraction.applicationId = originalEvent.interactionData.applicationId;
                responseDataInteraction.interactionId = originalEvent.interactionData.id;
                responseDataInteraction.interactionToken = originalEvent.interactionData.token;
                responseDataInteraction.embeds.push_back(messageEmbeds[currentPageIndex]);
                responseDataInteraction.components = actionRowDataVector;
                responseDataInteraction.requesterId = originalEvent.requesterId;
                responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE;
                InputEventHandler::respondToEvent(responseDataInteraction).get();
                event01.messageData.channelId = originalEvent.getChannelId();
                event01.messageData.id = originalEvent.getMessageId();
                responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
                event01 = InputEventHandler::respondToEvent(responseDataInteraction).get();
                event01.interactionData.applicationId = originalEvent.interactionData.applicationId;
                event01.interactionData.token = originalEvent.interactionData.token;
            }
            while (doWeQuit == false) {
                Button button(event01);
                ButtonInteractionData buttonIntData = button.getOurButtonData(60000).get();
                if (button.getButtonId() == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
                    newCurrentPageIndex = 0;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
                        responseDataRegularMessage.applicationId = buttonIntData.applicationId;
                        responseDataRegularMessage.interactionId = buttonIntData.id;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataRegularMessage.embeds = embeds;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        responseDataRegularMessage.channelId = event01.getChannelId();
                        responseDataRegularMessage.messageId = event01.getMessageId();
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();

                    }
                    else {
                        InputEventResponseData responseData(InputEventResponseType::DEFER_BUTTON_RESPONSE);
                        responseData.interactionToken = buttonIntData.token;
                        responseData.interactionId = buttonIntData.id;
                        InputEventHandler::respondToEvent(responseData).get();
                        responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataInteraction.embeds = embeds;
                        InputEventHandler::respondToEvent(responseDataInteraction).get();
                    }
                }
                else if (button.getButtonId() == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
                    newCurrentPageIndex += 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
                        responseDataRegularMessage.applicationId = buttonIntData.applicationId;
                        responseDataRegularMessage.interactionId = buttonIntData.id;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataRegularMessage.embeds = embeds;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        responseDataRegularMessage.channelId = event01.getChannelId();
                        responseDataRegularMessage.messageId = event01.getMessageId();
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                    }
                    else {
                        InputEventResponseData responseData(InputEventResponseType::DEFER_BUTTON_RESPONSE);
                        responseData.interactionToken = buttonIntData.token;
                        responseData.interactionId = buttonIntData.id;
                        InputEventHandler::respondToEvent(responseData).get();
                        responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataInteraction.embeds = embeds;
                        InputEventHandler::respondToEvent(responseDataInteraction).get();
                    }
                }
                else if (button.getButtonId() == "backwards" && (newCurrentPageIndex > 0)) {
                    newCurrentPageIndex -= 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
                        responseDataRegularMessage.applicationId = buttonIntData.applicationId;
                        responseDataRegularMessage.interactionId = buttonIntData.id;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataRegularMessage.embeds = embeds;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        responseDataRegularMessage.channelId = event01.getChannelId();
                        responseDataRegularMessage.messageId = event01.getMessageId();
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                    }
                    else {
                        InputEventResponseData responseData(InputEventResponseType::DEFER_BUTTON_RESPONSE);
                        responseData.interactionToken = buttonIntData.token;
                        responseData.interactionId = buttonIntData.id;
                        InputEventHandler::respondToEvent(responseData).get();
                        responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataInteraction.embeds = embeds;
                        InputEventHandler::respondToEvent(responseDataInteraction).get();
                    }
                }
                else if (button.getButtonId() == "backwards" && (newCurrentPageIndex == 0)) {
                    newCurrentPageIndex = (unsigned int)messageEmbeds.size() - 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
                        responseDataRegularMessage.applicationId = buttonIntData.applicationId;
                        responseDataRegularMessage.interactionId = buttonIntData.id;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                        responseDataRegularMessage.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataRegularMessage.embeds = embeds;
                        responseDataRegularMessage.interactionToken = buttonIntData.token;
                        responseDataRegularMessage.channelId = event01.getChannelId();
                        responseDataRegularMessage.messageId = event01.getMessageId();
                        InputEventHandler::respondToEvent(responseDataRegularMessage).get();
                    }
                    else {
                        InputEventResponseData responseData(InputEventResponseType::DEFER_BUTTON_RESPONSE);
                        responseData.interactionToken = buttonIntData.token;
                        responseData.interactionId = buttonIntData.id;
                        InputEventHandler::respondToEvent(responseData).get();
                        responseDataInteraction.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseDataInteraction.embeds = embeds;
                        InputEventHandler::respondToEvent(responseDataInteraction).get();
                    }
                }
                else if (button.getButtonId() == "exit" || button.getButtonId() == "") {
                    if (deleteAfter == true) {
                        InputEventHandler::deleteInputEventResponse(event01);
                    }
                    doWeQuit = true;
                }
            }
            co_await mainThread;
            co_return;
        }
        catch (exception& e) {
            cout << "recurseThroughMessagePages() Error: " << e.what() << endl << endl;
        }
    };

}
#endif