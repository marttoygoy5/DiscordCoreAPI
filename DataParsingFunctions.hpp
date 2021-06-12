// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATA_PARSING_FUNCTIONS_
#define _DATA_PARSING_FUNCTIONS_

#include "pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::OverWriteData* pDataStructure) {
        DiscordCoreAPI::OverWriteData overWriteData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            overWriteData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            overWriteData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("allow") && !jsonObjectData.at("allow").is_null()) {
            overWriteData.allow = jsonObjectData.at("allow").get<string>();
        }

        if (jsonObjectData.contains("deny") && !jsonObjectData.at("deny").is_null()) {
            overWriteData.deny = jsonObjectData.at("deny").get<string>();
        }

        *pDataStructure = overWriteData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::UserData* pDataStructure) {
        DiscordCoreAPI::UserData userData = *pDataStructure;

        if (jsonObjectData.contains("username") && !jsonObjectData.at("username").is_null()) {
            userData.username = jsonObjectData.at("username").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            userData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("discriminator") && !jsonObjectData.at("discriminator").is_null()) {
            userData.discriminator = jsonObjectData.at("discriminator").get<string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
            userData.avatar = jsonObjectData.at("avatar").get<string>();
        }

        if (jsonObjectData.contains("bot") && !jsonObjectData.at("bot").is_null()) {
            userData.bot = jsonObjectData.at("bot").get<bool>();
        }

        if (jsonObjectData.contains("system") && !jsonObjectData.at("system").is_null()) {
            userData.system = jsonObjectData.at("system").get<bool>();
        }

        if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData.at("mfa_enabled").is_null()) {
            userData.mfa_enabled = jsonObjectData.at("mfa_enabled").get<bool>();
        }

        if (jsonObjectData.contains("locale") && !jsonObjectData.at("locale").is_null()) {
            userData.locale = jsonObjectData.at("locale").get<string>();
        }

        if (jsonObjectData.contains("verified") && !jsonObjectData.at("verified").is_null()) {
            userData.verified = jsonObjectData.at("verified").get<bool>();
        }

        if (jsonObjectData.contains("email") && !jsonObjectData.at("email").is_null()) {
            userData.email = jsonObjectData.at("email").get<string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            userData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("premium_type") && !jsonObjectData.at("premium_type").is_null()) {
            userData.premium_type = jsonObjectData.at("premium_type").get<int>();
        }

        if (jsonObjectData.contains("public_flags") && !jsonObjectData.at("public_flags").is_null()) {
            userData.public_flags = jsonObjectData.at("public_flags").get<int>();
        }

        *pDataStructure = userData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMetadataData* pDataStructure) {
        DiscordCoreAPI::ThreadMetadataData threadMetadata = *pDataStructure;

        if (jsonObjectData.contains("archived") && !jsonObjectData.at("archived").is_null()) {
            threadMetadata.archived = jsonObjectData.at("archived").get<bool>();
        }

        if (jsonObjectData.contains("archiver_id") && !jsonObjectData.at("archiver_id").is_null()) {
            threadMetadata.archiverId = jsonObjectData.at("archiver_id").get<string>();
        }

        if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData.at("auto_archive_duration").is_null()) {
            threadMetadata.autoArchiveDuration = jsonObjectData.at("auto_archive_duration").get<int>();
        }

        if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData.at("archive_timestamp").is_null()) {
            threadMetadata.archiveTimestamp = jsonObjectData.at("archive_timestamp").get<string>();
        }

        if (jsonObjectData.contains("locked") && !jsonObjectData.at("locked").is_null()) {
            threadMetadata.locked = jsonObjectData.at("locked").get<bool>();
        }

        *pDataStructure = threadMetadata;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMember* pDataStructure) {
        DiscordCoreAPI::ThreadMember threadMember = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            threadMember.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            threadMember.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("join_timestamp") && !jsonObjectData.at("join_timestamp").is_null()) {
            threadMember.joinTimestamp = jsonObjectData.at("join_timestamp").get<string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            threadMember.flags = jsonObjectData.at("falgs").get<int>();
        }

        *pDataStructure = threadMember;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelData* pDataStructure) {
        DiscordCoreAPI::ChannelData channelData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            channelData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            channelData.type = (DiscordCoreAPI::ChannelType)jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            channelData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
            channelData.position = jsonObjectData.at("position").get<int>();
        }        

        if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData.at("permission_overwrites").is_null()) {
            map<string, DiscordCoreAPI::OverWriteData> newMap;
            for (auto value : jsonObjectData.at("permission_overwrites")) {
                DiscordCoreAPI::OverWriteData newData;
                parseObject(value, &newData);
                newMap.insert(make_pair(newData.id, newData));
            }
            channelData.permissionOverwrites = newMap;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            channelData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
            channelData.topic = jsonObjectData.at("topic").get<string>();
        }

        if (jsonObjectData.contains("nsfw") && !jsonObjectData.at("nsfw").is_null()) {
            channelData.nsfw = jsonObjectData.at("nsfw").get<bool>();
        }

        if (jsonObjectData.contains("last_message_id") && !jsonObjectData.at("last_message_id").is_null()) {
            channelData.lastMessageId = jsonObjectData.at("last_message_id").get<string>();
        }

        if (jsonObjectData.contains("bitrate") && !jsonObjectData.at("bitrate").is_null()) {
            channelData.bitrate = jsonObjectData.at("bitrate").get<int>();
        }

        if (jsonObjectData.contains("user_limit") && !jsonObjectData.at("user_limit").is_null()) {
            channelData.userLimit = jsonObjectData.at("user_limit").get<int>();
        }

        if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData.at("rate_limit_per_user").is_null()) {
            channelData.rateLimitPerUser = jsonObjectData.at("rate_limit_per_user").get<int>();
        }

        if (jsonObjectData.contains("recipients") && !jsonObjectData.at("recipients").is_null()) {
            vector<DiscordCoreAPI::UserData> newArray;
            for (auto value: jsonObjectData.at("recipients")) {
                DiscordCoreAPI::UserData newData;
                parseObject(value, &newData);
                newArray.push_back(newData);
            }
            channelData.recipients = newArray;
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            channelData.icon = jsonObjectData.at("icon").get<string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            channelData.ownerId = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            channelData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("parent_id") && !jsonObjectData.at("parent_id").is_null()) {
            channelData.parentId = jsonObjectData.at("parent_id").get<string>();
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData.at("last_pin_timestamp").is_null()) {
            channelData.lastPinTimestamp = jsonObjectData.at("last_pin_timestamp").get<string>();
        }

        if (jsonObjectData.contains("rtc_region") && !jsonObjectData.at("rtc_region").is_null()) {
            channelData.rtcRegion = jsonObjectData.at("rtc_region").get<string>();
        }

        if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData.at("video_quality_mode").is_null()) {
            channelData.videoQualityMode = jsonObjectData.at("video_quality_mode").get<int>();
        }

        if (jsonObjectData.contains("message_count") && !jsonObjectData.at("message_count").is_null()) {
            channelData.messageCount = jsonObjectData.at("message_count").get<int>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            channelData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            channelData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
            DiscordCoreAPI::ThreadMetadataData newValue = channelData.threadMetadata;
            parseObject(jsonObjectData.at("thread_metadata"), &newValue);
            channelData.threadMetadata = newValue;
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
            DiscordCoreAPI::ThreadMember newValue = channelData.member;
            parseObject(jsonObjectData.at("thread_metadata"), &newValue);
            channelData.member = newValue;
        }

        *pDataStructure = channelData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::RoleTagsData* pDataStructure) {
        DiscordCoreAPI::RoleTagsData roleTagsData = *pDataStructure;

        if (jsonObjectData.contains("bot_id") && !jsonObjectData.at("bot_id").is_null()) {
            roleTagsData.botId = jsonObjectData.at("bot_id").get<string>();
        }

        if (jsonObjectData.contains("integration_id") && !jsonObjectData.at("integration_id").is_null()) {
            roleTagsData.integrationId = jsonObjectData.at("integration_id").get<string>();
        }

        *pDataStructure = roleTagsData;
    }
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::RoleData* pDataStructure) {
        DiscordCoreAPI::RoleData roleData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            roleData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            roleData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
            roleData.color = jsonObjectData.at("color").get<int>();
        }

        if (jsonObjectData.contains("hoist") && !jsonObjectData.at("hoist").is_null()) {
            roleData.hoist = jsonObjectData.at("hoist").get<bool>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
            roleData.position = jsonObjectData.at("position").get<int>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            roleData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
            roleData.managed = jsonObjectData.at("managed").get<bool>();
        }

        if (jsonObjectData.contains("mentionable") && !jsonObjectData.at("mentionable").is_null()) {
            roleData.mentionable = jsonObjectData.at("mentionable").get<bool>();
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
            DiscordCoreAPI::RoleTagsData newValue = roleData.tags;
            parseObject(jsonObjectData.at("tags"), &newValue);
            roleData.tags = newValue;
        }

        *pDataStructure = roleData;
    }
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMemberData* pDataStructure) {
        DiscordCoreAPI::GuildMemberData guildMemberData;

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData newValue = guildMemberData.user;
            parseObject(jsonObjectData.at("user"), &newValue);
            guildMemberData.user = newValue;
        }

        if (jsonObjectData.contains("nick") && !jsonObjectData.at("nick").is_null()) {
            guildMemberData.nick = jsonObjectData.at("nick").get<string>();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<string> newArray;
            for (auto value: jsonObjectData.at("roles")) {
                newArray.push_back(value);
            }
            guildMemberData.roles = newArray;
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
            guildMemberData.joined_at = jsonObjectData.at("joined_at").get<string>();
        }

        if (jsonObjectData.contains("premium_since") && !jsonObjectData.at("premium_since").is_null()) {
            guildMemberData.premium_since = jsonObjectData.at("premium_since").get<string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
            guildMemberData.deaf = jsonObjectData.at("deaf").get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
            guildMemberData.mute = jsonObjectData.at("mute").get<bool>();
        }

        if (jsonObjectData.contains("pending") && !jsonObjectData.at("pending").is_null()) {
            guildMemberData.pending = jsonObjectData.at("pending").get<bool>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            guildMemberData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            guildMemberData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        *pDataStructure = guildMemberData;
    }
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::EmojiData* pDataStructure) {
        DiscordCoreAPI::EmojiData emojiData = *pDataStructure;
        
        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            emojiData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            emojiData.name = jsonObjectData.at("name").get<string>();
        }
        
        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<DiscordCoreAPI::RoleData> newVector;
            for (auto value: jsonObjectData.at("roles")) {
                DiscordCoreAPI::RoleData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            emojiData.roles = newVector;
        }
        
        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData newValue = emojiData.user;
            parseObject(jsonObjectData.at("user"), &newValue);
            emojiData.user = newValue;
        }
        
        if (jsonObjectData.contains("require_colons") && !jsonObjectData.at("require_colons").is_null()) {
            emojiData.requireColons = jsonObjectData.at("require_colons").get<bool>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
            emojiData.managed = jsonObjectData.at("managed").get<bool>();
        }

        if (jsonObjectData.contains("animated") && !jsonObjectData.at("animated").is_null()) {
            emojiData.animated = jsonObjectData.at("animated").get<bool>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
            emojiData.available = jsonObjectData.at("available").get<bool>();
        }
        
        *pDataStructure = emojiData;
    }
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::VoiceStateData* pDataStructure) {
        DiscordCoreAPI::VoiceStateData voiceStateData = *pDataStructure;

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            voiceStateData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            voiceStateData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            voiceStateData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("guild_member") && !jsonObjectData.at("guild_member").is_null()) {
            DiscordCoreAPI::GuildMemberData newValue = voiceStateData.guildMember;
            parseObject(jsonObjectData.at("guild_member"), &newValue);
            voiceStateData.guildMember = newValue;
        }

        if (jsonObjectData.contains("session_id") && !jsonObjectData.at("session_id").is_null()) {
            voiceStateData.sessionId = jsonObjectData.at("session_id").get<string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
            voiceStateData.deaf = jsonObjectData.at("deaf").get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
            voiceStateData.mute = jsonObjectData.at("mute").get<bool>();
        }

        if (jsonObjectData.contains("self_deaf") && !jsonObjectData.at("self_deaf").is_null()) {
            voiceStateData.selfDeaf = jsonObjectData.at("self_deaf").get<bool>();
        }

        if (jsonObjectData.contains("self_mute") && !jsonObjectData.at("self_mute").is_null()) {
            voiceStateData.selfMute = jsonObjectData.at("self_mute").get<bool>();
        }

        if (jsonObjectData.contains("self_stream") && !jsonObjectData.at("self_stream").is_null()) {
            voiceStateData.selfStream = jsonObjectData.at("self_stream").get<bool>();
        }

        if (jsonObjectData.contains("self_video") && !jsonObjectData.at("self_video").is_null()) {
            voiceStateData.selfVideo = jsonObjectData.at("self_video").get<bool>();
        }

        if (jsonObjectData.contains("suppress") && !jsonObjectData.at("suppress").is_null()) {
            voiceStateData.suppress = jsonObjectData.at("suppress").get<bool>();
        }

        if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData.at("request_to_speak_timestamp").is_null()) {
            voiceStateData.requestToSpeakTimestamp = jsonObjectData.at("request_to_speak_timestamp").get<string>();
        }

        *pDataStructure = voiceStateData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::PartyData* pDataStructure) {
        DiscordCoreAPI::PartyData partyData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            partyData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("size") && !jsonObjectData.at("size").is_null()) {
            int size[2];
            size[0] = jsonObjectData.at("size").at(0);
            size[1] = jsonObjectData.at("size").at(1);
            partyData.size[0] = size[0];
            partyData.size[1] = size[1];
        }

        *pDataStructure = partyData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AssetsData* pDataStructure) {
        DiscordCoreAPI::AssetsData assetData = *pDataStructure;

        if (jsonObjectData.contains("large_image") && !jsonObjectData.at("large_image").is_null()) {
            assetData.largeImage = jsonObjectData.at("large_image").get<string>();
        }

        if (jsonObjectData.contains("large_text") && !jsonObjectData.at("large_text").is_null()) {
            assetData.largeText = jsonObjectData.at("large_text").get<string>();
        }

        if (jsonObjectData.contains("small_image") && !jsonObjectData.at("small_image").is_null()) {
            assetData.smallImage = jsonObjectData.at("small_image").get<string>();
        }

        if (jsonObjectData.contains("small_text") && !jsonObjectData.at("small_text").is_null()) {
            assetData.smallText = jsonObjectData.at("small_text").get<string>();
        }

        *pDataStructure = assetData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::SecretsData* pDataStructure) {
        DiscordCoreAPI::SecretsData secretData = *pDataStructure;

        if (jsonObjectData.contains("join") && !jsonObjectData.at("join").is_null()) {
            secretData.join = jsonObjectData.at("join").get<string>();
        }

        if (jsonObjectData.contains("spectate") && !jsonObjectData.at("spectate").is_null()) {
            secretData.spectate = jsonObjectData.at("spectate").get<string>();
        }

        if (jsonObjectData.contains("match") && !jsonObjectData.at("match").is_null()) {
            secretData.match = jsonObjectData.at("match").get<string>();
        }

        *pDataStructure = secretData;

    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ButtonData* pDataStructure) {
        DiscordCoreAPI::ButtonData buttonData = *pDataStructure;

        if (jsonObjectData.contains("label") && !jsonObjectData.at("label").is_null()) {
            buttonData.label = jsonObjectData.at("label").get<string>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            buttonData.url = jsonObjectData.at("url").get<string>();
        }

        *pDataStructure = buttonData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ClientStatusData* pDataStructure) {
        DiscordCoreAPI::ClientStatusData clientStatusData = *pDataStructure;

        if (jsonObjectData.contains("desktop") && !jsonObjectData.at("desktop").is_null()) {
            clientStatusData.desktop = jsonObjectData.at("desktop").get<string>();
        }

        if (jsonObjectData.contains("mobile") && !jsonObjectData.at("mobile").is_null()) {
            clientStatusData.mobile = jsonObjectData.at("mobile").get<string>();
        }

        if (jsonObjectData.contains("web") && !jsonObjectData.at("web").is_null()) {
            clientStatusData.web = jsonObjectData.at("web").get<string>();
        }

        *pDataStructure = clientStatusData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TimestampData* pDataStructure) {
        DiscordCoreAPI::TimestampData timestampData = *pDataStructure;

        if (jsonObjectData.contains("start") && !jsonObjectData.at("start").is_null()) {
            timestampData.start = jsonObjectData.at("start").get<int>();
        }

        if (jsonObjectData.contains("end") && !jsonObjectData.at("end").is_null()) {
            timestampData.end = jsonObjectData.at("end").get<int>();
        }

        *pDataStructure = timestampData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ActivityData* pDataStructure) {
        DiscordCoreAPI::ActivityData activityData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            activityData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            activityData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            activityData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("created_at") && !jsonObjectData.at("created_at").is_null()) {
            activityData.createdAt = jsonObjectData.at("created_at").get<int>();
        }

        if (jsonObjectData.contains("timestamps") && !jsonObjectData.at("timestamps").is_null()) {
            DiscordCoreAPI::TimestampData newValue = activityData.timestamps;
            parseObject(jsonObjectData.at("timestamps"), &newValue);
            activityData.timestamps = newValue;
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            activityData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("details") && !jsonObjectData.at("details").is_null()) {
            activityData.details = jsonObjectData.at("details").get<string>();
        }

        if (jsonObjectData.contains("state") && !jsonObjectData.at("state").is_null()) {
            activityData.state = jsonObjectData.at("state").get<string>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            DiscordCoreAPI::EmojiData newValue = activityData.emoji;
            parseObject(jsonObjectData.at("emoji"), &newValue);
            activityData.emoji = newValue;
        }

        if (jsonObjectData.contains("party") && !jsonObjectData.at("party").is_null()) {
            DiscordCoreAPI::PartyData newValue = activityData.party;
            parseObject(jsonObjectData.at("party"), &newValue);
            activityData.party = newValue;
        }

        if (jsonObjectData.contains("assets") && !jsonObjectData.at("assets").is_null()) {
            DiscordCoreAPI::AssetsData newValue = activityData.assets;
            parseObject(jsonObjectData.at("assets"), &newValue);
            activityData.assets = newValue;
        }

        if (jsonObjectData.contains("secrets") && !jsonObjectData.at("secrets").is_null()) {
            DiscordCoreAPI::SecretsData newValue = activityData.secrets;
            parseObject(jsonObjectData.at("secrets"), &newValue);
            activityData.secrets = newValue;
        }

        if (jsonObjectData.contains("instance") && !jsonObjectData.at("instance").is_null()) {
            activityData.instance = jsonObjectData.at("instance").get<bool>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            activityData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("buttons") && !jsonObjectData.at("buttons").is_null()) {
            vector< DiscordCoreAPI::ButtonData> newVector;
            for (auto value: jsonObjectData.at("buttons")) {
                DiscordCoreAPI::ButtonData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            activityData.buttons = newVector;
        }

        *pDataStructure = activityData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::PresenceUpdateData* pDataStructure) {
        DiscordCoreAPI::PresenceUpdateData presenceUpdateData = *pDataStructure;

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData newValue = presenceUpdateData.user;
            parseObject(jsonObjectData.at("user"), &newValue);
            presenceUpdateData.user = newValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            presenceUpdateData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("status") && !jsonObjectData.at("status").is_null()) {
            presenceUpdateData.status = jsonObjectData.at("status").get<string>();
        }

        if (jsonObjectData.contains("activities") && !jsonObjectData.at("activities").is_null()) {
            vector<DiscordCoreAPI::ActivityData> newVector;
            for (auto value: jsonObjectData.at("activities")) {
                DiscordCoreAPI::ActivityData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            presenceUpdateData.activities = newVector;
        }

        if (jsonObjectData.contains("client_status") && !jsonObjectData.at("client_status").is_null()) {
            DiscordCoreAPI::ClientStatusData newValue = presenceUpdateData.clientStatus;
            parseObject(jsonObjectData.at("client_status"), &newValue);
            presenceUpdateData.clientStatus = newValue;
        }

        *pDataStructure = presenceUpdateData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenChannelData* pDataStructure) {
        DiscordCoreAPI::WelcomeScreenChannelData welcomeSceenChannelData = *pDataStructure;

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            welcomeSceenChannelData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            welcomeSceenChannelData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("emoji_id") && !jsonObjectData.at("emoji_id").is_null()) {
            welcomeSceenChannelData.emojiId = jsonObjectData.at("emoji_id").get<string>();
        }

        if (jsonObjectData.contains("emoji_name") && !jsonObjectData.at("emoji_name").is_null()) {
            welcomeSceenChannelData.emojiName = jsonObjectData.at("emoji_name").get<string>();
        }

        *pDataStructure = welcomeSceenChannelData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenData* pDataStructure) {
        DiscordCoreAPI::WelcomeScreenData welcomeScreenData = *pDataStructure;

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            welcomeScreenData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("welcome_channels") && !jsonObjectData.at("welcome_channels").is_null()) {
            vector<DiscordCoreAPI::WelcomeScreenChannelData> newVector;
            for (auto value : jsonObjectData.at("welcome_channels")) {
                DiscordCoreAPI::WelcomeScreenChannelData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            welcomeScreenData.welcomeChannels = newVector;
        }

        *pDataStructure = welcomeScreenData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::GuildData * pDataStructure) {
        DiscordCoreAPI::GuildData guildData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            guildData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
            guildData.afkChannelID = jsonObjectData.at("afk_channel_id").get<string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            guildData.icon = jsonObjectData.at("icon").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            guildData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("icon_hash") && !jsonObjectData.at("icon_hash").is_null()) {
            guildData.iconHash = jsonObjectData.at("icon_hash").get<string>();
        }

        if (jsonObjectData.contains("splash") && !jsonObjectData.at("splash").is_null()) {
            guildData.splash = jsonObjectData.at("splash").get<string>();
        }

        if (jsonObjectData.contains("discovery_splash") && !jsonObjectData.at("discovery_splash").is_null()) {
            guildData.discoverySplash = jsonObjectData.at("discovery_splash").get<string>();
        }

        if (jsonObjectData.contains<string>("owner") && !jsonObjectData.at("owner").is_null()) {
            guildData.owner = jsonObjectData.at("owner").get<bool>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            guildData.ownerID = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("preferred_locale") && !jsonObjectData.at("preferred_locale").is_null()) {
            guildData.preferredLocale = jsonObjectData.at("preferred_locale").get<string>();
        }

        if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData.at("public_updates_channel_id").is_null()) {
            guildData.publicUpdatesChannelID = jsonObjectData.at("public_updates_channel_id").get<string>();
        }

        if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData.at("vanity_url_code").is_null()) {
            guildData.vanityURLCode = jsonObjectData.at("vanity_url_code").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            guildData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("banner") && !jsonObjectData.at("banner").is_null()) {
            guildData.banner = jsonObjectData.at("banner").get<string>();
        }

        if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData.at("rule_Channel_id").is_null()) {
            guildData.ruleChannelID = jsonObjectData.at("rule_Channel_id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            guildData.applicationID = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
            guildData.joinedAt = jsonObjectData.at("joined_at").get<string>();
        }

        if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData.at("widget_channel_id").is_null()) {
            guildData.widgetChannelID = jsonObjectData.at("widget_channel_id").get<string>();
        }

        if (jsonObjectData.contains("system_channel_id") && !jsonObjectData.at("system_channel_id").is_null()) {
            guildData.systemChannelID = jsonObjectData.at("system_channel_id").get<string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
            guildData.region = jsonObjectData.at("region").get<string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
            guildData.afkChannelID = jsonObjectData.at("afk_channel_id").get<string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            guildData.ownerID = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
            guildData.region = jsonObjectData.at("region").get<string>();
        }

        if (jsonObjectData.contains("features") && !jsonObjectData.at("features").is_null()) {
            vector<string> newVector;
            for (auto value : jsonObjectData.at("features")) {
                newVector.push_back(value);
            }
            guildData.features = newVector;
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            guildData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()){
            map<string, DiscordCoreAPI::RoleData> newMap;
            for (auto value: jsonObjectData.at("roles")){
                DiscordCoreAPI::RoleData newData;
                parseObject(value, &newData);
                newMap.insert(make_pair(newData.id, newData));
            }
            guildData.roles = newMap;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
            guildData.owner = jsonObjectData.at("owner").get<bool>();
        }

        if (jsonObjectData.contains("afk_timeout") && !jsonObjectData.at("afk_timeout").is_null()) {
            guildData.afkTimeOut = jsonObjectData.at("afk_timeout").get<int>();
        }

        if (jsonObjectData.contains("widget_enabled") && !jsonObjectData.at("widget_enabled").is_null()) {
            guildData.widgetEnabled = jsonObjectData.at("widget_enabled").get<bool>();
        }

        if (jsonObjectData.contains("verification_level") && !jsonObjectData.at("verification_level").is_null()) {
            guildData.verificationLevel = jsonObjectData.at("verification_level").get<DiscordCoreAPI::VerificationLevel>();
        }

        if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData.at("default_message_notification_level").is_null()) {
            guildData.defaultMessageNotifications = jsonObjectData.at("default_message_notification_level").get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
        }

        if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData.at("explicit_content_filter_level").is_null()) {
            guildData.explicitContentFilter = jsonObjectData.at("explicit_content_filter_level").get< DiscordCoreAPI::ExplicitContentFilterLevel>();
        }
        
        if (jsonObjectData.contains("emojis") && !jsonObjectData.at("emojis").is_null()) {
            vector<DiscordCoreAPI::EmojiData> newVector;
            for (auto value: jsonObjectData.at("emojis")) {
                DiscordCoreAPI::EmojiData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            } 
            guildData.emoji = newVector;
        }

        if (jsonObjectData.contains("mfa_level") && !jsonObjectData.at("mfa_level").is_null()) {
            guildData.mfaLevel = jsonObjectData.at("mfa_level").get< DiscordCoreAPI::MFALevel>();
        }

        if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData.at("system_channel_flags").is_null()) {
            guildData.systemChannelFlags = jsonObjectData.at("system_channel_flags").get<int>();
        }

        if (jsonObjectData.contains("large") && !jsonObjectData.at("large").is_null()) {
            guildData.large = jsonObjectData.at("large").get<bool>();
        }

        if (jsonObjectData.contains("unavailable") && !jsonObjectData.at("unavailable").is_null()) {
            guildData.unavailable = jsonObjectData.at("unavailable").get<bool>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            guildData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("voice_states") && !jsonObjectData.at("voice_states").is_null()) {
            vector<DiscordCoreAPI::VoiceStateData> newVector;
            for (auto value: jsonObjectData.at("voice_states")) {
                DiscordCoreAPI::VoiceStateData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            guildData.voiceStates = newVector;
        }

        if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
            vector<DiscordCoreAPI::GuildMemberData> newVector;
            for (auto value : jsonObjectData.at("members")) {
                DiscordCoreAPI::GuildMemberData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            guildData.members = newVector;
        }

        if (jsonObjectData.contains("channels") && !jsonObjectData.at("channels").is_null()) {
            vector<DiscordCoreAPI::ChannelData> newVector;
            for (auto value : jsonObjectData.at("channels")) {
                DiscordCoreAPI::ChannelData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            guildData.channels = newVector;
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
            vector<DiscordCoreAPI::ChannelData> newVector;
            for (auto value : jsonObjectData.at("threads")) {
                DiscordCoreAPI::ChannelData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            guildData.threads = newVector;
        }

        if (jsonObjectData.contains("presences") && !jsonObjectData.at("presences").is_null()) {
            vector<DiscordCoreAPI::PresenceUpdateData> newVector;
            for (auto value : jsonObjectData.at("presences")) {
                DiscordCoreAPI::PresenceUpdateData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            guildData.presences = newVector;
        }

        if (jsonObjectData.contains("max_presences") && !jsonObjectData.at("max_presences").is_null()) {
            guildData.maxPresences = jsonObjectData.at("max_presences").get<int>();
        }

        if (jsonObjectData.contains("max_members") && !jsonObjectData.at("max_members").is_null()) {
            guildData.maxMembers = jsonObjectData.at("max_members").get<int>();
        }

        if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData.at("premium_subscription_count").is_null()) {
            guildData.premiumSubscriptionCount = jsonObjectData.at("premium_subscription_count").get<int>();
        }

        if (jsonObjectData.contains("premium_tier") && !jsonObjectData.at("premium_tier").is_null()) {
            guildData.premiumTier = jsonObjectData.at("premium_tier").get< DiscordCoreAPI::PremiumTier>();
        }

        if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData.at("max_video_channel_users").is_null()) {
            guildData.maxVideoChannelUsers = jsonObjectData.at("max_video_channel_users").get<int>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
            guildData.approximateMemberCount = jsonObjectData.at("approximate_member_count").get<int>();
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
            guildData.approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<int>();
        }

        if (jsonObjectData.contains("welcome_screen") && !jsonObjectData.at("welcome_screen").is_null()) {
            DiscordCoreAPI::WelcomeScreenData newValue = guildData.welcomeScreen;
            parseObject(jsonObjectData.at("welcome_screen"), &newValue);
            guildData.welcomeScreen = newValue;
        }

        *pDataStructure = guildData;
    };
    
    void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelMentionData* pDataStructure) {
        DiscordCoreAPI::ChannelMentionData channelMentionData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            channelMentionData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            channelMentionData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            channelMentionData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            channelMentionData.name = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = channelMentionData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::AttachmentData* pDataStructure) {
        DiscordCoreAPI::AttachmentData attachmentData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            attachmentData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("filename") && !jsonObjectData.at("filename").is_null()) {
            attachmentData.filename = jsonObjectData.at("filename").get<string>();
        }

        if (jsonObjectData.contains("content_type") && !jsonObjectData.at("content_type").is_null()) {
            attachmentData.contentType = jsonObjectData.at("content_type").get<string>();
        }

        if (jsonObjectData.contains("size") && !jsonObjectData.at("size").is_null()) {
            attachmentData.size = jsonObjectData.at("size").get<int>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            attachmentData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            attachmentData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            attachmentData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            attachmentData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = attachmentData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFooterData* pDataStructure) {
        DiscordCoreAPI::EmbedFooterData footerData = *pDataStructure;

        if (jsonObjectData.contains("text") && !jsonObjectData.at("text").is_null()) {
            footerData.text = jsonObjectData.at("text").get<string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
            footerData.iconUrl = jsonObjectData.at("icon_url").get<string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
            footerData.proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
        }

        *pDataStructure = footerData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedImageData* pDataStructure) {
        DiscordCoreAPI::EmbedImageData imageData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            imageData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            imageData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            imageData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            imageData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = imageData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedThumbnailData* pDataStructure) {
        DiscordCoreAPI::EmbedThumbnailData thumbnailData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            thumbnailData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            thumbnailData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            thumbnailData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            thumbnailData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = thumbnailData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedVideoData* pDataStructure) {
        DiscordCoreAPI::EmbedVideoData videoData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            videoData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            videoData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            videoData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            videoData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = videoData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedAuthorData* pDataStructure) {
        DiscordCoreAPI::EmbedAuthorData authorData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            authorData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
            authorData.proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            authorData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
            authorData.iconUrl = jsonObjectData.at("icon_url").get<string>();
        }

        *pDataStructure = authorData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedProviderData* pDataStructure) {
        DiscordCoreAPI::EmbedProviderData providerData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            providerData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            providerData.name = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = providerData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFieldData* pDataStructure) {
        DiscordCoreAPI::EmbedFieldData fieldData = *pDataStructure;

        if (jsonObjectData.contains("inline") && !jsonObjectData.at("inline").is_null()) {
            fieldData.Inline = jsonObjectData.at("inline").get<bool>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            fieldData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
            fieldData.value = jsonObjectData.at("value").get<string>();
        }

        *pDataStructure = fieldData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedData* pDataStructure) {
        DiscordCoreAPI::EmbedData embedData = *pDataStructure;

        if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
            embedData.title = jsonObjectData.at("title").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            embedData.type = jsonObjectData.at("type").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            embedData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            embedData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            embedData.timestamp = jsonObjectData.at("timestamp").get<string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
            stringstream stream;
            stream << jsonObjectData.at("color");
            embedData.hexColorValue = stream.str();
        }

        if (jsonObjectData.contains("footer") && !jsonObjectData.at("footer").is_null()) {
            DiscordCoreAPI::EmbedFooterData newValue = embedData.footer;
            parseObject(jsonObjectData.at("footer"), &newValue);
            embedData.footer = newValue;
        }

        if (jsonObjectData.contains("image") && !jsonObjectData.at("image").is_null()) {
            DiscordCoreAPI::EmbedImageData newValue = embedData.image;
            parseObject(jsonObjectData.at("image"), &newValue);
            embedData.image = newValue;
        }

        if (jsonObjectData.contains("provider") && !jsonObjectData.at("provider").is_null()) {
            DiscordCoreAPI::EmbedProviderData newValue = embedData.provider;
            parseObject(jsonObjectData.at("provider"), &newValue);
            embedData.provider = newValue;
        }

        if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
            DiscordCoreAPI::EmbedThumbnailData newValue = embedData.thumbnail;
            parseObject(jsonObjectData.at("thumbnail"), &newValue);
            embedData.thumbnail = newValue;
        }

        if (jsonObjectData.contains("video") && !jsonObjectData.at("video").is_null()) {
            DiscordCoreAPI::EmbedVideoData newValue = embedData.video;
            parseObject(jsonObjectData.at("video"), &newValue);
            embedData.video = newValue;
        }

        if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
            DiscordCoreAPI::EmbedAuthorData newValue = embedData.author;
            parseObject(jsonObjectData.at("author"), &newValue);
            embedData.author = newValue;
        }

        if (jsonObjectData.contains("fields") && !jsonObjectData.at("fields").is_null()) {
            vector<DiscordCoreAPI::EmbedFieldData> newVector;
            for (auto value: jsonObjectData.at("fields")) {
                DiscordCoreAPI::EmbedFieldData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            embedData.fields = newVector;
        }

        *pDataStructure = embedData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionData* pDataStructure) {
        DiscordCoreAPI::ReactionData reactionData = *pDataStructure;

       if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
           reactionData.count = jsonObjectData.at("count").get<int>();
        }

        if (jsonObjectData.contains("me") && !jsonObjectData.at("me").is_null()) {
            reactionData.count = jsonObjectData.at("me").get<bool>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            DiscordCoreAPI::EmojiData newValue = reactionData.emoji;
            parseObject(jsonObjectData.at("emoji"), &newValue);
            reactionData.emoji = newValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            reactionData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            reactionData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            reactionData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            reactionData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            DiscordCoreAPI::GuildMemberData newValue = reactionData.member;
            parseObject(jsonObjectData.at("member"), &newValue);
            reactionData.member = newValue;
        }

        *pDataStructure = reactionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageActivityData* pDataStructure) {
        DiscordCoreAPI::MessageActivityData messageActivityData = *pDataStructure;

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            messageActivityData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("party_id") && !jsonObjectData.at("party_id").is_null()) {
            messageActivityData.partyId = jsonObjectData.at("party_id").get<string>();
        }

        *pDataStructure = messageActivityData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TeamMembersObjectData* pDataStructure) {
        DiscordCoreAPI::TeamMembersObjectData teamMemberObjectData = *pDataStructure;

        if (jsonObjectData.contains("membership_state") && !jsonObjectData.at("membership_state").is_null()) {
            int theValue = jsonObjectData.at("membership_state");
            teamMemberObjectData.membershipState = theValue;
        }

        if (jsonObjectData.contains("team_id") && !jsonObjectData.at("team_id").is_null()) {
            string theValue = jsonObjectData.at("team_id");
            teamMemberObjectData.teamId = theValue;
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            json theValue = jsonObjectData.at("permissions");
            for (unsigned int x = 0; x < theValue.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < teamMemberObjectData.permissions.size(); y += 1) {
                    if (teamMemberObjectData.permissions.at(y) == theValue.at(x)) {
                        isItFound = true;
                        break;
                    }
                }
                if (isItFound == false) {
                    teamMemberObjectData.permissions.push_back(theValue.at(x));
                }
            }
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData theValue = teamMemberObjectData.user;
            parseObject(jsonObjectData.at("user"), &theValue);
            teamMemberObjectData.user = theValue;
        }

        *pDataStructure = teamMemberObjectData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TeamObjectData* pDataStructure) {
        DiscordCoreAPI::TeamObjectData teamObjectData = *pDataStructure;

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            string theValue = jsonObjectData.at("icon");
            teamObjectData.icon = theValue;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            teamObjectData.id = theValue;
        }

        if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
            json membersArray = jsonObjectData.at("members");
            for (unsigned int x = 0; x < membersArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < teamObjectData.members.size(); y += 1) {
                    if (teamObjectData.members.at(y).user.id == membersArray.at(x).at("user").at("id")) {
                        isItFound = true;
                        teamObjectData.members.erase(teamObjectData.members.begin() + y);
                        DiscordCoreAPI::TeamMembersObjectData teamMembersObjectData = teamObjectData.members.at(y);
                        parseObject(membersArray.at(x), &teamMembersObjectData);
                        teamObjectData.members.push_back(teamMembersObjectData);
                        break;
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::TeamMembersObjectData teamMembersObjectData;
                    parseObject(membersArray.at(x), &teamMembersObjectData);
                    teamObjectData.members.push_back(teamMembersObjectData);
                    break;
                }
            }
        }

        if (jsonObjectData.contains("owner_user_id") && !jsonObjectData.at("owner_user_id").is_null()) {
            string theValue = jsonObjectData.at("owner_user_id");
            teamObjectData.ownerUserId = theValue;
        }

        *pDataStructure = teamObjectData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationData* pDataStructure) {
        DiscordCoreAPI::ApplicationData applicationData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            applicationData.id = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            applicationData.name = theValue;
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            string theValue = jsonObjectData.at("icon");
            applicationData.icon = theValue;
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            string theValue = jsonObjectData.at("description");
            applicationData.description = theValue;
        }

        if (jsonObjectData.contains("rpc_origins") && !jsonObjectData.at("rpc_origins").is_null()) {
            json stringsArray = jsonObjectData.at("rpc_origins");
            for (unsigned int x = 0; x < stringsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < applicationData.rpcOrigins.size(); y += 1) {
                    if (stringsArray.at(x) == applicationData.rpcOrigins.at(y)) {
                        isItFound = true;
                        break;
                    }
                }
                if (isItFound == false) {
                    applicationData.rpcOrigins.push_back(stringsArray.at(x));
                }
            }
        }

        if (jsonObjectData.contains("bot_public") && !jsonObjectData.at("bot_public").is_null()) {
            bool theValue = jsonObjectData.at("bot_public");
            applicationData.botPublic = theValue;
        }

        if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData.at("bot_require_code_grant").is_null()) {
            bool theValue = jsonObjectData.at("bot_require_code_grant");
            applicationData.botRequireCodeGrant = theValue;
        }

        if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData.at("terms_of_service_url").is_null()) {
            string theValue = jsonObjectData.at("terms_of_service_url");
            applicationData.termsOfServiceUrl = theValue;
        }

        if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData.at("privacy_policy_url").is_null()) {
            string theValue = jsonObjectData.at("privacy_policy_url");
            applicationData.privacyPolicyUrl = theValue;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
            DiscordCoreAPI::UserData theValue = applicationData.owner;
            parseObject(jsonObjectData.at("owner"), &theValue);
            applicationData.owner = theValue;
        }

        if (jsonObjectData.contains("summary") && !jsonObjectData.at("summary").is_null()) {
            string theValue = jsonObjectData.at("summary");
            applicationData.summary = theValue;
        }

        if (jsonObjectData.contains("verify_key") && !jsonObjectData.at("verify_key").is_null()) {
            string theValue = jsonObjectData.at("verify_key");
            applicationData.verifyKey = theValue;
        }

        if (jsonObjectData.contains("team") && !jsonObjectData.at("team").is_null()) {
            DiscordCoreAPI::TeamObjectData theValue = applicationData.team;
            parseObject(jsonObjectData.at("team"), &theValue);
            applicationData.team = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            applicationData.guildId = theValue;
        }

        if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData.at("primary_sku_id").is_null()) {
            string theValue = jsonObjectData.at("primary_sku_id");
            applicationData.primarySkuId = theValue;
        }

        if (jsonObjectData.contains("slug") && !jsonObjectData.at("slug").is_null()) {
            string theValue = jsonObjectData.at("slug");
            applicationData.slug = theValue;
        }

        if (jsonObjectData.contains("cover_image") && !jsonObjectData.at("cover_image").is_null()) {
            string theValue = jsonObjectData.at("cover_image");
            applicationData.coverImage = theValue;
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            string theValue = jsonObjectData.at("flags");
            applicationData.coverImage = theValue;
        }

        *pDataStructure = applicationData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InviteData* pDataStructure) {
        DiscordCoreAPI::InviteData inviteData = *pDataStructure;

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            inviteData.channelId = theValue;
        }

        if (jsonObjectData.contains("code") && !jsonObjectData.at("code").is_null()) {
            string theValue = jsonObjectData.at("code");
            inviteData.code = theValue;
        }

        if (jsonObjectData.contains("created_at") && !jsonObjectData.at("created_at").is_null()) {
            string theValue = jsonObjectData.at("created_at");
            inviteData.createdAt = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            inviteData.guildId = theValue;
        }

        if (jsonObjectData.contains("inviter") && !jsonObjectData.at("inviter").is_null()) {
            DiscordCoreAPI::UserData userData = inviteData.inviter;
            parseObject(jsonObjectData.at("inviter"), &userData);
            inviteData.inviter = userData;
        }

        if (jsonObjectData.contains("max_age") && !jsonObjectData.at("max_age").is_null()) {
            unsigned int theValue = jsonObjectData.at("max_age");
            inviteData.maxAge = theValue;
        }

        if (jsonObjectData.contains("max_uses") && !jsonObjectData.at("max_uses").is_null()) {
            unsigned int theValue = jsonObjectData.at("max_uses");
            inviteData.maxAge = theValue;
        }

        if (jsonObjectData.contains("target_type") && !jsonObjectData.at("target_type").is_null()) {
            unsigned int theValue = jsonObjectData.at("target_type");
            inviteData.maxAge = theValue;
        }

        if (jsonObjectData.contains("target_user") && !jsonObjectData.at("target_user").is_null()) {
            DiscordCoreAPI::UserData userData = inviteData.inviter;
            parseObject(jsonObjectData.at("target_user"), &userData);
            inviteData.inviter = userData;
        }

        if (jsonObjectData.contains("target_application") && !jsonObjectData.at("target_application").is_null()) {
            DiscordCoreAPI::ApplicationData appData = inviteData.targetApplication;
            parseObject(jsonObjectData.at("target_application"), &appData);
            inviteData.targetApplication = appData;
        }

        if (jsonObjectData.contains("temporary") && !jsonObjectData.at("temporary").is_null()) {
            bool theValue = jsonObjectData.at("temporary");
            inviteData.temporary = theValue;
        }

        if (jsonObjectData.contains("uses") && !jsonObjectData.at("uses").is_null()) {
            unsigned int theValue = jsonObjectData.at("uses");
            inviteData.uses = theValue;
        }

        *pDataStructure = inviteData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageReferenceData* pDataStructure) {
        DiscordCoreAPI::MessageReferenceData messageReferenceData = *pDataStructure;

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            string theValue = jsonObjectData.at("message_id");
            messageReferenceData.messageId = theValue;
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            messageReferenceData.channelId = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            messageReferenceData.guildId = theValue;
        }

        if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData.at("fail_if_not_exists").is_null()) {
            bool theValue = jsonObjectData.at("fail_if_not_exists");
            messageReferenceData.failIfNotExists = theValue;
        }

        *pDataStructure = messageReferenceData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageStickerData* pDataStructure) {
        DiscordCoreAPI::MessageStickerData messageStickerData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            messageStickerData.id = theValue;
        }

        if (jsonObjectData.contains("pack_id") && !jsonObjectData.at("pack_id").is_null()) {
            string theValue = jsonObjectData.at("pack_id");
            messageStickerData.packId = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            messageStickerData.name = theValue;
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            string theValue = jsonObjectData.at("description");
            messageStickerData.description = theValue;
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
            string theValue = jsonObjectData.at("tags");
            messageStickerData.tags = theValue;
        }

        if (jsonObjectData.contains("asset") && !jsonObjectData.at("asset").is_null()) {
            string theValue = jsonObjectData.at("asset");
            messageStickerData.asset = theValue;
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
            int theValue = jsonObjectData.at("format_type");
            messageStickerData.formatType = theValue;
        }

        *pDataStructure = messageStickerData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::AllowedMentionsData* pDataStructure) {
        DiscordCoreAPI::AllowedMentionsData allowedMentionsData = *pDataStructure;

        if (jsonObjectData.contains("parse") && !jsonObjectData.at("parse").is_null()) {
            json stringsArray = jsonObjectData.at("parse");
            for (unsigned int x = 0; x < stringsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < allowedMentionsData.parse.size(); y += 1) {
                    if (stringsArray.at(x) == allowedMentionsData.parse.at(y)) {
                        isItFound = true;
                        break;
                    }
                }
                if (isItFound == false) {
                    allowedMentionsData.parse.push_back(stringsArray.at(x));
                }
            }
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            json stringsArray = jsonObjectData.at("roles");
            for (unsigned int x = 0; x < stringsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < allowedMentionsData.roles.size(); y += 1) {
                    if (stringsArray.at(x) == allowedMentionsData.roles.at(y)) {
                        isItFound = true;
                        break;
                    }
                }
                if (isItFound == false) {
                    allowedMentionsData.roles.push_back(stringsArray.at(x));
                }
            }
        }

        if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
            json stringsArray = jsonObjectData.at("users");
            for (unsigned int x = 0; x < stringsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < allowedMentionsData.users.size(); y += 1) {
                    if (stringsArray.at(x) == allowedMentionsData.users.at(y)) {
                        isItFound = true;
                        break;
                    }
                }
                if (isItFound == false) {
                    allowedMentionsData.users.push_back(stringsArray.at(x));
                }
            }
        }

        if (jsonObjectData.contains("replied_user") && !jsonObjectData.at("replied_user").is_null()) {
            bool theValue = jsonObjectData.at("replied_user");
            allowedMentionsData.repliedUser = theValue;
        }

        *pDataStructure = allowedMentionsData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionApplicationCommandCallbackData* pDataStructure) {
        DiscordCoreAPI::InteractionApplicationCommandCallbackData interactionAppCommandCallbackData = *pDataStructure;

        if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
            bool theValue = jsonObjectData.at("tts");
            interactionAppCommandCallbackData.tts = theValue;
        }

        if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
            string theValue = jsonObjectData.at("content");
            interactionAppCommandCallbackData.content = theValue;
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
            json embedsArray = jsonObjectData.at("embeds");
            for (unsigned int x = 0; x < embedsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < interactionAppCommandCallbackData.embeds.size(); y += 1) {
                    if (interactionAppCommandCallbackData.embeds.at(y).author.name == embedsArray.at(x).at("author").at("name")) {
                        isItFound = true;
                        DiscordCoreAPI::EmbedData embedData = interactionAppCommandCallbackData.embeds.at(y);
                        parseObject(embedsArray.at(x), &embedData);
                        interactionAppCommandCallbackData.embeds.erase(interactionAppCommandCallbackData.embeds.begin() + y);
                        interactionAppCommandCallbackData.embeds.push_back(embedData);
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::EmbedData embedData;
                    parseObject(embedsArray.at(x), &embedData);
                    interactionAppCommandCallbackData.embeds.push_back(embedData);
                }
            }
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            int theValue = jsonObjectData.at("flags");
            interactionAppCommandCallbackData.flags = theValue;
        }

        if (jsonObjectData.contains("allowed_mentions") && !jsonObjectData.at("allowed_mentions").is_null()) {
            DiscordCoreAPI::AllowedMentionsData theValue = interactionAppCommandCallbackData.allowedMentions;
            parseObject(jsonObjectData.at("allowed_mentions"), &theValue);
            interactionAppCommandCallbackData.allowedMentions = theValue;
        }

        *pDataStructure = interactionAppCommandCallbackData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionResponseData* pDataStructure) {
        DiscordCoreAPI::InteractionResponseData interactionData = *pDataStructure;

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            DiscordCoreAPI::InteractionCallbackType theValue = jsonObjectData.at("type");
            interactionData.type = theValue;
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            DiscordCoreAPI::InteractionApplicationCommandCallbackData theValue = interactionData.data;
            parseObject(jsonObjectData.at("data"), &theValue);
            interactionData.data = theValue;
        }

        *pDataStructure = interactionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionAddEventData* pDataStructure) {
        DiscordCoreAPI::ReactionAddEventData reactionAddData = *pDataStructure;

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            string theValue = jsonObjectData.at("user_id");
            reactionAddData.userId = theValue;
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            reactionAddData.channelId = theValue;
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            string theValue = jsonObjectData.at("message_id");
            reactionAddData.messageId = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            reactionAddData.guildId = theValue;
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            DiscordCoreAPI::GuildMemberData theValue = reactionAddData.member;
            parseObject(jsonObjectData.at("member"), &theValue);
            reactionAddData.member = theValue;
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            DiscordCoreAPI::EmojiData theValue = reactionAddData.emoji;
            parseObject(jsonObjectData.at("emoji"), &theValue);
            reactionAddData.emoji = theValue;
        }

        *pDataStructure = reactionAddData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ActionRowData* pDataStructure) {
        DiscordCoreAPI::ActionRowData actionRowData = *pDataStructure;

        if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
            for (auto& value : jsonObjectData) {
                DiscordCoreAPI::ComponentData componentData;

                if (value.contains("type") && !value.at("type").is_null()) {
                    DiscordCoreAPI::ComponentType theValue = value.at("type");
                    componentData.type = theValue;
                }

                if (value.contains("custom_id") && !value.at("custom_id").is_null()) {
                    string theValue = value.at("custom_id");
                    componentData.customId = theValue;
                }

                if (value.contains("style") && !value.at("style").is_null()) {
                    DiscordCoreAPI::ButtonStyle theValue = value.at("style");
                    componentData.style = theValue;
                }

                if (value.contains("label") && !value.at("label").is_null()) {
                    string theValue = value.at("label");
                    componentData.label = theValue;
                }

                if (value.contains("emoji") && !value.at("emoji").is_null()) {
                    DiscordCoreAPI::EmojiData theValue;
                    parseObject(value.at("emoji"), &theValue);
                    componentData.emoji = theValue;
                }

                if (value.contains("url") && !value.at("url").is_null()) {
                    string theValue = value.at("url");
                    componentData.url = theValue;
                }

                if (value.contains("disabled") && !value.at("disabled").is_null()) {
                    bool theValue = value.at("disabled");
                    componentData.disabled = theValue;
                }

                actionRowData.components.push_back(componentData);
            }
        }
        *pDataStructure = actionRowData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageInteractionData* pDataStructure) {
        DiscordCoreAPI::MessageInteractionData messageInteractionData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            messageInteractionData.id = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            DiscordCoreAPI::InteractionType theValue = jsonObjectData.at("type");
            messageInteractionData.type = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            messageInteractionData.name = theValue;
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData theValue = messageInteractionData.user;
            parseObject(jsonObjectData.at("user"), &theValue);
            messageInteractionData.user = theValue;
        }

        *pDataStructure = messageInteractionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageData* pDataStructure) {
        DiscordCoreAPI::MessageData messageData = *pDataStructure;

        if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
            messageData.content = jsonObjectData.at("content").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            messageData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            messageData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            messageData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
            DiscordCoreAPI::UserData theValue = messageData.author;
            parseObject(jsonObjectData.at("author"), &theValue);
            messageData.author = theValue;
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            DiscordCoreAPI::GuildMemberData theValue = messageData.member;
            parseObject(jsonObjectData.at("member"), &theValue);
            messageData.member = theValue;
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            messageData.timestamp = jsonObjectData.at("timestamp").get<string>();
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
            messageData.editedTimestamp = jsonObjectData.at("edited_timestamp").get<string>();
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
            messageData.tts = jsonObjectData.at("tts").get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
            messageData.mentionEveryone = jsonObjectData.at("mention_everyone").get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
            vector<DiscordCoreAPI::UserData> newVector;
            for (auto value: jsonObjectData.at("mentions")) {
                DiscordCoreAPI::UserData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            messageData.mentions = newVector;
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
            vector<string> newVector;
            for (auto value: jsonObjectData.at("mention_roles")) {
                newVector.push_back(value);
            }
            messageData.mentionRoles = newVector;
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
            vector<DiscordCoreAPI::ChannelMentionData> newVector;
            for (auto value: jsonObjectData.at("mention_channels")) {
                DiscordCoreAPI::ChannelMentionData newData;
                parseObject(value, &newData);
                newVector.push_back(newData);
            }
            messageData.mentionChannels = newVector;
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
            json attachmentsArray = jsonObjectData.at("attachments");
            for (unsigned int x = 0; x < attachmentsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < messageData.attachments.size(); y += 1) {
                    if (messageData.attachments.at(y).id == attachmentsArray.at(x).at("id")) {
                        isItFound = true;
                        DiscordCoreAPI::AttachmentData attachmentData = messageData.attachments.at(y);
                        parseObject(attachmentsArray.at(x), &attachmentData);
                        messageData.attachments.erase(messageData.attachments.begin() + y);
                        messageData.attachments.push_back(attachmentData);
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::AttachmentData attachmentData;
                    parseObject(attachmentsArray.at(x), &attachmentData);
                    messageData.attachments.push_back(attachmentData);
                }
            }
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
            json embedsArray = jsonObjectData.at("embeds");
            for (unsigned int x = 0; x < embedsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < messageData.embeds.size(); y += 1) {
                    if (embedsArray.at(x).contains("author")) {
                        if (messageData.embeds.at(y).author.name == embedsArray.at(x).at("author").at("name")) {
                            isItFound = true;
                            DiscordCoreAPI::EmbedData embedData = messageData.embeds.at(y);
                            parseObject(embedsArray.at(x), &embedData);
                            messageData.embeds.erase(messageData.embeds.begin() + y);
                            messageData.embeds.push_back(embedData);
                        }
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::EmbedData embedData;
                    parseObject(embedsArray.at(x), &embedData);
                    messageData.embeds.push_back(embedData);
                }
            }
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData.at("reactions").is_null()) {
            json reactionsArray = jsonObjectData.at("reactions");
            for (unsigned int x = 0; x < reactionsArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < messageData.reactions.size(); y += 1) {
                    if (messageData.reactions.at(y).emoji.id == reactionsArray.at(x).at("emoji").at("id")) {
                        isItFound = true;
                        DiscordCoreAPI::ReactionData reactionData = messageData.reactions.at(y);
                        parseObject(reactionsArray.at(x), &reactionData);
                        messageData.reactions.erase(messageData.reactions.begin() + y);
                        messageData.reactions.push_back(reactionData);
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::ReactionData reactionData;
                    parseObject(reactionsArray.at(x), &reactionData);
                    messageData.reactions.push_back(reactionData);
                }
            }
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
            string theValue = jsonObjectData.at("nonce");
            messageData.nonce = theValue;
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
            bool theValue = jsonObjectData.at("pinned");
            messageData.pinned = theValue;
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
            string theValue = jsonObjectData.at("webhook_id");
            messageData.webhookId = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            int theValue = jsonObjectData.at("type");
            messageData.type = (DiscordCoreAPI::MessageType)theValue;
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
            DiscordCoreAPI::MessageActivityData theValue = messageData.activity;
            parseObject(jsonObjectData.at("activity"), &theValue);
            messageData.activity = theValue;
        }

        if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
            DiscordCoreAPI::ApplicationData theValue = messageData.application;
            parseObject(jsonObjectData.at("application"), &theValue);
            messageData.application = theValue;
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData.at("message_reference").is_null()) {
            DiscordCoreAPI::MessageReferenceData theValue = messageData.messageReference;
            parseObject(jsonObjectData.at("message_reference"), &theValue);
            messageData.messageReference = theValue;
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            int theValue = jsonObjectData.at("flags");
            messageData.flags = theValue;
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
            json stickersArray = jsonObjectData.at("stickers");
            for (unsigned int x = 0; x < stickersArray.size(); x += 1) {
                bool isItFound = false;
                for (unsigned int y = 0; y < messageData.stickers.size(); y += 1) {
                    if (messageData.stickers.at(y).id == stickersArray.at(x).at("id")) {
                        isItFound = true;
                        DiscordCoreAPI::MessageStickerData messageStickerData = messageData.stickers.at(y);
                        parseObject(stickersArray.at(x), &messageStickerData);
                        messageData.stickers.erase(messageData.stickers.begin() + y);
                        messageData.stickers.push_back(messageStickerData);
                    }
                }
                if (isItFound == false) {
                    DiscordCoreAPI::MessageStickerData messageStickerData;
                    parseObject(stickersArray.at(x), &messageStickerData);
                    messageData.stickers.push_back(messageStickerData);
                }
            }
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
            DiscordCoreAPI::MessageInteractionData theValue = messageData.interaction;
            parseObject(jsonObjectData.at("interaction"), &theValue);
            messageData.interaction = theValue;
        }

        if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
            for (auto& value : jsonObjectData.at("components")) {
                DiscordCoreAPI::ActionRowData actionRowData;
                parseObject(value, &actionRowData);
                messageData.components.push_back(actionRowData);
            }
        }

        *pDataStructure = messageData;
    }  

    void parseObject(json jsonObjectData, ApplicationCommandOptionChoiceData* pDataStructure) {
        ApplicationCommandOptionChoiceData appCommandOptionChoiceData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            appCommandOptionChoiceData.name = theValue;
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_string()) {
            string theValue = jsonObjectData.at("value");
            appCommandOptionChoiceData.valueString = theValue;
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_number_integer()) {
            int theValue = jsonObjectData.at("value");
            appCommandOptionChoiceData.valueInt = theValue;
        }

        *pDataStructure = appCommandOptionChoiceData;
    }

    void parseObject(json jsonObjectData, ApplicationCommandOptionData* pDataStructure) {
        ApplicationCommandOptionData appCommandOptionData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            appCommandOptionData.name = theValue;
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            string theValue = jsonObjectData.at("description");
            appCommandOptionData.description = theValue;
        }

        if (jsonObjectData.contains("required") && !jsonObjectData.at("required").is_null()) {
            bool theValue = jsonObjectData.at("required");
            appCommandOptionData.required = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            ApplicationCommandOptionType theValue = (ApplicationCommandOptionType)jsonObjectData.at("type");
            appCommandOptionData.type = theValue;
        }

        if (appCommandOptionData.type == ApplicationCommandOptionType::SUB_COMMAND_GROUP || appCommandOptionData.type == ApplicationCommandOptionType::SUB_COMMAND) {
            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                for (unsigned int x = 0; x < jsonObjectData.at("options").size(); x += 1) {
                    ApplicationCommandOptionData appCommandOptionDataNew;
                    parseObject(jsonObjectData.at("options").at(x), &appCommandOptionDataNew);
                    appCommandOptionData.options.push_back(appCommandOptionDataNew);
                }
            }
        }
        else {
            if (jsonObjectData.contains("choices") && !jsonObjectData.at("choices").is_null()) {
                for (unsigned int x = 0; x < jsonObjectData.at("choices").size(); x += 1) {
                    ApplicationCommandOptionChoiceData appCommandChoiceData;
                    parseObject(jsonObjectData.at("choices").at(x), &appCommandChoiceData);
                    appCommandOptionData.choices.push_back(appCommandChoiceData);
                }
            }
        }

        *pDataStructure = appCommandOptionData;
    }

    void parseObject(json jsonObjectData, ApplicationCommandData* pDataStructure) {
        ApplicationCommandData appCommandData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            appCommandData.id = theValue;
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            string theValue = jsonObjectData.at("application_id");
            appCommandData.applicationId = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            appCommandData.name = theValue;
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            string theValue = jsonObjectData.at("description");
            appCommandData.description = theValue;
        }

        if (jsonObjectData.contains("default_permission") && !jsonObjectData.at("default_permission").is_null()) {
            bool theValue = jsonObjectData.at("default_permission");
            appCommandData.defaultPermission = theValue;
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            json theValue = jsonObjectData.at("options");
            for (unsigned int x = 0; x < theValue.size(); x += 1) {
                ApplicationCommandOptionData appCommandOptionData;
                parseObject(theValue.at(x), &appCommandOptionData);
                appCommandData.options.push_back(appCommandOptionData);
            }
        }

        *pDataStructure = appCommandData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionDataOption* pDataStructure) {
        DiscordCoreAPI::ApplicationCommandInteractionDataOption appCommandInteractionDataOption = *pDataStructure;
        
        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            appCommandInteractionDataOption.name = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            DiscordCoreAPI::ApplicationCommandOptionType theValue = jsonObjectData.at("type");
            appCommandInteractionDataOption.type = theValue;
        }
        
        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
            if (jsonObjectData.at("value").is_boolean()) {
                bool theValue = jsonObjectData.at("value");
                appCommandInteractionDataOption.valueBool = theValue;
            }
            if (jsonObjectData.at("value").is_string()) {
                string theValue = jsonObjectData.at("value");
                appCommandInteractionDataOption.valueString = theValue;
            }
            if (jsonObjectData.at("value").is_number()) {
                int theValue = jsonObjectData.at("value");
                appCommandInteractionDataOption.valueInt = theValue;
            }
        }
        
        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            json optionsArray = jsonObjectData.at("options");
            if (optionsArray.size() > 0){
                for (const auto& value : optionsArray) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption option;
                    parseObject(value, &option);
                    appCommandInteractionDataOption.options.push_back(option);
                }
            }            
        }
        
        *pDataStructure = appCommandInteractionDataOption;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionData* pDataStructure) {
        DiscordCoreAPI::ApplicationCommandInteractionData appCommandInteractionData = *pDataStructure;
        
        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            appCommandInteractionData.name = theValue;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            appCommandInteractionData.id = theValue;
        }

        if (jsonObjectData.contains("custom_id") && !jsonObjectData.at("custom_id").is_null()) {
            string theValue = jsonObjectData.at("custom_id");
            appCommandInteractionData.customId = theValue;
        }

        if (jsonObjectData.contains("component_type") && !jsonObjectData.at("component_type").is_null()) {
            int theValue = jsonObjectData.at("component_type");
            appCommandInteractionData.componentType = theValue;
        }
        
        if (jsonObjectData.contains("resolved") && !jsonObjectData.at("resolved").is_null()) {
            auto theValue = jsonObjectData.at("resolved");
            if (theValue.contains("users") && !theValue.at("users").is_null()) {
                auto theUserValue = theValue.at("users").get<map<string, json>>();
                for (const auto& [key, value] : theUserValue) {
                    DiscordCoreAPI::UserData userData;
                    parseObject(value, &userData);
                    appCommandInteractionData.resolved.users.insert(make_pair(key, userData));
                }
            }
            if (theValue.contains("channels") && !theValue.at("channels").is_null()) {
                auto theChannelValue = theValue.at("channels").get<map<string, json>>();
                for (const auto& [key, value] : theChannelValue) {
                    DiscordCoreAPI::ChannelData channelData;
                    parseObject(value, &channelData);
                    appCommandInteractionData.resolved.channels.insert(make_pair(key, channelData));
                }
            }
            if (theValue.contains("roles") && !theValue.at("roles").is_null()) {
                auto theRoleValue = theValue.at("roles").get<map<string, json>>();
                for (const auto& [key, value] : theRoleValue) {
                    DiscordCoreAPI::RoleData roleData;
                    parseObject(value, &roleData);
                    appCommandInteractionData.resolved.roles.insert(make_pair(key, roleData));
                }
            }
            if (theValue.contains("members") && !theValue.at("members").is_null()) {
                auto theMemberValue = theValue.at("members").get<map<string, json>>();
                for (const auto& [key, value] : theMemberValue) {
                    DiscordCoreAPI::GuildMemberData guildMemberData;
                    parseObject(value, &guildMemberData);
                    appCommandInteractionData.resolved.members.insert(make_pair(key, guildMemberData));
                }
            }
        }
        
        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            json optionsArray = jsonObjectData.at("options");
            if (optionsArray.size() > 0) {
                for (auto& value : optionsArray) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption option;
                    parseObject(value, &option);
                    appCommandInteractionData.options.push_back(option);
                }
            }            
        }
        
        *pDataStructure = appCommandInteractionData;
    }

    void parseObject(json jsonObjectData, CommandData* pDataStructure) {
        CommandData commandData = *pDataStructure;

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            json::array_t theValue = jsonObjectData.at("options");
            for (auto& value : theValue) {
                if (value.contains("value") && !value.at("value").is_null()) {
                    auto valueNew = value.at("value");
                    if (valueNew.is_string()) {
                        commandData.optionsArgs.push_back(valueNew);
                    }
                    else if (valueNew.is_number()) {
                        commandData.optionsArgs.push_back(to_string(valueNew));
                    }

                }
            }
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            commandData.commandName = theValue;
        }

        *pDataStructure = commandData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionData* pDataStructure) {
        DiscordCoreAPI::InteractionData interactionData = *pDataStructure;

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            DiscordCoreAPI::ApplicationCommandInteractionData theValue = interactionData.data;
            parseObject(jsonObjectData.at("data"), &theValue);
            interactionData.data = theValue;
            if (jsonObjectData.at("data").contains("custom_id") && !jsonObjectData.at("data").at("custom_id").is_null()) {
                interactionData.customId = jsonObjectData.at("data").at("custom_id");
            }
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            json theValue = jsonObjectData.at("data");
            interactionData.dataRaw = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            DiscordCoreAPI::InteractionType theValue = jsonObjectData.at("type");
            interactionData.type = theValue;
        }        

        if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
            string theValue = jsonObjectData.at("token");
            interactionData.token = theValue;
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            DiscordCoreAPI::GuildMemberData theValue = interactionData.member;
            parseObject(jsonObjectData.at("member"), &theValue);
            interactionData.member = theValue;
            interactionData.user = interactionData.member.user;
        }
        else if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData theValue = interactionData.user;
            parseObject(jsonObjectData.at("user"), &theValue);
            interactionData.user = theValue;
        }
        
        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            interactionData.channelId = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            interactionData.guildId = theValue;
        }

        if (jsonObjectData.contains("message") && !jsonObjectData.at("message").is_null()) {
            DiscordCoreAPI::MessageData theValue = interactionData.message;
            parseObject(jsonObjectData.at("message"), &theValue);
            interactionData.message = theValue;
        }

        if (jsonObjectData.contains("version") && !jsonObjectData.at("version").is_null()) {
            int theValue = jsonObjectData.at("version");
            interactionData.version = theValue;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            interactionData.id = theValue;
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            string theValue = jsonObjectData.at("application_id");
            interactionData.applicationId = theValue;
            
        }
        
        *pDataStructure = interactionData;
    
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WebhookData* pDataStructure) {
        DiscordCoreAPI::WebhookData webhookData;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            webhookData.id = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            unsigned int theValue = jsonObjectData.at("id");
            webhookData.type = theValue;
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            string theValue = jsonObjectData.at("guild_id");
            webhookData.guildId = theValue;
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            webhookData.channelId = theValue;
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            DiscordCoreAPI::UserData theValue;
            parseObject(jsonObjectData.at("user"), &theValue);
            webhookData.user = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            webhookData.name = theValue;
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
            string theValue = jsonObjectData.at("avatar");
            webhookData.avatar = theValue;
        }

        if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
            string theValue = jsonObjectData.at("token");
            webhookData.token = theValue;
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            string theValue = jsonObjectData.at("application_id");
            webhookData.applicationId = theValue;
        }

        if (jsonObjectData.contains("source_guild") && !jsonObjectData.at("source_guild").is_null()) {
            DiscordCoreAPI::GuildData theValue;
            parseObject(jsonObjectData.at("source_guild"), &theValue);
            webhookData.sourceGuild = theValue;
        }

        if (jsonObjectData.contains("source_channel") && !jsonObjectData.at("source_channel").is_null()) {
            DiscordCoreAPI::ChannelData theValue;
            parseObject(jsonObjectData.at("source_channel"), &theValue);
            webhookData.sourceChannel = theValue;
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            string theValue = jsonObjectData.at("url");
            webhookData.url = theValue;
        }

        *pDataStructure = webhookData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogChangeData* pDataStructure) {
        DiscordCoreAPI::AuditLogChangeData changeData;

        if (jsonObjectData.contains("new_value") && !jsonObjectData.at("new_value").is_null()) {
            string theValue = jsonObjectData.at("new_value").dump();
            changeData.newValueString = theValue;
        }

        if (jsonObjectData.contains("old_value") && !jsonObjectData.at("old_value").is_null()) {
            string theValue = jsonObjectData.at("old_value").dump();
            changeData.oldValueString = theValue;
        }

        if (jsonObjectData.contains("key") && !jsonObjectData.at("key").is_null()) {
            string theValue = jsonObjectData.at("key");
            changeData.key = theValue;
        }

        *pDataStructure = changeData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryInfoData* pDataStructure) {
        DiscordCoreAPI::AuditLogEntryInfoData entryInfoData;

        if (jsonObjectData.contains("delete_member_days") && !jsonObjectData.at("delete_member_days").is_null()) {
            string theValue = jsonObjectData.at("delete_member_days");
            entryInfoData.deleteMemberDays = theValue;
        }

        if (jsonObjectData.contains("members_removed") && !jsonObjectData.at("members_removed").is_null()) {
            string theValue = jsonObjectData.at("members_removed");
            entryInfoData.membersRemoved = theValue;
        }
        
        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            string theValue = jsonObjectData.at("channel_id");
            entryInfoData.channelId = theValue;
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            string theValue = jsonObjectData.at("message_id");
            entryInfoData.messageId = theValue;
        }

        if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
            string theValue = jsonObjectData.at("count");
            entryInfoData.count = theValue;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            entryInfoData.id = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            string theValue = jsonObjectData.at("type");
            entryInfoData.type = theValue;
        }

        if (jsonObjectData.contains("role_name") && !jsonObjectData.at("role_name").is_null()) {
            string theValue = jsonObjectData.at("role_name");
            entryInfoData.roleName = theValue;
        }

        *pDataStructure = entryInfoData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryData* pDataStructure) {
        DiscordCoreAPI::AuditLogEntryData entryData;

        if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
            string theValue = jsonObjectData.at("target_id");
            entryData.targetId = theValue;
        }
        
        if (jsonObjectData.contains("changes") && !jsonObjectData.at("changes").is_null()) {
            for (auto value : jsonObjectData.at("changes")) {
                DiscordCoreAPI::AuditLogChangeData changesData;
                parseObject(value, &changesData);
                entryData.changes.push_back(changesData);
            }
        }
       
        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            string theValue = jsonObjectData.at("user_id");
            entryData.userId = theValue;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            entryData.id = theValue;
        }

        if (jsonObjectData.contains("action_type") && !jsonObjectData.at("action_type").is_null()) {
            AuditLogEvent theValue = jsonObjectData.at("action_type");
            entryData.actionType = (DiscordCoreAPI::AuditLogEvent)theValue;
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            DiscordCoreAPI::AuditLogEntryInfoData theValue;
            parseObject(jsonObjectData.at("options"), &theValue);
            entryData.options = theValue;
        }

        if (jsonObjectData.contains("reason") && !jsonObjectData.at("reason").is_null()) {
            string theValue = jsonObjectData.at("reason");
            entryData.reason = theValue;
        }

        *pDataStructure = entryData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AccountData* pDataStructure) {
        DiscordCoreAPI::AccountData accountData;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            accountData.id= theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            accountData.name = theValue;
        }

        *pDataStructure = accountData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::IntegrationData* pDataStructure) {
        DiscordCoreAPI::IntegrationData integrationData;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            string theValue = jsonObjectData.at("id");
            integrationData.id = theValue;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            string theValue = jsonObjectData.at("name");
            integrationData.name = theValue;
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            string theValue = jsonObjectData.at("type");
            integrationData.type = theValue;
        }

        if (jsonObjectData.contains("enabled") && !jsonObjectData.at("enabled").is_null()) {
            bool theValue = jsonObjectData.at("enabled");
            integrationData.enabled = theValue;
        }

        if (jsonObjectData.contains("syncing") && !jsonObjectData.at("syncing").is_null()) {
            bool theValue = jsonObjectData.at("syncing");
            integrationData.syncing = theValue;
        }

        if (jsonObjectData.contains("role_id") && !jsonObjectData.at("role_id").is_null()) {
            string theValue = jsonObjectData.at("role_id");
            integrationData.roleId = theValue;
        }

        if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData.at("enable_emoticons").is_null()) {
            bool theValue = jsonObjectData.at("enable_emoticons");
            integrationData.enableEmoticons = theValue;
        }

        if (jsonObjectData.contains("expire_behavior") && !jsonObjectData.at("expire_behavior").is_null()) {
            unsigned int theValue = jsonObjectData.at("expire_behavior");
            integrationData.expireBehavior = theValue;
        }

        if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData.at("expire_grace_period").is_null()) {
            unsigned int theValue = jsonObjectData.at("expire_grace_period");
            integrationData.expireGracePeriod = theValue;
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            DiscordCoreAPI::UserData theValue;
            parseObject(jsonObjectData.at("user"), &theValue);
            integrationData.user = theValue;
        }

        if (jsonObjectData.contains("account") && !jsonObjectData.at("account").is_null()) {
            DiscordCoreAPI::AccountData theValue;
            parseObject(jsonObjectData.at("account"), &theValue);
            integrationData.account = theValue;
        }

        if (jsonObjectData.contains("synced_at") && !jsonObjectData.at("synced_at").is_null()) {
            string theValue = jsonObjectData.at("synced_at");
            integrationData.syncedAt = theValue;
        }

        if (jsonObjectData.contains("subscriber_count") && !jsonObjectData.at("subscriber_count").is_null()) {
            unsigned int theValue = jsonObjectData.at("subscriber_count");
            integrationData.subscriberCount = theValue;
        }

        if (jsonObjectData.contains("revoked") && !jsonObjectData.at("revoked").is_null()) {
            bool theValue = jsonObjectData.at("revoked");
            integrationData.revoked = theValue;
        }

        if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
            DiscordCoreAPI::ApplicationData theValue;
            parseObject(jsonObjectData.at("application"), &theValue);
            integrationData.application = theValue;
        }

        *pDataStructure = integrationData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogData* pDataStructure) {
        DiscordCoreAPI::AuditLogData auditLogData;

        if (jsonObjectData.contains("webhooks") && !jsonObjectData.at("webhooks").is_null()) {
            for (auto theValue : jsonObjectData.at("webhooks")) {
                DiscordCoreAPI::WebhookData webhookData;
                parseObject(theValue, &webhookData);
                auditLogData.webhooks.push_back(webhookData);
            }
        }

        if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
            for (auto theValue : jsonObjectData.at("users")) {
                DiscordCoreAPI::UserData userData;
                parseObject(theValue, &userData);
                auditLogData.users.push_back(userData);
            }
        }
        
        if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData.at("audit_log_entries").is_null()) {
            for (auto theValue : jsonObjectData.at("audit_log_entries")) {
                DiscordCoreAPI::AuditLogEntryData entryData;
                parseObject(theValue, &entryData);
                auditLogData.auditLogEntries.push_back(entryData);
            }
        }
        
        if (jsonObjectData.contains("integrations") && !jsonObjectData.at("integrations").is_null()) {
            for (auto theValue : jsonObjectData.at("integrations")) {
                DiscordCoreAPI::IntegrationData integrationData;
                parseObject(theValue, &integrationData);
                auditLogData.integrations.push_back(integrationData);
            }
        }
        
        *pDataStructure = auditLogData;
    }

};
#endif