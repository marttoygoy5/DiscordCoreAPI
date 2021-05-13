// DiscordDataStructs.hpp - Header for all of the Discord API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_DATA_STRUCTS_
#define _DISCORD_DATA_STRUCTS_

#include "pch.h"

namespace DiscordCoreInternal {

    struct DeleteReactionData {
        string channelId;
        string messageId;
        string emojiName;
        string emojiId;
        string userId;
    };

    struct DeleteOwnReactionData {
        string channelId;
        string messageId;
        string emojiName;
        string emojiId;
    };

    struct DeleteAllReactionsByEmojiData {
        string channelId;
        string messageId;
        string emojiName;
        string emojiId;
    };


    struct DeleteAllReactionsData {
        string channelId;
        string messageId;
    };

    struct AllowedMentionsData {
        vector<string> parse;
        vector<string> roles;
        vector<string> users;
        bool repliedUser;
    };

    struct MessageReferenceData {
        string messageId;
        string channelId;
        string guildId;
        bool failIfNotExists;
    };

    struct AttachmentData {
        string id = "";
        string filename = "";
        string contentType = "";
        int size = 0;
        string url = "";
        string proxyUrl = "";
        int height = 0;
        int width = 0;
    };

    struct EmbedFooterData {
        string iconUrl;
        string text;
        string proxyIconUrl;
    };

