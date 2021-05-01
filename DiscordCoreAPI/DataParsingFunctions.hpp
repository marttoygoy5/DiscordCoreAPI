// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "ClientDataTypes.hpp"

namespace CommanderNS {
	
    namespace DataParsingFunctions {

        void parseObject(json jsonObjectData, ClientDataTypes::OverWriteData * pDataStructure) {
            ClientDataTypes::OverWriteData overWriteData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                overWriteData.id = theValue;
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                overWriteData.type = theValue;
            }

            if (jsonObjectData.contains("allow") && !jsonObjectData.at("allow").is_null()) {
                std::string theValue = jsonObjectData.at("allow");
                overWriteData.allow = theValue;
            }

            if (jsonObjectData.contains("deny") && !jsonObjectData.at("deny").is_null()) {
                std::string theValue = jsonObjectData.at("deny");
                overWriteData.deny = theValue;
            }
            
            *pDataStructure = overWriteData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::UserData* pDataStructure) {
            ClientDataTypes::UserData userData = *pDataStructure;

            if (jsonObjectData.contains("username") && !jsonObjectData.at("username").is_null()) {
                std::string theValue = jsonObjectData.at("username");
                userData.username = theValue;
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                userData.id = theValue;
            }

            if (jsonObjectData.contains("discriminator") && !jsonObjectData.at("discriminator").is_null()) {
                std::string theValue = jsonObjectData.at("discriminator");
                userData.discriminator = theValue;
            }

            if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
                std::string theValue = jsonObjectData.at("avatar");
                userData.avatar = theValue;
            }

            if (jsonObjectData.contains("bot") && !jsonObjectData.at("bot").is_null()) {
                bool theValue = jsonObjectData.at("bot");
                userData.bot = theValue;
            }

            if (jsonObjectData.contains("system") && !jsonObjectData.at("system").is_null()) {
                bool theValue = jsonObjectData.at("system");
                userData.system = theValue;
            }

            if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData.at("mfa_enabled").is_null()) {
                bool theValue = jsonObjectData.at("mfa_enabled");
                userData.mfa_enabled = theValue;
            }

            if (jsonObjectData.contains("locale") && !jsonObjectData.at("locale").is_null()) {
                std::string theValue = jsonObjectData.at("locale");
                userData.locale = theValue;
            }

            if (jsonObjectData.contains("verified") && !jsonObjectData.at("verified").is_null()) {
                bool theValue = jsonObjectData.at("verified");
                userData.verified = theValue;
            }

            if (jsonObjectData.contains("email") && !jsonObjectData.at("email").is_null()) {
                std::string theValue = jsonObjectData.at("email");
                userData.email = theValue;
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                int theValue = jsonObjectData.at("flags");
                userData.flags = theValue;
            }

            if (jsonObjectData.contains("premium_type") && !jsonObjectData.at("premium_type").is_null()) {
                int theValue = jsonObjectData.at("premium_type");
                userData.premium_type = theValue;
            }

            if (jsonObjectData.contains("public_flags") && !jsonObjectData.at("public_flags").is_null()) {
                int theValue = jsonObjectData.at("public_flags");
                userData.public_flags = theValue;
            }

            *pDataStructure = userData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::ChannelData* pDataStructure) {
            ClientDataTypes::ChannelData channelData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                channelData.id = theValue;
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                channelData.type = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                channelData.guildId = theValue;
            }

            if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
                int theValue = jsonObjectData.at("position");
                channelData.position = theValue;
            }

            if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData.at("permission_overwrites").is_null()) {
                json overWritesArray = jsonObjectData.at("permission_overwrites");
                for (unsigned int x = 0; x < overWritesArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < channelData.permissionOverwrites.size(); y += 1) {
                        if (channelData.permissionOverwrites.at(y).id == overWritesArray.at(x).at("id")) {
                            isItFound = true;
                            ClientDataTypes::OverWriteData overWriteData;
                            parseObject(overWritesArray.at(x), &overWriteData);
                            channelData.permissionOverwrites.erase(channelData.permissionOverwrites.begin() + y);
                            channelData.permissionOverwrites.push_back(overWriteData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::OverWriteData overWriteData;
                        parseObject(overWritesArray.at(x), &overWriteData);
                        channelData.permissionOverwrites.push_back(overWriteData);
                    }
                }
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                channelData.name = theValue;
            }           

            if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
                std::string theValue = jsonObjectData.at("topic");
                channelData.topic = theValue;
            }

            if (jsonObjectData.contains("nsfw") && !jsonObjectData.at("nsfw").is_null()) {
                bool theValue = jsonObjectData.at("nsfw");
                channelData.nsfw = theValue;
            }

            if (jsonObjectData.contains("last_message_id") && !jsonObjectData.at("last_message_id").is_null()) {
                std::string theValue = jsonObjectData.at("last_message_id");
                channelData.lastMessageId = theValue;
            }
           
            if (jsonObjectData.contains("bitrate") && !jsonObjectData.at("bitrate").is_null()) {
                int theValue = jsonObjectData.at("bitrate");
                channelData.bitrate = theValue;
            }

            if (jsonObjectData.contains("user_limit") && !jsonObjectData.at("user_limit").is_null()) {
                int theValue = jsonObjectData.at("user_limit");
                channelData.userLimit = theValue;
            }

            if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData.at("rate_limit_per_user").is_null()) {
                int theValue = jsonObjectData.at("rate_limit_per_user");
                channelData.rateLimitPerUser = theValue;
            }

            if (jsonObjectData.contains("recipients") && !jsonObjectData.at("recipients").is_null()) {
                json userArray = jsonObjectData.at("recipients");
                for (int x = 0; x < userArray.size(); x += 1) {
                    bool isItFound = false;
                    for (int y = 0; y < channelData.recipients.size(); y += 1) {
                        if (channelData.recipients.at(y).id == userArray.at(x).at("id")) {
                            isItFound = true;
                            ClientDataTypes::UserData userData = channelData.recipients.at(y);
                            parseObject(userArray.at(x), &userData);
                            channelData.recipients.erase(channelData.recipients.begin() + y);
                            channelData.recipients.push_back(userData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::UserData userData;
                        parseObject(userArray.at(x), &userData);
                        channelData.recipients.push_back(userData);
                    }
                }
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                std::string theValue = jsonObjectData.at("icon");
                channelData.icon = theValue;
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
                std::string theValue = jsonObjectData.at("owner_id");
                channelData.ownerId = theValue;
            }
           
            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                std::string theValue = jsonObjectData.at("application_id");
                channelData.applicationId = theValue;
            }

            if (jsonObjectData.contains("parent_id") && !jsonObjectData.at("parent_id").is_null()) {
                std::string theValue = jsonObjectData.at("parent_id");
                channelData.parentId = theValue;
            }

            if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData.at("last_pin_timestamp").is_null()) {
                std::string theValue = jsonObjectData.at("last_pin_timestamp");
                channelData.lastPinTimestamp = theValue;
            }

            if (jsonObjectData.contains("rtc_region") && !jsonObjectData.at("rtc_region").is_null()) {
                std::string theValue = jsonObjectData.at("rtc_region");
                channelData.rtcRegion = theValue;
            }

            if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData.at("video_quality_mode").is_null()) {
                int theValue = jsonObjectData.at("video_quality_mode");
                channelData.videoQualityMode = theValue;
            }

            *pDataStructure = channelData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::RoleTagsData * pDataStructure) {
            ClientDataTypes::RoleTagsData roleTagsData = *pDataStructure;

            if (jsonObjectData.contains("bot_id") && !jsonObjectData.at("bot_id").is_null()) {
                std::string theValue = jsonObjectData.at("bot_id");
                roleTagsData.botId = theValue;
            }

            if (jsonObjectData.contains("integration_id") && !jsonObjectData.at("integration_id").is_null()) {
                std::string theValue = jsonObjectData.at("integration_id");
                roleTagsData.integrationId = theValue;
            }

            *pDataStructure = roleTagsData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::RoleData* pDataStructure) {
            ClientDataTypes::RoleData roleData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                roleData.id = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                roleData.name = theValue;
            }

            if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
                int theValue = jsonObjectData.at("color");
                roleData.color = theValue;
            }

            if (jsonObjectData.contains("hoist") && !jsonObjectData.at("hoist").is_null()) {
                bool theValue = jsonObjectData.at("hoist");
                roleData.hoist = theValue;
            }

            if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
                int theValue = jsonObjectData.at("position");
                roleData.position = theValue;
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                std::string theValue = jsonObjectData.at("permissions");
                roleData.permissions = theValue;
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
                bool theValue = jsonObjectData.at("managed");
                roleData.managed = theValue;
            }

            if (jsonObjectData.contains("mentionable") && !jsonObjectData.at("mentionable").is_null()) {
                bool theValue = jsonObjectData.at("mentionable");
                roleData.mentionable = theValue;
            }

            if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
                ClientDataTypes::RoleTagsData roleTagsData = roleData.tags;
                parseObject(jsonObjectData.at("tags"), &roleTagsData);
                roleData.tags = roleTagsData;
            }

            *pDataStructure = roleData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::GuildMemberData* pDataStructure) {
            ClientDataTypes::GuildMemberData guildMemberData = *pDataStructure;

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                ClientDataTypes::UserData userData = guildMemberData.user;
                parseObject(jsonObjectData.at("user"), &userData);
                guildMemberData.user = userData;
            }

            if (jsonObjectData.contains("nick") && !jsonObjectData.at("nick").is_null()) {
                std::string theValue = jsonObjectData.at("nick");
                guildMemberData.nick = theValue;
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                json theValue = jsonObjectData.at("roles");
                for (unsigned int x = 0; x < theValue.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildMemberData.roles.size(); y += 1) {
                        if (guildMemberData.roles.at(y) == theValue.at(x)) {
                            isItFound = true;
                            break;
                        }
                    }
                    if (isItFound == false) {
                        guildMemberData.roles.push_back(theValue.at(x));
                    }                    
                }
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
                std::string theValue = jsonObjectData.at("joined_at");
                guildMemberData.joined_at = theValue;
            }

            if (jsonObjectData.contains("premium_since") && !jsonObjectData.at("premium_since").is_null()) {
                std::string theValue = jsonObjectData.at("premium_since");
                guildMemberData.premium_since = theValue;
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
                bool theValue = jsonObjectData.at("deaf");
                guildMemberData.deaf = theValue;
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
                bool theValue = jsonObjectData.at("mute");
                guildMemberData.mute = theValue;
            }

            if (jsonObjectData.contains("pending") && !jsonObjectData.at("pending").is_null()) {
                bool theValue = jsonObjectData.at("pending");
                guildMemberData.pending = theValue;
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                std::string theValue = jsonObjectData.at("permissions");
                guildMemberData.permissions = theValue;
            }

            *pDataStructure = guildMemberData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmojiData* pDataStructure) {
            ClientDataTypes::EmojiData emojiData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                emojiData.id = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                emojiData.id = theValue;
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                json roles = jsonObjectData.at("roles");
                for (unsigned int x = 0; x < roles.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < emojiData.roles.size(); y += 1) {
                        if (emojiData.roles.at(y).id == roles.at(x).at("id")) {
                            isItFound = true;
                            emojiData.roles.erase(emojiData.roles.begin() + y);
                            ClientDataTypes::RoleData roleData = emojiData.roles.at(y);
                            parseObject(roles.at(x), &roleData);
                            emojiData.roles.push_back(roleData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::RoleData roleData;
                        parseObject(roles.at(x), &roleData);
                        emojiData.roles.push_back(roleData);
                    }
                }
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                ClientDataTypes::UserData theUser = emojiData.user;
                parseObject(jsonObjectData.at("user"), &theUser);
                emojiData.user = theUser;
            }

            if (jsonObjectData.contains("require_colons") && !jsonObjectData.at("require_colons").is_null()) {
                bool theValue = jsonObjectData.at("require_colons");
                emojiData.requireColons = theValue;
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
                bool theValue = jsonObjectData.at("mananged");
                emojiData.managed = theValue;
            }

            if (jsonObjectData.contains("animated") && !jsonObjectData.at("animated").is_null()) {
                bool theValue = jsonObjectData.at("animated");
                emojiData.animated = theValue;
            }

            if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
                bool theValue = jsonObjectData.at("available");
                emojiData.available = theValue;
            }

            *pDataStructure = emojiData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::VoiceStateData* pDataStructure) {
            ClientDataTypes::VoiceStateData voiceStateData = *pDataStructure;

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                voiceStateData.guildId = theValue;
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("channel_id");
                voiceStateData.channelId = theValue;
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                std::string theValue = jsonObjectData.at("user_id");
                voiceStateData.userId = theValue;
            }

            if (jsonObjectData.contains("guild_member") && !jsonObjectData.at("guild_member").is_null()) {
                ClientDataTypes::GuildMemberData guildMemberData = voiceStateData.guildMember;
                parseObject(jsonObjectData.at("guild_member"), &guildMemberData);
                voiceStateData.guildMember = guildMemberData;
            }

            if (jsonObjectData.contains("session_id") && !jsonObjectData.at("session_id").is_null()) {
                std::string theValue = jsonObjectData.at("session_id");
                voiceStateData.sessionId = theValue;
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
                bool theValue = jsonObjectData.at("deaf");
                voiceStateData.deaf = theValue;
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
                bool theValue = jsonObjectData.at("mute");
                voiceStateData.mute = theValue;
            }

            if (jsonObjectData.contains("self_deaf") && !jsonObjectData.at("self_deaf").is_null()) {
                bool theValue = jsonObjectData.at("self_deaf");
                voiceStateData.selfDeaf = theValue;
            }

            if (jsonObjectData.contains("self_mute") && !jsonObjectData.at("self_mute").is_null()) {
                bool theValue = jsonObjectData.at("self_mute");
                voiceStateData.selfMute = theValue;
            }

            if (jsonObjectData.contains("self_stream") && !jsonObjectData.at("self_stream").is_null()) {
                bool theValue = jsonObjectData.at("self_stream");
                voiceStateData.selfStream = theValue;
            }

            if (jsonObjectData.contains("self_video") && !jsonObjectData.at("self_video").is_null()) {
                bool theValue = jsonObjectData.at("self_video");
                voiceStateData.selfVideo = theValue;
            }

            if (jsonObjectData.contains("suppress") && !jsonObjectData.at("suppress").is_null()) {
                bool theValue = jsonObjectData.at("suppress");
                voiceStateData.suppress = theValue;
            }

            if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData.at("request_to_speak_timestamp").is_null()) {
                std::string theValue = jsonObjectData.at("request_to_speak_timestamp");
                voiceStateData.requestToSpeakTimestamp = theValue;
            }

            *pDataStructure = voiceStateData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::PartyData* pDataStructure) {
            ClientDataTypes::PartyData partyData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                partyData.id = theValue;
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

        void parseObject(json jsonObjectData, ClientDataTypes::AssetsData* pDataStructure) {
            ClientDataTypes::AssetsData assetData = *pDataStructure;

            if (jsonObjectData.contains("large_image") && !jsonObjectData.at("large_image").is_null()) {
                std::string theValue = jsonObjectData.at("large_image");
                assetData.largeImage = theValue;
            }

            if (jsonObjectData.contains("large_text") && !jsonObjectData.at("large_text").is_null()) {
                std::string theValue = jsonObjectData.at("large_text");
                assetData.largeText = theValue;
            }

            if (jsonObjectData.contains("small_image") && !jsonObjectData.at("small_image").is_null()) {
                std::string theValue = jsonObjectData.at("small_image");
                assetData.smallImage = theValue;
            }

            if (jsonObjectData.contains("small_text") && !jsonObjectData.at("small_text").is_null()) {
                std::string theValue = jsonObjectData.at("small_text");
                assetData.smallText = theValue;
            }
            
            *pDataStructure = assetData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::SecretsData* pDataStructure) {
            ClientDataTypes::SecretsData secretData = *pDataStructure;

            if (jsonObjectData.contains("join") && !jsonObjectData.at("join").is_null()) {
                std::string theValue = jsonObjectData.at("join");
                secretData.join = theValue;
            }

            if (jsonObjectData.contains("spectate") && !jsonObjectData.at("spectate").is_null()) {
                std::string theValue = jsonObjectData.at("spectate");
                secretData.spectate = theValue;
            }

            if (jsonObjectData.contains("match") && !jsonObjectData.at("match").is_null()) {
                std::string theValue = jsonObjectData.at("match");
                secretData.match = theValue;
            }

            *pDataStructure = secretData;

        }

        void parseObject(json jsonObjectData, ClientDataTypes::ButtonData* pDataStructure) {
            ClientDataTypes::ButtonData buttonData = *pDataStructure;

            if (jsonObjectData.contains("label") && !jsonObjectData.at("label").is_null()) {
                std::string theValue = jsonObjectData.at("label");
                buttonData.label = theValue;
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                buttonData.url = theValue;
            }
            
            *pDataStructure = buttonData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::ClientStatusData* pDataStructure) {
            ClientDataTypes::ClientStatusData clientStatusData = *pDataStructure;

            if (jsonObjectData.contains("desktop") && !jsonObjectData.at("desktop").is_null()) {
                std::string theValue = jsonObjectData.at("desktop");
                clientStatusData.desktop = theValue;
            }

            if (jsonObjectData.contains("mobile") && !jsonObjectData.at("mobile").is_null()) {
                std::string theValue = jsonObjectData.at("mobile");
                clientStatusData.mobile = theValue;
            }

            if (jsonObjectData.contains("web") && !jsonObjectData.at("web").is_null()) {
                std::string theValue = jsonObjectData.at("web");
                clientStatusData.web = theValue;
            }

            *pDataStructure = clientStatusData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::TimestampData* pDataStructure) {
            ClientDataTypes::TimestampData timestampData = *pDataStructure;

            if (jsonObjectData.contains("start") && !jsonObjectData.at("start").is_null()) {
                int theValue = jsonObjectData.at("start");
                timestampData.start = theValue;
            }

            if (jsonObjectData.contains("end") && !jsonObjectData.at("end").is_null()) {
                int theValue = jsonObjectData.at("end");
                timestampData.end = theValue;
            }
            
            *pDataStructure = timestampData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::ActivityData* pDataStructure) {
            ClientDataTypes::ActivityData activityData = *pDataStructure;

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                activityData.name = theValue;
            }
            
            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                activityData.type = theValue;
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                activityData.url = theValue;
            }

            if (jsonObjectData.contains("created_at") && !jsonObjectData.at("created_at").is_null()) {
                int theValue = jsonObjectData.at("created_at");
                activityData.createdAt = theValue;
            }

            if (jsonObjectData.contains("timestamps") && !jsonObjectData.at("timestamps").is_null()) {
                ClientDataTypes::TimestampData theValue = activityData.timestamps;
                parseObject(jsonObjectData.at("timestamps"), &theValue);
                activityData.timestamps = theValue;
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                std::string theValue = jsonObjectData.at("application_id");
                activityData.applicationId = theValue;
            }

            if (jsonObjectData.contains("details") && !jsonObjectData.at("details").is_null()) {
                std::string theValue = jsonObjectData.at("details");
                activityData.details = theValue;
            }

            if (jsonObjectData.contains("state") && !jsonObjectData.at("state").is_null()) {
                std::string theValue = jsonObjectData.at("state");
                activityData.state = theValue;
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                ClientDataTypes::EmojiData theValue;
                parseObject(jsonObjectData.at("emoji"), &theValue);
                activityData.emoji = theValue;
            }

            if (jsonObjectData.contains("party") && !jsonObjectData.at("party").is_null()) {
                ClientDataTypes::PartyData theValue;
                parseObject(jsonObjectData.at("party"), &theValue);
                activityData.party = theValue;
            }

            if (jsonObjectData.contains("assets") && !jsonObjectData.at("assets").is_null()) {
                ClientDataTypes::AssetsData theValue;
                parseObject(jsonObjectData.at("assets"), &theValue);
                activityData.assets = theValue;
            }

            if (jsonObjectData.contains("secrets") && !jsonObjectData.at("secrets").is_null()) {
                ClientDataTypes::SecretsData theValue;
                parseObject(jsonObjectData.at("secrets"), &theValue);
                activityData.secrets = theValue;
            }

            if (jsonObjectData.contains("instance") && !jsonObjectData.at("instance").is_null()) {
                bool theValue = jsonObjectData.at("instance");
                activityData.instance = theValue;                
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                int theValue = jsonObjectData.at("flags");
                activityData.flags = theValue;
            }

            if (jsonObjectData.contains("buttons") && !jsonObjectData.at("buttons").is_null()) {
                json buttonArray = jsonObjectData.at("buttons");
                for (unsigned int x = 0; x < buttonArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < activityData.buttons.size(); y += 1) {
                        if (activityData.buttons.at(y).url == buttonArray.at(x)) {
                            isItFound = true;
                            activityData.buttons.erase(activityData.buttons.begin() + y);
                            ClientDataTypes::ButtonData buttonData = activityData.buttons.at(y);
                            parseObject(buttonArray.at(x), &buttonData);
                            activityData.buttons.push_back(buttonData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::ButtonData buttonData;
                        parseObject(buttonArray.at(x), &buttonData);
                        activityData.buttons.push_back(buttonData);
                    }
                }
            }
            
            *pDataStructure = activityData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::PresenceUpdateData* pDataStructure) {
            ClientDataTypes::PresenceUpdateData presenceUpdateData = *pDataStructure;

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                ClientDataTypes::UserData theValue = presenceUpdateData.user;
                parseObject(jsonObjectData.at("user"), &theValue);
                presenceUpdateData.user = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                presenceUpdateData.guildId = theValue;
            }

            if (jsonObjectData.contains("status") && !jsonObjectData.at("status").is_null()) {
                std::string theValue = jsonObjectData.at("status");
                presenceUpdateData.status = theValue;
            }

            if (jsonObjectData.contains("activities") && !jsonObjectData.at("activities").is_null()) {
                json activityArray = jsonObjectData.at("activities");
                for (unsigned int x = 0; x < activityArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < presenceUpdateData.activities.size(); y += 1) {
                        if (presenceUpdateData.activities.at(y).applicationId == activityArray.at(x)) {
                            isItFound = true;
                            presenceUpdateData.activities.erase(presenceUpdateData.activities.begin() + y);
                            ClientDataTypes::ActivityData pUpdateData = presenceUpdateData.activities.at(y);
                            parseObject(activityArray.at(x), &pUpdateData);
                            presenceUpdateData.activities.push_back(pUpdateData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::ActivityData pUpdateData;
                        parseObject(activityArray.at(x), &pUpdateData);
                        presenceUpdateData.activities.push_back(pUpdateData);
                    }
                }
            }

            if (jsonObjectData.contains("client_status") && !jsonObjectData.at("client_status").is_null()) {
                ClientDataTypes::ClientStatusData theValue = presenceUpdateData.clientStatus;
                parseObject(jsonObjectData.at("client_status"), &theValue);
                presenceUpdateData.clientStatus = theValue;
            }

            *pDataStructure = presenceUpdateData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::WelcomeScreenChannelData* pDataStructure) {
            ClientDataTypes::WelcomeScreenChannelData welcomeSceenChannelData = *pDataStructure;
            
            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("channel_id");
                welcomeSceenChannelData.channelId = theValue;
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
                welcomeSceenChannelData.description = theValue;
            }

            if (jsonObjectData.contains("emoji_id") && !jsonObjectData.at("emoji_id").is_null()) {
                std::string theValue = jsonObjectData.at("emoji_id");
                welcomeSceenChannelData.emojiId = theValue;
            }

            if (jsonObjectData.contains("emoji_name") && !jsonObjectData.at("emoji_name").is_null()) {
                std::string theValue = jsonObjectData.at("emoji_name");
                welcomeSceenChannelData.emojiName = theValue;
            }

            *pDataStructure = welcomeSceenChannelData;
        }
        
        void parseObject(json jsonObjectData, ClientDataTypes::WelcomeScreenData* pDataStructure) {
            ClientDataTypes::WelcomeScreenData welcomeScreenData = *pDataStructure;

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
                welcomeScreenData.description = theValue;
            }

            if (jsonObjectData.contains("welcome_channels") && !jsonObjectData.at("welcome_channels").is_null()) {
                json welcomeChannelsArray = jsonObjectData.at("welcome_channels");
                for (unsigned int x = 0; x < welcomeChannelsArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < welcomeScreenData.welcomeChannels.size(); y += 1) {
                        if (welcomeScreenData.welcomeChannels.at(y).channelId == welcomeChannelsArray.at(x).at("channel_id")) {
                            isItFound = true;
                            welcomeScreenData.welcomeChannels.erase(welcomeScreenData.welcomeChannels.begin() + y);
                            ClientDataTypes::WelcomeScreenChannelData welcomeChannelData = welcomeScreenData.welcomeChannels.at(y);
                            parseObject(welcomeChannelsArray.at(x), &welcomeChannelData);
                            welcomeScreenData.welcomeChannels.push_back(welcomeChannelData);
                        break;
                    }
                }
                if (isItFound == false) {
                    ClientDataTypes::WelcomeScreenChannelData welcomeChannelData;
                    parseObject(welcomeChannelsArray.at(x), &welcomeChannelData);
                    welcomeScreenData.welcomeChannels.push_back(welcomeChannelData);
                }
                }
            }
            
            *pDataStructure = welcomeScreenData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::GuildData* pDataStructure) {
            ClientDataTypes::GuildData guildData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                guildData.id = theValue;
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("afk_channel_id");
                guildData.afkChannelID = theValue;
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                std::string theValue = jsonObjectData.at("icon");
                guildData.icon = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                guildData.name = theValue;
            }

            if (jsonObjectData.contains("icon_hash") && !jsonObjectData.at("icon_hash").is_null()) {
                std::string theValue = jsonObjectData.at("icon_hash");
                guildData.iconHash = theValue;
            }

            if (jsonObjectData.contains("splash") && !jsonObjectData.at("splash").is_null()) {
                std::string theValue = jsonObjectData.at("splash");
                guildData.splash = theValue;
            }

            if (jsonObjectData.contains("discovery_splash") && !jsonObjectData.at("discovery_splash").is_null()) {
                std::string theValue = jsonObjectData.at("discovery_splash");
                guildData.discoverySplash = theValue;
            }

            if (jsonObjectData.contains<std::string>("owner") && !jsonObjectData.at("owner").is_null()) {
                bool theValue = jsonObjectData.at("owner");
                guildData.owner = theValue;
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
                std::string theValue = jsonObjectData.at("owner_id");
                guildData.ownerID = theValue;
            }

            if (jsonObjectData.contains("preferred_locale") && !jsonObjectData.at("preferred_locale").is_null()) {
                std::string theValue = jsonObjectData.at("preferred_locale");
                guildData.preferredLocale = theValue;
            }

            if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData.at("public_updates_channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("public_updates_channel_id");
                guildData.publicUpdatesChannelID = theValue;
            }

            if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData.at("vanity_url_code").is_null()) {
                std::string theValue = jsonObjectData.at("vanity_url_code");
                guildData.vanityURLCode = theValue;
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
                guildData.description = theValue;
            }

            if (jsonObjectData.contains("banner") && !jsonObjectData.at("banner").is_null()) {
                std::string theValue = jsonObjectData.at("banner");
                guildData.banner = theValue;
            }
            
            if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData.at("rule_Channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("rule_Channel_id");
                guildData.ruleChannelID = theValue;
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                std::string theValue = jsonObjectData.at("application_id");
                guildData.applicationID = theValue;
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
                std::string theValue = jsonObjectData.at("joined_at");
                guildData.joinedAt = theValue;
            }

            if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData.at("widget_channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("widget_channel_id");
                guildData.widgetChannelID = theValue;
            }

            if (jsonObjectData.contains("system_channel_id") && !jsonObjectData.at("system_channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("system_channel_id");
                guildData.systemChannelID = theValue;
            }
            
            if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
                std::string theValue = jsonObjectData.at("region");
                guildData.region = theValue;
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("afk_channel_id");
                guildData.afkChannelID = theValue;
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
                std::string theValue = jsonObjectData.at("owner_id");
                guildData.ownerID = theValue;
            }

            if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
                std::string theValue = jsonObjectData.at("region");
                guildData.region = theValue;
            }
            
            if (jsonObjectData.contains("features") && !jsonObjectData.at("features").is_null()) {
                json theValue = jsonObjectData.at("features");
                for (unsigned int x = 0; x < theValue.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.features.size(); y += 1) {
                        if (guildData.features.at(y) == theValue.at(x)) {
                            isItFound = true;
                            break;
                        }
                    }
                    if (isItFound == false) {
                        guildData.features.push_back(theValue.at(x));
                    }
                }
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                json theValue = jsonObjectData.at("permissions");
                for (unsigned int x = 0; x < theValue.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.permissions.size(); y += 1) {
                        if (guildData.permissions.at(y) == theValue.at(x)) {
                            isItFound = true;
                            break;
                        }
                    }
                    if (isItFound == false) {
                        guildData.permissions.push_back(theValue.at(x));
                    }
                }
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                json roleDataArray = jsonObjectData.at("roles");
                for (unsigned int x = 0; x < roleDataArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.roles.size(); y += 1) {
                        if (guildData.roles.at(y).id == roleDataArray.at(x).at("id")) {
                            isItFound = true;
                            guildData.roles.erase(guildData.roles.begin() + y);
                            ClientDataTypes::RoleData roleData = guildData.roles.at(y);
                            parseObject(roleDataArray.at(x), &roleData);
                            guildData.roles.push_back(roleData);                            
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::RoleData roleData;
                        parseObject(roleDataArray.at(x), &roleData);
                        guildData.roles.push_back(roleData);
                    }
                }
            }
            
            if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
                bool theValue = jsonObjectData.at("owner");
                guildData.owner = theValue;
            }

            if (jsonObjectData.contains("afk_timeout") && !jsonObjectData.at("afk_timeout").is_null()) {
                int theValue = jsonObjectData.at("afk_timeout");
                guildData.afkTimeOut = theValue;
            }

            if (jsonObjectData.contains("widget_enabled") && !jsonObjectData.at("widget_enabled").is_null()) {
                bool theValue = jsonObjectData.at("widget_enabled");
                guildData.widgetEnabled = theValue;
            }

            if (jsonObjectData.contains("verification_level") && !jsonObjectData.at("verification_level").is_null()) {
                ClientDataTypes::VerificationLevel theValue = jsonObjectData.at("verification_level");
                guildData.verificationLevel = theValue;
            }

            if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData.at("default_message_notification_level").is_null()) {
                ClientDataTypes::DefaultMessageNotificationLevel theValue = jsonObjectData.at("default_message_notification_level");
                guildData.defaultMessageNotifications = theValue;
            }

            if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData.at("explicit_content_filter_level").is_null()) {
                ClientDataTypes::ExplicitContentFilterLevel theValue = jsonObjectData.at("explicit_content_filter_level");
                guildData.explicitContentFilter = theValue;
            }
            
            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                json emojiDataArray = jsonObjectData.at("emoji");
                for (unsigned int x = 0; x < emojiDataArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.emoji.size(); y += 1) {
                        if (guildData.emoji.at(y).id == emojiDataArray.at(x).at("id")) {
                            isItFound = true;
                            guildData.emoji.erase(guildData.emoji.begin() + y);
                            ClientDataTypes::EmojiData emojiData = guildData.emoji.at(y);
                            parseObject(emojiDataArray.at(x), &emojiData);
                            guildData.emoji.push_back(emojiData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::EmojiData emojiData;
                        parseObject(emojiDataArray.at(x), &emojiData);
                        guildData.emoji.push_back(emojiData);
                    }
                }
            }
            
            if (jsonObjectData.contains("mfa_level") && !jsonObjectData.at("mfa_level").is_null()) {
                ClientDataTypes::MFALevel theValue = jsonObjectData.at("mfa_level");
                guildData.mfaLevel = theValue;
            }

            if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData.at("system_channel_flags").is_null()) {
                int theValue = jsonObjectData.at("system_channel_flags");
                guildData.systemChannelFlags = theValue;
            }

            if (jsonObjectData.contains("large") && !jsonObjectData.at("large").is_null()) {
                bool theValue = jsonObjectData.at("large");
                guildData.large = theValue;
            }

            if (jsonObjectData.contains("unavailable") && !jsonObjectData.at("unavailable").is_null()) {
                bool theValue = jsonObjectData.at("unavailable");
                guildData.unavailable = theValue;
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
                int theValue = jsonObjectData.at("member_count");
                guildData.memberCount = theValue;
            }
           
            if (jsonObjectData.contains("voice_states") && !jsonObjectData.at("voice_states").is_null()) {
                json voiceStates = jsonObjectData.at("voice_states");
                for (unsigned int x = 0; x < voiceStates.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.voiceStates.size(); y += 1) {
                        if (guildData.voiceStates.at(y).userId == voiceStates.at(x).at("user_id")) {
                            isItFound = true;
                            guildData.voiceStates.erase(guildData.voiceStates.begin() + y);
                            ClientDataTypes::VoiceStateData voiceStateData = guildData.voiceStates.at(y);
                            parseObject(voiceStates.at(x), &voiceStateData);
                            guildData.voiceStates.push_back(voiceStateData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::VoiceStateData voiceStateData;
                        parseObject(voiceStates.at(x), &voiceStateData);
                        guildData.voiceStates.push_back(voiceStateData);
                    }
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                json memberArray = jsonObjectData.at("members");
                for (unsigned int x = 0; x < memberArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.members.size(); y += 1) {
                        if (guildData.members.at(y).user.id == memberArray.at(x).at("user").at("id")) {
                            isItFound = true;
                            guildData.members.erase(guildData.members.begin() + y);
                            ClientDataTypes::GuildMemberData guildMemberData = guildData.members.at(y);
                            parseObject(memberArray.at(x), &guildMemberData);
                            guildData.members.push_back(guildMemberData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::GuildMemberData guildMemberData;
                        parseObject(memberArray.at(x), &guildMemberData);
                        guildData.members.push_back(guildMemberData);
                    }
                }
            }

            if (jsonObjectData.contains("channels") && !jsonObjectData.at("channels").is_null()) {
                json channelArray = jsonObjectData.at("channels");
                for (unsigned int x = 0; x < channelArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.channels.size(); y += 1) {
                        if (guildData.channels.at(y).id == channelArray.at(x).at("id")) {
                            isItFound = true;
                            guildData.channels.erase(guildData.channels.begin() + y);
                            ClientDataTypes::ChannelData channelData = guildData.channels.at(y);
                            parseObject(channelArray.at(x), &channelData);
                            guildData.channels.push_back(channelData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::ChannelData channelData;
                        parseObject(channelArray.at(x), &channelData);
                        guildData.channels.push_back(channelData);
                    }
                }
            }
            
            if (jsonObjectData.contains("presences") && !jsonObjectData.at("presences").is_null()) {
                json presencesArray = jsonObjectData.at("presences");
                for (unsigned int x = 0; x < presencesArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < guildData.presences.size(); y += 1) {
                        if (guildData.presences.at(y).user.id == presencesArray.at(x).at("user").at("id")) {
                            isItFound = true;
                            guildData.presences.erase(guildData.presences.begin() + y);
                            ClientDataTypes::PresenceUpdateData presenceUpdateData = guildData.presences.at(y);
                            parseObject(presencesArray.at(x), &presenceUpdateData);
                            guildData.presences.push_back(presenceUpdateData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::PresenceUpdateData presenceUpdateData;
                        parseObject(presencesArray.at(x), &presenceUpdateData);
                        guildData.presences.push_back(presenceUpdateData);
                    }
                }
            }
           
            if (jsonObjectData.contains("max_presences") && !jsonObjectData.at("max_presences").is_null()) {
                int theValue = jsonObjectData.at("max_presences");
                guildData.maxPresences = theValue;
            }
            
            if (jsonObjectData.contains("max_members") && !jsonObjectData.at("max_members").is_null()) {
                int theValue = jsonObjectData.at("max_members");
                guildData.maxMembers = theValue;
            }

            if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData.at("premium_subscription_count").is_null()) {
                int theValue = jsonObjectData.at("premium_subscription_count");
                guildData.premiumSubscriptionCount = theValue;
            }

            if (jsonObjectData.contains("premium_tier") && !jsonObjectData.at("premium_tier").is_null()) {
                ClientDataTypes::PremiumTier theValue = jsonObjectData.at("premium_tier");
                guildData.premiumTier = theValue;
            }
            
            if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData.at("max_video_channel_users").is_null()) {
                int theValue = jsonObjectData.at("max_video_channel_users");
                guildData.maxVideoChannelUsers = theValue;
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
                int theValue = jsonObjectData.at("approximate_member_count");
                guildData.approximateMemberCount = theValue;
            }

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
                int theValue = jsonObjectData.at("approximate_presence_count");
                guildData.approximatePresenceCount = theValue;
            }

            if (jsonObjectData.contains("welcome_screen") && !jsonObjectData.at("welcome_screen").is_null()) {
                ClientDataTypes::WelcomeScreenData theValue = guildData.welcomeScreen;
                parseObject(jsonObjectData.at("welcome_screen"), &theValue);
                guildData.welcomeScreen = theValue;
            }
        
            *pDataStructure = guildData;
        };

        void parseObject(json jsonObjectData, ClientDataTypes::ChannelMentionData* pDataStructure) {
            ClientDataTypes::ChannelMentionData channelMentionData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                channelMentionData.id = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                channelMentionData.guildId = theValue;
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                channelMentionData.type = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                channelMentionData.name = theValue;
            }
            
            *pDataStructure = channelMentionData;
        };

        void parseObject(json jsonObjectData, ClientDataTypes::AttachmentData* pDataStructure) {
            ClientDataTypes::AttachmentData attachmentData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                attachmentData.id = theValue;
            }

            if (jsonObjectData.contains("filename") && !jsonObjectData.at("filename").is_null()) {
                std::string theValue = jsonObjectData.at("filename");
                attachmentData.filename = theValue;
            }

            if (jsonObjectData.contains("content_type") && !jsonObjectData.at("content_type").is_null()) {
                std::string theValue = jsonObjectData.at("content_type");
                attachmentData.contentType = theValue;
            }

            if (jsonObjectData.contains("size") && !jsonObjectData.at("size").is_null()) {
                int theValue = jsonObjectData.at("size");
                attachmentData.size = theValue;
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                attachmentData.url = theValue;
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_url");
                attachmentData.proxyUrl = theValue;
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                int theValue = jsonObjectData.at("width");
                attachmentData.width = theValue;
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                int theValue = jsonObjectData.at("height");
                attachmentData.heigh = theValue;
            }

            *pDataStructure = attachmentData;
        };

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedFooterData* pDataStructure) {
            ClientDataTypes::EmbedFooterData footerData = *pDataStructure;

            if (jsonObjectData.contains("text") && !jsonObjectData.at("text").is_null()) {
                std::string theValue = jsonObjectData.at("text");
                footerData.text = theValue;
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
                std::string theValue = jsonObjectData.at("icon_url");
                footerData.iconUrl = theValue;
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_icon_url");
                footerData.proxyIconUrl = theValue;
            }

            *pDataStructure = footerData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedImageData* pDataStructure) {
            ClientDataTypes::EmbedImageData imageData = *pDataStructure;

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                imageData.url = theValue;
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_url");
                imageData.proxyUrl = theValue;
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                int theValue = jsonObjectData.at("width");
                imageData.width = theValue;
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                int theValue = jsonObjectData.at("height");
                imageData.height = theValue;
            }

            *pDataStructure = imageData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedThumbnailData* pDataStructure) {
            ClientDataTypes::EmbedThumbnailData thumbnailData = *pDataStructure;

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                thumbnailData.url = theValue;
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_url");
                thumbnailData.proxyUrl = theValue;
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                int theValue = jsonObjectData.at("width");
                thumbnailData.width = theValue;
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                int theValue = jsonObjectData.at("height");
                thumbnailData.height = theValue;
            }

            *pDataStructure = thumbnailData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedVideoData* pDataStructure) {
            ClientDataTypes::EmbedVideoData videoData = *pDataStructure;

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                videoData.url = theValue;
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_url");
                videoData.proxyUrl = theValue;
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                int theValue = jsonObjectData.at("width");
                videoData.width = theValue;
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                int theValue = jsonObjectData.at("height");
                videoData.height = theValue;
            }

            *pDataStructure = videoData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedAuthorData* pDataStructure) {
            ClientDataTypes::EmbedAuthorData authorData = *pDataStructure;

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                authorData.url = theValue;
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
                std::string theValue = jsonObjectData.at("proxy_icon_url");
                authorData.proxyIconUrl = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                authorData.name = theValue;
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
                std::string theValue = jsonObjectData.at("icon_url");
                authorData.iconUrl = theValue;
            }

            *pDataStructure = authorData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedProviderData* pDataStructure) {
            ClientDataTypes::EmbedProviderData providerData = *pDataStructure;

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                providerData.url = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                providerData.name = theValue;
            }

            *pDataStructure = providerData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedFieldData* pDataStructure) {
            ClientDataTypes::EmbedFieldData fieldData = *pDataStructure;

            if (jsonObjectData.contains("inline") && !jsonObjectData.at("inline").is_null()) {
                bool theValue = jsonObjectData.at("inline");
                fieldData.Inline = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                fieldData.name = theValue;
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
                std::string theValue = jsonObjectData.at("value");
                fieldData.value = theValue;
            }

            *pDataStructure = fieldData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::EmbedData* pDataStructure) {
            ClientDataTypes::EmbedData embedData = *pDataStructure;

            if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
                std::string theValue = jsonObjectData.at("title");
                embedData.title = theValue;
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                std::string theValue = jsonObjectData.at("type");
                embedData.type = theValue;
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
                embedData.description = theValue;
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                std::string theValue = jsonObjectData.at("url");
                embedData.url = theValue;
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                std::string theValue = jsonObjectData.at("timestamp");
                embedData.timestamp = theValue;
            }

            if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
                int theValue = jsonObjectData.at("color");
                embedData.color = theValue;
            }

            if (jsonObjectData.contains("footer") && !jsonObjectData.at("footer").is_null()) {
                ClientDataTypes::EmbedFooterData theValue = embedData.footer;
                parseObject(jsonObjectData.at("footer"), &theValue);
                embedData.footer = theValue;
            }

            if (jsonObjectData.contains("image") && !jsonObjectData.at("image").is_null()) {
                ClientDataTypes::EmbedImageData theValue = embedData.image;
                parseObject(jsonObjectData.at("image"), &theValue);
                embedData.image = theValue;
            }

            if (jsonObjectData.contains("provider") && !jsonObjectData.at("provider").is_null()) {
                ClientDataTypes::EmbedProviderData theValue = embedData.provider;
                parseObject(jsonObjectData.at("provider"), &theValue);
                embedData.provider = theValue;
            }

            if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
                ClientDataTypes::EmbedThumbnailData theValue = embedData.thumbnail;
                parseObject(jsonObjectData.at("thumbnail"), &theValue);
                embedData.thumbnail = theValue;
            }

            if (jsonObjectData.contains("video") && !jsonObjectData.at("video").is_null()) {
                ClientDataTypes::EmbedVideoData theValue = embedData.video;
                parseObject(jsonObjectData.at("video"), &theValue);
                embedData.video = theValue;
            }

            if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
                ClientDataTypes::EmbedAuthorData theValue = embedData.author;
                parseObject(jsonObjectData.at("author"), &theValue);
                embedData.author = theValue;
            }

            if (jsonObjectData.contains("fields") && !jsonObjectData.at("fields").is_null()) {
                json fieldsArray = jsonObjectData.at("fields");
                for (unsigned int x = 0; x < fieldsArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < embedData.fields.size(); y += 1) {
                        if (embedData.fields.at(y).name == fieldsArray.at(x).at("name")) {
                            isItFound = true;
                            embedData.fields.erase(embedData.fields.begin() + y);
                            ClientDataTypes::EmbedFieldData fieldData = embedData.fields.at(y);
                            parseObject(fieldsArray.at(x), &fieldData);
                            embedData.fields.push_back(fieldData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::EmbedFieldData fieldData;
                        parseObject(fieldsArray.at(x), &fieldData);
                        embedData.fields.push_back(fieldData);
                        break;
                    }
                }
            }

            *pDataStructure = embedData;
        };

        void parseObject(json jsonObjectData, ClientDataTypes::ReactionData* pDataStructure) {
            ClientDataTypes::ReactionData reactionData = *pDataStructure;

            if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
                int theValue = jsonObjectData.at("count");
                reactionData.count = theValue;
            }

            if (jsonObjectData.contains("me") && !jsonObjectData.at("me").is_null()) {
                bool theValue = jsonObjectData.at("me");
                reactionData.count = theValue;
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                ClientDataTypes::EmojiData theValue = reactionData.emoji;
                parseObject(jsonObjectData.at("emoji"), &theValue);
                reactionData.emoji = theValue;
            }
            
            *pDataStructure = reactionData;
        }
        
        void parseObject(json jsonObjectData, ClientDataTypes::MessageActivityData* pDataStructure) {
            ClientDataTypes::MessageActivityData messageActivityData = *pDataStructure;

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                messageActivityData.type = theValue;
            }

            if (jsonObjectData.contains("party_id") && !jsonObjectData.at("party_id").is_null()) {
                std::string theValue = jsonObjectData.at("party_id");
                messageActivityData.partyId = theValue;
            }

            *pDataStructure = messageActivityData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::TeamMembersObjectData* pDataStructure) {
            ClientDataTypes::TeamMembersObjectData teamMemberObjectData = *pDataStructure;

            if (jsonObjectData.contains("membership_state") && !jsonObjectData.at("membership_state").is_null()) {
                int theValue = jsonObjectData.at("membership_state");
                teamMemberObjectData.membershipState = theValue;
            }

            if (jsonObjectData.contains("team_id") && !jsonObjectData.at("team_id").is_null()) {
                std::string theValue = jsonObjectData.at("team_id");
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
                ClientDataTypes::UserData theValue = teamMemberObjectData.user;
                parseObject(jsonObjectData.at("user"), &theValue);
                teamMemberObjectData.user = theValue;
            }

            *pDataStructure = teamMemberObjectData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::TeamObjectData* pDataStructure) {
            ClientDataTypes::TeamObjectData teamObjectData = *pDataStructure;

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                std::string theValue = jsonObjectData.at("icon");
                teamObjectData.icon = theValue;
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                teamObjectData.id = theValue;
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                json membersArray = jsonObjectData.at("id");
                for (unsigned int x = 0; x < membersArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < teamObjectData.members.size(); y += 1) {
                        if (teamObjectData.members.at(y).user.id == membersArray.at(x).at("user").at("id")) {
                            isItFound = true;
                            teamObjectData.members.erase(teamObjectData.members.begin() + y);
                            ClientDataTypes::TeamMembersObjectData teamMembersObjectData = teamObjectData.members.at(y);
                            parseObject(membersArray.at(x), &teamMembersObjectData);
                            teamObjectData.members.push_back(teamMembersObjectData);
                            break;
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::TeamMembersObjectData teamMembersObjectData;
                        parseObject(membersArray.at(x), &teamMembersObjectData);
                        teamObjectData.members.push_back(teamMembersObjectData);
                        break;
                    }
                }
            }

            if (jsonObjectData.contains("owner_user_id") && !jsonObjectData.at("owner_user_id").is_null()) {
                std::string theValue = jsonObjectData.at("owner_user_id");
                teamObjectData.ownerUserId = theValue;
            }

            *pDataStructure = teamObjectData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::ApplicationData* pDataStructure) {
            ClientDataTypes::ApplicationData applicationData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                applicationData.id = theValue;
            }
            
            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                applicationData.name = theValue;
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                std::string theValue = jsonObjectData.at("icon");
                applicationData.icon = theValue;
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
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
                std::string theValue = jsonObjectData.at("terms_of_service_url");
                applicationData.termsOfServiceUrl = theValue;
            }

            if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData.at("privacy_policy_url").is_null()) {
                std::string theValue = jsonObjectData.at("privacy_policy_url");
                applicationData.privacyPolicyUrl = theValue;
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
                ClientDataTypes::UserData theValue = applicationData.owner;
                parseObject(jsonObjectData.at("owner"), &theValue);
                applicationData.owner = theValue;
            }

            if (jsonObjectData.contains("summary") && !jsonObjectData.at("summary").is_null()) {
                std::string theValue = jsonObjectData.at("summary");
                applicationData.summary = theValue;
            }

            if (jsonObjectData.contains("verify_key") && !jsonObjectData.at("verify_key").is_null()) {
                std::string theValue = jsonObjectData.at("verify_key");
                applicationData.verifyKey = theValue;
            }

            if (jsonObjectData.contains("team") && !jsonObjectData.at("team").is_null()) {
                ClientDataTypes::TeamObjectData theValue = applicationData.team;
                parseObject(jsonObjectData.at("team"), &theValue);
                applicationData.team = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                applicationData.guildId = theValue;
            }

            if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData.at("primary_sku_id").is_null()) {
                std::string theValue = jsonObjectData.at("primary_sku_id");
                applicationData.primarySkuId = theValue;
            }

            if (jsonObjectData.contains("slug") && !jsonObjectData.at("slug").is_null()) {
                std::string theValue = jsonObjectData.at("slug");
                applicationData.slug = theValue;
            }

            if (jsonObjectData.contains("cover_image") && !jsonObjectData.at("cover_image").is_null()) {
                std::string theValue = jsonObjectData.at("cover_image");
                applicationData.coverImage = theValue;
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                std::string theValue = jsonObjectData.at("flags");
                applicationData.coverImage = theValue;
            }

            *pDataStructure = applicationData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::MessageReferenceData* pDataStructure) {
            ClientDataTypes::MessageReferenceData messageReferenceData = *pDataStructure;

            if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
                std::string theValue = jsonObjectData.at("message_id");
                messageReferenceData.messageId = theValue;
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("channel_id");
                messageReferenceData.channelId = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                messageReferenceData.guildId = theValue;
            }

            if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData.at("fail_if_not_exists").is_null()) {
                bool theValue = jsonObjectData.at("fail_if_not_exists");
                messageReferenceData.failIfNotExists = theValue;
            }

            *pDataStructure = messageReferenceData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::MessageStickerData* pDataStructure) {
            ClientDataTypes::MessageStickerData messageStickerData = *pDataStructure;

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                messageStickerData.id = theValue;
            }

            if (jsonObjectData.contains("pack_id") && !jsonObjectData.at("pack_id").is_null()) {
                std::string theValue = jsonObjectData.at("pack_id");
                messageStickerData.packId = theValue;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                std::string theValue = jsonObjectData.at("name");
                messageStickerData.name = theValue;
            }
           
            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                std::string theValue = jsonObjectData.at("description");
                messageStickerData.description = theValue;
            }

            if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
                std::string theValue = jsonObjectData.at("tags");
                messageStickerData.tags = theValue;
            }

            if (jsonObjectData.contains("asset") && !jsonObjectData.at("asset").is_null()) {
                std::string theValue = jsonObjectData.at("asset");
                messageStickerData.asset = theValue;
            }

            if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
                int theValue = jsonObjectData.at("format_type");
                messageStickerData.formatType = theValue;
            }

            *pDataStructure = messageStickerData;
        };
        
        void parseObject(json jsonObjectData, ClientDataTypes::AllowedMentionsData* pDataStructure) {
            ClientDataTypes::AllowedMentionsData allowedMentionsData = *pDataStructure;

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


            std::vector<std::string> parse;
            std::vector<std::string> roles;
            std::vector<std::string> users;
            bool repliedUser;

            *pDataStructure = allowedMentionsData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::InteractionApplicationCommandCallbackData* pDataStructure) {
            ClientDataTypes::InteractionApplicationCommandCallbackData interactionAppCommandCallbackData = *pDataStructure;

            if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
                bool theValue = jsonObjectData.at("tts");
                interactionAppCommandCallbackData.tts = theValue;
            }

            if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
                std::string theValue = jsonObjectData.at("content");
                interactionAppCommandCallbackData.content = theValue;
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
                json embedsArray = jsonObjectData.at("embeds");
                for (unsigned int x = 0; x < embedsArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < interactionAppCommandCallbackData.embeds.size(); y += 1) {
                        if (interactionAppCommandCallbackData.embeds.at(y).author.name == embedsArray.at(x).at("author").at("name")) {
                            isItFound = true;
                            ClientDataTypes::EmbedData embedData = interactionAppCommandCallbackData.embeds.at(y);
                            parseObject(embedsArray.at(x), &embedData);
                            interactionAppCommandCallbackData.embeds.erase(interactionAppCommandCallbackData.embeds.begin() + y);
                            interactionAppCommandCallbackData.embeds.push_back(embedData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::EmbedData embedData;
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
                ClientDataTypes::AllowedMentionsData theValue = interactionAppCommandCallbackData.allowedMentions;
                parseObject(jsonObjectData.at("allowed_mentions"), &theValue);
                interactionAppCommandCallbackData.allowedMentions = theValue;
            }

            *pDataStructure = interactionAppCommandCallbackData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::InteractionData* pDataStructure) {
            ClientDataTypes::InteractionData interactionData = *pDataStructure;

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                ClientDataTypes::InteractionResponseType theValue = jsonObjectData.at("type");
                interactionData.type = theValue;
            }

            if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
                ClientDataTypes::InteractionApplicationCommandCallbackData theValue = interactionData.data;
                parseObject(jsonObjectData.at("data"), &theValue);
                interactionData.data = theValue;
            }

            *pDataStructure = interactionData;
        }

        void parseObject(json jsonObjectData, ClientDataTypes::MessageData* pDataStructure) {
            ClientDataTypes::MessageData messageData = *pDataStructure;

            if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
                std::string theValue = jsonObjectData.at("content");
                messageData.content = theValue;
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                std::string theValue = jsonObjectData.at("id");
                messageData.id = theValue;
            }            

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                std::string theValue = jsonObjectData.at("channel_id");
                messageData.channelId = theValue;
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                std::string theValue = jsonObjectData.at("guild_id");
                messageData.guildId = theValue;
            }

            if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
                ClientDataTypes::UserData theValue = messageData.author;
                parseObject(jsonObjectData.at("author"), &theValue);
                messageData.author = theValue;
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                ClientDataTypes::GuildMemberData theValue = messageData.member;
                parseObject(jsonObjectData.at("member"), &theValue);
                messageData.member = theValue;
            }
            
            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                std::string theValue = jsonObjectData.at("timestamp");
                messageData.timestamp = theValue;
            }
            
            if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
                std::string theValue = jsonObjectData.at("edited_timestamp");
                messageData.editedTimestamp = theValue;
            }
            
            if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
                bool theValue = jsonObjectData.at("tts");
                messageData.tts = theValue;
            }

            if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
                bool theValue = jsonObjectData.at("mention_everyone");
                messageData.mentionEveryone = theValue;
            } 

            if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
                json userArray = jsonObjectData.at("mentions");
                for (unsigned int x = 0; x < userArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < messageData.mentions.size(); y += 1) {
                        if (messageData.mentions.at(y).id == userArray.at(x).at("id")) {
                            isItFound = true;
                            ClientDataTypes::UserData userData = messageData.mentions.at(y);
                            parseObject(userArray.at(x), &userData);
                            messageData.mentions.erase(messageData.mentions.begin() + y);
                            messageData.mentions.push_back(userData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::UserData userData;
                        parseObject(userArray.at(x), &userData);
                        messageData.mentions.push_back(userData);
                    }
                }
            }
 
            if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
                json roleMentionArray = jsonObjectData.at("mention_roles");
                for (unsigned int x = 0; x < roleMentionArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < messageData.mentionRoles.size(); y += 1) {
                        if (messageData.mentionRoles.at(y).c_str() == roleMentionArray.at(x)) {
                            isItFound = true;
                            break;
                        }
                    }
                    if (isItFound == false) {
                        messageData.mentionRoles.push_back(roleMentionArray.at(x));
                    }
                }
            }
            
            if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
                json mentionChannelsArray = jsonObjectData.at("mention_channels");
                for (unsigned int x = 0; x < mentionChannelsArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < messageData.mentionChannels.size(); y += 1) {
                        if (messageData.mentionChannels.at(y).id == mentionChannelsArray.at(x).at("id")) {
                            isItFound = true;
                            ClientDataTypes::ChannelMentionData channelMentionData = messageData.mentionChannels.at(y);
                            parseObject(mentionChannelsArray.at(x), &channelMentionData);
                            messageData.mentionChannels.erase(messageData.mentionChannels.begin() + y);
                            messageData.mentionChannels.push_back(channelMentionData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::ChannelMentionData channelMentionData;
                        parseObject(mentionChannelsArray.at(x), &channelMentionData);
                        messageData.mentionChannels.push_back(channelMentionData);
                    }
                }
            }

            if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
                json attachmentsArray = jsonObjectData.at("attachments");
                for (unsigned int x = 0; x < attachmentsArray.size(); x += 1) {
                    bool isItFound = false;
                    for (unsigned int y = 0; y < messageData.attachments.size(); y += 1) {
                        if (messageData.attachments.at(y).id == attachmentsArray.at(x).at("id")) {
                            isItFound = true;
                            ClientDataTypes::AttachmentData attachmentData = messageData.attachments.at(y);
                            parseObject(attachmentsArray.at(x), &attachmentData);
                            messageData.attachments.erase(messageData.attachments.begin() + y);
                            messageData.attachments.push_back(attachmentData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::AttachmentData attachmentData;
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
                        if (messageData.embeds.at(y).author.name == embedsArray.at(x).at("author").at("name")) {
                            isItFound = true;
                            ClientDataTypes::EmbedData embedData = messageData.embeds.at(y);
                            parseObject(embedsArray.at(x), &embedData);
                            messageData.embeds.erase(messageData.embeds.begin() + y);
                            messageData.embeds.push_back(embedData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::EmbedData embedData;
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
                            ClientDataTypes::ReactionData reactionData = messageData.reactions.at(y);
                            parseObject(reactionsArray.at(x), &reactionData);
                            messageData.reactions.erase(messageData.reactions.begin() + y);
                            messageData.reactions.push_back(reactionData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::ReactionData reactionData;
                        parseObject(reactionsArray.at(x), &reactionData);
                        messageData.reactions.push_back(reactionData);
                    }
                }
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
                std::string theValue = jsonObjectData.at("nonce");
                messageData.nonce = theValue;
            }

            if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
                bool theValue = jsonObjectData.at("pinned");
                messageData.pinned = theValue;
            }

            if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
                std::string theValue = jsonObjectData.at("webhook_id");
                messageData.webhookId = theValue;
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                int theValue = jsonObjectData.at("type");
                messageData.type = theValue;
            }

            if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
                ClientDataTypes::MessageActivityData theValue = messageData.activity;
                parseObject(jsonObjectData.at("activity"), &theValue);
                messageData.activity = theValue;
            }

            if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
                ClientDataTypes::ApplicationData theValue = messageData.application;
                parseObject(jsonObjectData.at("application"), &theValue);
                messageData.application = theValue;
            }

            if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
                ClientDataTypes::MessageReferenceData theValue = messageData.messageReference;
                parseObject(jsonObjectData.at("application"), &theValue);
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
                            ClientDataTypes::MessageStickerData messageStickerData = messageData.stickers.at(y);
                            parseObject(stickersArray.at(x), &messageStickerData);
                            messageData.stickers.erase(messageData.stickers.begin() + y);
                            messageData.stickers.push_back(messageStickerData);
                        }
                    }
                    if (isItFound == false) {
                        ClientDataTypes::MessageStickerData messageStickerData;
                        parseObject(stickersArray.at(x), &messageStickerData);
                        messageData.stickers.push_back(messageStickerData);
                    }
                }
            }

            if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
                ClientDataTypes::InteractionData theValue = messageData.interaction;
                parseObject(jsonObjectData.at("interaction"), &theValue);
                messageData.interaction = theValue;
            }

            /*
            InteractionData interaction;
            */


            *pDataStructure = messageData;
        }
	}
};