    struct EmbedImageData {
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedThumbnailData {
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedVideoData {
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedProviderData {
        string name;
        string url;
    };

    struct EmbedAuthorData {
        string name;
        string url;
        string iconUrl;
        string proxyIconUrl;
    };

    struct EmbedFieldData {
        string name;
        string value;
        bool Inline;
    };

    struct EmbedData {
        EmbedData setTitle(string titleNew) {
            this->title = titleNew;
            return *this;
        }
        EmbedData setAuthor(string authorName, string authorAvatarURL = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarURL;
            return *this;
        }
        EmbedData setImage(string imageURL) {
            this->image.url = imageURL;
            return *this;
        }
        EmbedData setThumbnail(string thumbnailURL) {
            this->thumbnail.url = thumbnailURL;
            return *this;
        }
        EmbedData setColor(int colorNew[3]) {
            this->color[0] = colorNew[0];
            this->color[1] = colorNew[1];
            this->color[2] = colorNew[2];
            return *this;
        }
        EmbedData setDescription(string descriptionNew) {
            this->description = descriptionNew;
            return *this;
        }
        EmbedData setFooter(string footerText, string footerIconURLText = "") {
            this->footer.text = footerText;
            this->footer.iconUrl = footerIconURLText;
            return *this;
        }
        EmbedData setTimeStamp(string timeStamp) {
            time_t now = time(0);
            char* dt = ctime(&now);
            this->timestamp = dt;
            return *this;
        }
        EmbedData addField(string name, string value, bool Inline = true) {
            EmbedFieldData embedFieldData;
            embedFieldData.name = name;
            embedFieldData.Inline = Inline;
            embedFieldData.value = value;
            this->fields.push_back(embedFieldData);
            return *this;
        }
        string title;
        string type;
        string description;
        string url;
        string timestamp;
        int color[3];
        EmbedFooterData footer;
        EmbedImageData image;
        EmbedThumbnailData thumbnail;
        EmbedVideoData video;
        EmbedProviderData provider;
        EmbedAuthorData author;
        vector<EmbedFieldData> fields;
        int actualColor() {
            if (this->color[0] > 255) {
                this->color[0] = 255;
            }
            else if (this->color[0] < 0) {
                this->color[0] = 0;
            }
            if (this->color[1] > 255) {
                this->color[1] = 255;
            }
            else if (this->color[1] < 0) {
                this->color[1] = 0;
            }
            if (this->color[2] > 255) {
                this->color[2] = 255;
            }
            else if (this->color[2] < 0) {
                this->color[2] = 0;
            }
            int colorValue = 65536 * this->color[0] + 256 * this->color[1] + this->color[2];
            return colorValue;
        };
        int actualColorVal = actualColor();
    };

    struct CreateMessageData {
        int nonce;
        bool tts = false;
        string content;
        EmbedData embed;
        AllowedMentionsData allowedMentions;
        MessageReferenceData messageReference;
    };

    struct EditMessageData {
        string content;
        EmbedData embed;
        int flags;
        AllowedMentionsData allowedMentions;
        vector<AttachmentData> attachments;
    };

    enum class Permissions {
        CREATE_INSTANT_INVITE = (1 << 0),
        KICK_MEMBERS = (1 << 1),
        BAN_MEMBERS = (1 << 2),
        ADMINISTRATOR = (1 << 3),
        MANAGE_CHANNELS = (1 << 4),
        MANAGE_GUILD = (1 << 5),
        ADD_REACTIONS = (1 << 6),
        VIEW_AUDIT_LOG = (1 << 6),
        PRIORITY_SPEAKER = (1 << 8),
        STREAM = (1 << 9),
        VIEW_CHANNEL = (1 << 10),
        SEND_MESSAGES = (1 << 11),
        SEND_TTS_MESSAGES = (1 << 12),
        MANAGE_MESSAGES = (1 << 13),
        EMBED_LINKS = (1 << 14),
        ATTACH_FILES = (1 << 15),
        READ_MESSAGE_HISTORY = (1 << 16),
        MENTION_EVERYONE = (1 << 17),
        USE_EXTERNAL_EMOJIS = (1 << 18),
        VIEW_GUILD_INSIGHTS = (1 << 19),
        CONNECT = (1 << 20),
        SPEAK = (1 << 21),
        MUTE_MEMBERS = (1 << 22),
        DEAFEN_MEMBERS = (1 << 23),
        MOVE_MEMBERS = (1 << 24),
        USE_VAD = (1 << 25),
        CHANGE_NICKNAME = (1 << 26),
        MANAGE_NICKNAMES = (1 << 27),
        MANAGE_ROLES = (1 << 28),
        MANAGE_WEBHOOKS = (1 << 29),
        MANAGE_EMOJIS = (1 << 30),
        USE_SLASH_COMMANDS = (1 << 31),
        REQUEST_TO_SPEAK = (1 << 32),
        MANAGE_THREADS = (1 << 34),
        USE_PUBLIC_THREADS = (1 << 35),
        USE_PRIVATE_THREADS = (1 << 36)
    };

    enum class UserFlags {
        NONE = 0,
        DISCORD_EMPLOYEE = 1 << 0,
        PARTNERED_SERVER_OWNER = 1 << 1,
        HYPESQUAD_EVENTS = 1 << 2,
        BUG_HUNTER_LEVEL_1 = 1 << 3,
        HOUSE_BRAVERY = 1 << 6,
        HOUSE_BRILLIANCE = 1 << 7,
        HOUSE_BALANCE = 1 << 8,
        EARLY_SUPPORTER = 1 << 9,
        TEAM_USER = 1 << 10,
        BUG_HUNTER_LEVEL_2 = 1 << 14,
        VERIFIED_BOT = 1 << 16,
        EARLY_VERIFIED_BOT_DEVELOPER = 1 << 17
    };

    enum class DefaultMessageNotificationLevel {
        ALL_MESSAGES = 0,
        ONLY_MENTIONS = 1
    };

    enum class ExplicitContentFilterLevel {
        DISABLED = 0,
        MEMBERS_WITHOUT_ROLES = 1,
        ALL_MEMBERS = 2
    };

    enum class MFALevel {
        NONE = 0,
        ELEVATED = 1
    };

    enum class VerificationLevel {
        NONE = 0,
        LOW = 1,
        MEDIUM = 2,
        HIGH = 3,
        VERY_HIGH = 4
    };

    enum class PremiumTier {
        NONE = 0,
        TIER_2 = 2,
        TIER_1 = 1,
        TIER_3 = 3
    };

    enum class NitroSubscriptionLevel {
        NONE = 0,
        NITRO_CLASSIC = 1,
        NITRO = 2
    };

    enum class InteractionResponseType {
        Pong = 1,
        Acknowledge = 2,
        ChannelMessage = 3,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5
    };

    struct UserData {
        string username;
        string id;
        string discriminator;
        string avatar;
        bool bot;
        bool system = false;
        bool mfa_enabled = false;
        string locale;
        bool verified = false;
        string email;
        int flags = 0;
        int premium_type = 0;
        int public_flags = 0;
    };

    struct GuildMemberData {
        UserData user;
        string nick;
        vector<string> roles;
        string joined_at;
        string premium_since;
        bool deaf;
        bool mute;
        bool pending;
        string permissions;
    };

    struct WelcomeScreenChannelData {
        string channelId;
        string description;
        string emojiId;
        string emojiName;
    };

    struct WelcomeScreenData {
        string description;
        vector<WelcomeScreenChannelData> welcomeChannels{};
    };

    struct PartyData {
        string id;
        int size[2]{};
    };

    struct AssetsData {
        string largeImage;
        string largeText;
        string smallImage;
        string smallText;
    };

    struct SecretsData {
        string join;
        string spectate;
        string match;
    };

    struct TimestampData {
        int start;
        int end;
    };

    struct ButtonData {
        string label;
        string url;
    };

    struct RoleTagsData {
        string botId;
        string integrationId;
    };

    struct RoleData {
        string id;
        string name;
        int color;
        bool hoist;
        int position;
        string permissions;
        bool managed;
        bool mentionable;
        RoleTagsData tags;
    };

    struct EmojiData {
        string id;
        string name;
        map<string, RoleData> roles;
        UserData user;
        bool requireColons;
        bool managed;
        bool animated;
        bool available;
    };

    struct ReactionData {
        int count;
        bool me;
        EmojiData emoji;
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
    };

    struct ActivityData {
        string name;
        int type;
        string url;
        int createdAt;
        TimestampData timestamps;
        string applicationId;
        string details;
        string state;
        EmojiData emoji;
        PartyData party;
        AssetsData assets;
        SecretsData secrets;
        bool instance;
        int flags;
        vector<ButtonData> buttons;
    };

    struct ClientStatusData {
        string desktop;
        string mobile;
        string web;
    };

    struct PresenceUpdateData {
        UserData user;
        string guildId;
        string status;
        vector<ActivityData> activities;
        ClientStatusData clientStatus;
    };

    struct OverWriteData {
        string id;
        int type;	           //	either 0 (role) or 1 (member)
        string allow;
        string deny;
    };

    struct ChannelData {
        string id;
        int type;
        string guildId;
        int position;
        map<string, OverWriteData> permissionOverwrites;
        string name;
        string topic;
        bool nsfw;
        string lastMessageId;
        int bitrate;
        int userLimit;
        int rateLimitPerUser;
        vector<UserData> recipients;
        string icon;
        string ownerId;
        string applicationId;
        string parentId;
        string lastPinTimestamp;
        string rtcRegion;
        int videoQualityMode;
    };

    struct VoiceStateData {
        string guildId;
        string channelId;
        string userId;
        GuildMemberData guildMember;
        string sessionId;
        bool deaf;
        bool mute;
        bool selfDeaf;
        bool selfMute;
        bool selfStream;
        bool selfVideo;
        bool suppress;
        string requestToSpeakTimestamp;
    };

    struct GuildData {
        string icon;
        string name;
        string id;
        string iconHash;
        string splash;
        string discoverySplash;
        string preferredLocale;
        string publicUpdatesChannelID;
        string vanityURLCode;
        string description;
        string banner;
        string ruleChannelID;
        string applicationID;
        string joinedAt;
        string widgetChannelID;
        string systemChannelID;
        string region;
        string afkChannelID;
        string ownerID;
        vector<string> features{};
        string permissions;
        bool owner = false;
        int afkTimeOut = 0;
        bool widgetEnabled = false;
        VerificationLevel verificationLevel = VerificationLevel::NONE;
        DefaultMessageNotificationLevel defaultMessageNotifications = DefaultMessageNotificationLevel::ALL_MESSAGES;
        ExplicitContentFilterLevel explicitContentFilter = ExplicitContentFilterLevel::DISABLED;
        vector<EmojiData> emoji{};
        map<string, RoleData> roles{};
        int systemChannelFlags = 0;
        MFALevel mfaLevel = MFALevel::NONE;
        bool large = false;
        bool unavailable = false;
        int memberCount = 0;
        vector<VoiceStateData> voiceStates{};
        vector<PresenceUpdateData> presences{};
        int maxPresences = 0;
        int maxMembers = 0;
        int premiumSubscriptionCount = 0;
        PremiumTier premiumTier = PremiumTier::NONE;
        int maxVideoChannelUsers = 0;
        int approximateMemberCount = 0;
        int approximatePresenceCount = 0;
        WelcomeScreenData welcomeScreen{};
        vector<GuildMemberData> members{};
        vector<ChannelData> channels{};
    };

    struct ChannelMentionData {
        string id;
        string guildId;
        int type;
        string name;
    };

    struct MessageActivityData {
        int type;
        string partyId;
    };

    struct TeamMembersObjectData {
        int membershipState;
        vector<string> permissions;
        string teamId;
        UserData user;
    };

    struct TeamObjectData {
        string icon;
        string id;
        vector<TeamMembersObjectData> members;
        string ownerUserId;
    };

    struct ApplicationData {
        string id;
        string name;
        string icon;
        string description;
        vector<string> rpcOrigins;
        bool botPublic;
        bool botRequireCodeGrant;
        string termsOfServiceUrl;
        string privacyPolicyUrl;
        UserData owner;
        string  summary;
        string verifyKey;
        TeamObjectData team;
        string guildId;
        string primarySkuId;
        string slug;
        string coverImage;
        int flags;
    };

    struct MessageStickerData {
        string id;
        string packId;
        string name;
        string description;
        string tags;
        string asset;
        int formatType;
    };

    struct InteractionApplicationCommandCallbackData {
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags;
    };

    struct InteractionData {
        InteractionResponseType type;
        InteractionApplicationCommandCallbackData data;
    };

    struct MessageDataOld {
        string id;
        string channelId;
        string guildId;
        UserData author;
        GuildMemberData member;
        string content;
        string timestamp;
        string editedTimestamp;
        bool tts;
        bool mentionEveryone;
        vector<UserData> mentions;
        vector<string> mentionRoles;
        vector<ChannelMentionData> mentionChannels;
        vector<AttachmentData> attachments;
        vector<EmbedData> embeds;
        vector<ReactionData> reactions;
        string nonce;
        bool pinned;
        string webhookId;
        int type;
        MessageActivityData activity;
        ApplicationData application;
        MessageReferenceData messageReference;;
        int flags;
        vector<MessageStickerData> stickers;
        InteractionData interaction;
    };

    struct MessageData : MessageDataOld {
        MessageDataOld referencedMessage;
    };

    struct CreateReactionData {
        string name;
        string id;
    };

    struct ReactionAddEventData {
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
        EmojiData emoji;
    };
}
#endif