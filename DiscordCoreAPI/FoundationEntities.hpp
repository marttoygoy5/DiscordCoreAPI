// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_ENTITIES_
#define _FOUNDATION_ENTITIES_

#include "pch.h"

namespace DiscordCoreAPI {
    class DiscordCoreClient;
};

namespace  DiscordCoreInternal {

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
        string id;
        string filename;
        string contentType;
        int size;
        string url;
        string proxyUrl;
        int height;
        int width;
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
        EmbedData setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
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
            this->timestamp = timeStamp;
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
        string hexColorValue;
        EmbedFooterData footer;
        EmbedImageData image;
        EmbedThumbnailData thumbnail;
        EmbedVideoData video;
        EmbedProviderData provider;
        EmbedAuthorData author;
        vector<EmbedFieldData> fields;
    };

    enum class Permissions :__int64 {
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
        USE_SLASH_COMMANDS = (1ull << 31),
        REQUEST_TO_SPEAK = (1ull << 32),
        MANAGE_THREADS = (1ull << 33),
        USE_PUBLIC_THREADS = (1ull << 34),
        USE_PRIVATE_THREADS = (1ull << 35)
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

    enum class InteractionCallbackType {
        Pong = 1,
        Acknowledge = 2,
        ChannelMessage = 3,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5,
        DeferredUpdateMessage = 6,
        UpdateMessage = 7
    };

    enum class ChannelType {
        GUILD_TEXT = 0,
        DM = 1,
        GUILD_VOICE = 2,
        GROUP_DM = 3,
        GUILD_CATEGORY = 4,
        GUILD_NEWS = 5,
        GUILD_STORE = 6,
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
        GUILD_STAGE_VOICE = 13
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
        string getAvatarURL() {
            if (this->avatar != "") {
                return "https://cdn.discordapp.com/avatars/" + this->id + "/" + this->avatar;
            }
            else {
                return "";
            }
        }
    };

    struct GuildMemberData {
        string guildId;
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
        string id = "";
        string name = "";
        vector<RoleData> roles;
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
        int flags = 0;
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
        ChannelType type = ChannelType::DM;
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

    struct ThreadContext {
        Scheduler* scheduler{ nullptr };
        shared_ptr<task_group> taskGroup{ nullptr };
        shared_ptr<DispatcherQueue> threadQueue{ nullptr };
    };

    struct HttpAgentResources {
        hstring botToken;
        hstring baseURL;
    };

    struct DeleteInteractionResponseData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string applicationId;
        string interactionToken;
        unsigned int timeDelayInMs;
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
        int flags = 0;
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

    enum class ComponentType {
        ActionRow = 1,
        Button = 2
    };

    enum class ButtonStyle {
        Primary = 1,
        Secondary = 2,
        Success = 3,
        Danger = 4,
        Link = 5
    };

    struct ComponentData {
        ComponentType type;
        ButtonStyle style;
        string label = "";
        EmojiData emoji;
        string customId = "";
        string url = "";
        bool disabled = false;
    };

    struct ActionRowData {
        vector<ComponentData> components;
    };

    struct InteractionApplicationCommandCallbackData {
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags = 0;
        vector<DiscordCoreInternal::ActionRowData> components;
    };

    enum class MessageType {
        DEFAULT	= 0,
        RECIPIENT_ADD = 1,
        RECIPIENT_REMOVE = 2,
        CALL = 3,
        CHANNEL_NAME_CHANGE = 4,
        CHANNEL_ICON_CHANGE = 5,
        CHANNEL_PINNED_MESSAGE = 6,
        GUILD_MEMBER_JOIN = 7,
        USER_PREMIUM_GUILD_SUBSCRIPTION = 8,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
        CHANNEL_FOLLOW_ADD = 12,
        GUILD_DISCOVERY_DISQUALIFIED = 14,
        GUILD_DISCOVERY_REQUALIFIED = 15,
        GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING = 16,
        GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING = 17,
        THREAD_CREATED = 18,
        REPLY = 19,
        APPLICATION_COMMAND = 20,
        THREAD_STARTER_MESSAGE = 21,
        GUILD_INVITE_REMINDER = 22
    };

    struct ReactionAddEventData {
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
        EmojiData emoji;
    };

    enum class HttpWorkloadClass {
        GET = 0,
        PUT = 1,
        POST = 2,
        PATCH = 3,
        DELETED = 4
    };

    enum class HttpWorkloadType {
        UNSET = 0,
        GET_MESSAGE = 1,
        POST_MESSAGE = 2,
        DELETE_MESSAGE = 3,
        GET_USER = 4,
        GET_USER_SELF = 5,
        GET_GUILD = 6,
        GET_CHANNEL = 7,
        GET_REACTION = 8,
        PUT_REACTION = 9,
        DELETE_REACTION = 10,
        PATCH_MESSAGE = 11,
        DELETE_ALL_REACTION = 12,
        GET_GUILD_MEMBER = 13,
        GET_GUILD_MEMBERS = 14,
        GET_ROLES = 15,
        GET_USER_GUILDS = 16,
        POST_USER_DM = 17,
        GET_DM_CHANNEL = 18,
        PATCH_ROLE = 19,
        GET_APPLICATION = 20,
        POST_APPLICATION_COMMAND = 21,
        GET_SLASH_COMMANDS = 22,
        GET_SOCKET_PATH = 23,
        DELETE_SLASH_COMMAND = 24,
        PATCH_SLASH_COMMAND = 25,
        POST_INTERACTION_RESPONSE = 26,
        PATCH_INTERACTION_RESPONSE = 27,
        POST_ROLE = 28,
        PUT_GUILD_MEMBER_ROLE = 30,
        DELETE_GUILD_MEMBER_ROLE = 31,
        PUT_CHANNEL_PERMISSION_OVERWRITES = 32,
        DELETE_CHANNEL_PERMISSION_OVERWRITES = 33,
        POST_FOLLOW_UP_MESSAGE = 34,
        DELETE_INTERACTION = 35,
        PATCH_FOLLOW_UP_MESSAGE = 36,
        POST_DEFERRED_INTERACTION_RESPONSE = 37,
        GET_MESSAGES = 38,
        DELETE_MESSAGES_BULK = 39
    };

    struct GetApplicationData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
    };
    
    struct EditInteractionResponseData {
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        string  content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<AttachmentData> attachments;
        vector<ActionRowData> components;
        string finalContent;
        string applicationId;
        int flags;
        InteractionCallbackType type;
        string interactionToken;
    };
    
    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data;
        InteractionCallbackType type;
    };

    struct CreateInteractionResponseData {
        InteractionApplicationCommandCallbackData data;
        InteractionCallbackType type;
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        string content;
        string interactionId;
        string interactionToken;
        string finalContent;
    };

    struct CreateFollowUpMessageData {
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        string content;
        string username;
        string avatarURL;
        bool tts;
        int flags;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        string applicationId;
        string interactionToken;
    };

    struct CreateDeferredInteractionResponseData {
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        DiscordCoreInternal::InteractionCallbackType type;
        string interactionId;
        string interactionToken;
        string content;
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct MessageInteractionData {
        string id;
        InteractionType type;
        string name;
        UserData user;
    };

    struct HttpData {
        unsigned int returnCode = 0;
        string returnMessage = "";
        json data;
    };

    struct FetchMessagesData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        unsigned int limit;
        string beforeThisId;
        string afterThisId;
        string aroundThisId;
    };

    struct DeleteMessagesBulkData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        vector<string> messageIds;
        string content;
    };

    struct HttpWorkload {
        HttpWorkloadClass workloadClass;
        HttpWorkloadType workloadType;
        string relativePath;
        string content;
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
        MessageType type;
        MessageActivityData activity;
        ApplicationData application;
        MessageReferenceData messageReference;
        int flags = 0;
        vector<MessageStickerData> stickers;
        MessageInteractionData interaction;
        vector<ActionRowData> components;
    };

    struct MessageData : MessageDataOld {
        MessageDataOld referencedMessage;
    };

    struct RateLimitData {
        HttpWorkloadType workloadType;
        int getsRemaining = 1;
        float msRemain = 0.0f;
        float timeStartedAt = 0.0f;
        string bucket;
    };

    struct GetGuildData {
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        GuildData oldGuildData;
        string guildId;
    };

    struct FetchGuildData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        GuildData oldGuildData;
        string guildId;
    };

    struct PutReactionData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        string messageId;
        string emoji;
    };

    enum class ReactionDeletionType {
        SELF_DELETE = 0,
        USER_DELETE = 1,
        EMOJI_DELETE = 2,
        ALL_DELETE = 3
    };

    struct DeleteReactionDataAll {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        string messageId;
        string encodedEmoji;
        string userId;
        ReactionDeletionType deletionType;
    };

    struct GetReactionData {
        string channelId;
        string messageId;
        string userId;
        string emojiName;
    };

    struct GetChannelData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
    };

    struct FetchChannelData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
    };

    struct EditChannelPermissionOverwritesData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string allow;
        string deny;
        unsigned int type;
        string roleOrUserId;
        string content;
        string channelId;
    };

    struct DeleteChannelPermissionOverwritesData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string roleOrUserId;
        string channelId;
    };

    struct GetDMChannelData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string userId;
    };

    struct GetMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        string messageId;
        string requesterId;
    };

    enum class GetUserDataType {
        SELF = 0,
        USER = 1
    };

    struct GetUserData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string userId;
        GetUserDataType userType;
    };
    
    struct FetchUserData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string userId;
        GetUserDataType userType;
    };

    struct PostMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        string finalContent;
        string requesterId;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        DiscordCoreInternal::MessageReferenceData messageReference;
        string content;
        DiscordCoreInternal::EmbedData embed;
        int nonce;
        MessageData replyingToMessageData;
        bool tts = false;
        vector<ActionRowData> components;
    };

    struct PatchMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string content;
        string channelId;
        string messageId;
        string requesterId;
        string finalContent;
        DiscordCoreInternal::EmbedData embed;
        int flags = 0;
        vector<DiscordCoreInternal::AttachmentData> attachments;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
    };

    struct DeleteMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        unsigned int timeDelay = 0;
        string channelId;
        string messageId;
    };

    struct SendDMData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string userId;
        string channelId;
        string finalContent;
        string content;
        int nonce;
        bool tts = false;
        DiscordCoreInternal::EmbedData embed;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        DiscordCoreInternal::MessageReferenceData messageReference;
        vector<ActionRowData> components;
    };

    struct GetGuildMemberData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        GuildMemberData oldGuildMemberData;
        string guildId;
        string guildMemberId;
    };

    struct GetGuildMemberRolesData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        vector<string> roleIds;
    };

    struct FetchRolesData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
    };

    struct GetRoleData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
        string roleId;
    };

    struct DeleteRoleData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
        string userId;
        string roleId;
    };

    struct FetchRoleData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
        string roleId;
    };

    struct PostRoleData {
        string content;
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string guildId;
        string roleId;
        string name;
        __int64 permissions;
        string hexColorValue;
        bool hoist;
        bool mentionable;
    };

    struct PutRoleData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
        string userId;
        string roleId;
    };

    struct PatchRoleData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string guildId;
        string roleId;
        string content;
        string name;
        string permissions;
        string hexColorValue;
        bool hoist;
        bool mentionable;
    };

    enum class ApplicationCommandOptionType {
        SUB_COMMAND = 1,
        SUB_COMMAND_GROUP = 2,
        STRING = 3,
        INTEGER = 4,
        BOOLEAN = 5,
        USER = 6,
        CHANNEL = 7,
        ROLE = 8,
        MENTIONABLE = 9
    };

    struct ApplicationCommandOptionChoiceData {
        string name = "";
        int	valueInt = 0;
        string valueString = "";
    };

    struct ApplicationCommandOptionData {
        ApplicationCommandOptionType type = ApplicationCommandOptionType::SUB_COMMAND_GROUP;
        string name = "";
        string description = "";
        bool required = false;
        vector<ApplicationCommandOptionChoiceData>	choices;
        vector<ApplicationCommandOptionData> options;
    };

    struct ApplicationCommandData {
        string id;
        string applicationId;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct EditApplicationCommandData {
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct CreateApplicationCommandData {
        string applicationId;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct CommandData {
        vector<string> optionsArgs;
        string commandName;
    };

    struct ApplicationCommandInteractionDataResolved {
        map<string, UserData> users{};
        map<string, GuildMemberData> members{};
        map<string, RoleData> roles{};
        map<string, ChannelData> channels{};
    };

    struct ApplicationCommandInteractionDataOption {
        string name;
        ApplicationCommandOptionType type;
        bool valueBool;
        string valueString;
        int valueInt;
        vector<ApplicationCommandInteractionDataOption> options;
    };

    struct ApplicationCommandInteractionData {
        string name;
        string id;
        ApplicationCommandInteractionDataResolved resolved;
        vector<ApplicationCommandInteractionDataOption> options;
        string customId;
        int componentType;
    };

    struct InteractionData {
        string id;
        string applicationId;
        InteractionType type;
        ApplicationCommandInteractionData data;
        json dataRaw;
        string guildId;
        string channelId;
        GuildMemberData member;
        UserData user;
        string token;
        int version;
        MessageData message;
        string customId;
    };

    struct InteractionResponseFullData {
        InteractionData interactionData;
        CreateInteractionResponseData interactionResponseData;
    };

    class StopWatch {
    public:
        StopWatch(unsigned long long maxNumberOfMsNew) {
            this->maxNumberOfMs = maxNumberOfMsNew;
            this->startTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        bool hasTimePassed() {
            unsigned long long currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
            unsigned long long elapsedTime = currentTime - this->startTime;
            if (elapsedTime >= this->maxNumberOfMs) {
                return true;
            }
            else {
                return false;
            }
        }

    protected:
        unsigned long long maxNumberOfMs;
        unsigned long long startTime;
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3
    };

    struct InputEventData {
        InputEventData(DiscordCoreInternal::MessageData messageData, DiscordCoreInternal::InteractionData interactionData, DiscordCoreInternal::InputEventType eventType) {
            this->messageData = messageData;
            this->interactionData = interactionData;
            this->eventType = eventType;
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
        }
        DiscordCoreAPI::DiscordCoreClient* discordCoreClient;
        InteractionData interactionData;
        MessageData messageData;
        InputEventType eventType;
        string requesterId;
        string getUserName() {
            if (this->messageData.author.username == "") {
                return this->interactionData.member.user.username;
            }
            else {
                return this->messageData.author.username;
            }
        }
        string getAvatar() {
            if (this->messageData.author.getAvatarURL() == "") {
                
                return this->interactionData.member.user.getAvatarURL();
            }
            else {
                return this->messageData.author.getAvatarURL();
            }
        }
        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else {
                return this->interactionData.channelId;
            }
        };
        string getMessageId() {
            if (this->messageData.id == "") {
                return this->interactionData.message.id;
            }
            else {
                return this->messageData.id;
            }
        }
        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else {
                return this->messageData.author.id;
            }
        }
        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else {
                return this->messageData.guildId;
            }
        }
    };

    enum class InputEventResponseType {
        UNSET = 0,
        REGULAR_MESSAGE_RESPONSE = 1,
        REGULAR_MESSAGE_EDIT = 2,
        REGULAR_MESSAGE_DELETE = 3,
        INTERACTION_RESPONSE = 4,
        INTERACTION_RESPONSE_DEFERRED = 5,
        INTERACTION_RESPONSE_EPHEMERAL = 6,
        INTERACTION_RESPONSE_EDIT = 7,
        INTERACTION_RESPONSE_DELETE = 8,
        INTERACTION_FOLLOW_UP_MESSAGE = 9,
        INTERACTION_FOLLOW_UP_MESSAGE_EDIT = 10,
        INTERACTION_FOLLOW_UP_MESSAGE_DELETE = 11,
        DEFER_BUTTON_RESPONSE = 12
    };

    struct InputEventResponseData {
        InputEventResponseData(InputEventResponseType responseType) {
            if (responseType == InputEventResponseType::UNSET) {
                throw exception("You forgot to set the event response type!");
            }
            else {
                this->eventResponseType = responseType;
            }
        }
        InputEventResponseData() = delete;
        InputEventResponseType eventResponseType;
        InteractionCallbackType type;
        string channelId;
        string messageId;
        string interactionToken;
        string applicationId;
        string interactionId;
        string finalContent;
        string requesterId;
        MessageReferenceData messageReference;
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags = 0;
        vector<ActionRowData> components;
    };

    struct EditFollowUpMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string applicationId;
        string interactionToken;
        string messageId;
        string finalContent;
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<DiscordCoreInternal::EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<DiscordCoreInternal::ActionRowData> components;
        int flags;
    };

}

namespace DiscordCoreAPI {
    class DiscordCoreClient;

    enum class Permissions :__int64 {
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
        USE_SLASH_COMMANDS = (1ull << 31),
        REQUEST_TO_SPEAK = (1ull << 32),
        MANAGE_THREADS = (1ull << 33),
        USE_PUBLIC_THREADS = (1ull << 34),
        USE_PRIVATE_THREADS = (1ull << 35)
    };

    string getTimeAndDate() {
        const std::time_t now = std::time(nullptr);
        char charArray[32];
        std::tm time;
        localtime_s(&time, &now);
        strftime(charArray, 32, "%F %R", &time);
        return charArray;
    }

    enum class ComponentType {
        ActionRow = 1,
        Button = 2
    };

    enum class ButtonStyle {
        Primary = 1,
        Success = 3,
        Secondary = 2,
        Danger = 4,
        Link = 5
    };

    struct RoleTagsData {
        string integrationId;
        string botId;
    };

    struct RoleData {
        operator DiscordCoreInternal::RoleData() {
            DiscordCoreInternal::RoleData newData;
            newData.color = this->color;
            newData.hoist = this->hoist;
            newData.id = this->id;
            newData.managed = this->managed;
            newData.mentionable = this->mentionable;
            newData.name = this->name;
            newData.permissions = this->permissions;
            newData.position = this->position;
            newData.tags.botId = this->tags.botId;
            newData.tags.integrationId = this->tags.integrationId;
            return newData;
        }
        RoleTagsData tags;
        string permissions;
        bool mentionable;
        int position;
        string name;
        bool managed;
        bool hoist;
        string id;
        int color;
    };

    struct UserData {
        operator DiscordCoreInternal::UserData() {
            DiscordCoreInternal::UserData newData;
            newData.avatar = this->avatar;
            newData.bot = this->bot;
            newData.discriminator = this->discriminator;
            newData.email = this->email;
            newData.flags = this->flags;
            newData.id = this->id;
            newData.locale = this->locale;
            newData.mfa_enabled = this->mfa_enabled;
            newData.premium_type = this->premium_type;
            newData.public_flags = this->public_flags;
            newData.system = this->system;
            newData.username = this->username;
            newData.verified = this->verified;
            return newData;
        }
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
        string getAvatarURL() {
            if (this->avatar != "") {
                return "https://cdn.discordapp.com/avatars/" + this->id + "/" + this->avatar;
            }
            else {
                return "";
            }
        }
    };

    struct EmojiData {
        operator DiscordCoreInternal::EmojiData() {
            DiscordCoreInternal::EmojiData newData;
            newData.animated = this->animated;
            newData.available = this->available;
            newData.id = this->id;
            newData.managed = this->managed;
            newData.name = this->name;
            newData.requireColons = this->requireColons;
            for (auto value : this->roles) {
                DiscordCoreInternal::RoleData roleData;
                roleData = value;
                newData.roles.push_back(roleData);
            }
            newData.user = this->user;
            return newData;
        }
        string id = "";
        string name = "";
        vector<RoleData> roles;
        UserData user;
        bool requireColons;
        bool managed;
        bool animated;
        bool available;
    };

    struct ComponentData {
        operator DiscordCoreInternal::ComponentData() {
            DiscordCoreInternal::ComponentData newData;
            newData.customId = this->customId;
            newData.disabled = this->disabled;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.style = (DiscordCoreInternal::ButtonStyle)this->style;
            newData.type = (DiscordCoreInternal::ComponentType)this->type;
            newData.url = this->url;
            return newData;
        }
        ComponentType type;
        ButtonStyle style;
        string label = "";
        EmojiData emoji;
        string customId = "";
        string url = "";
        bool disabled = false;
    };

    struct AttachmentData {
        operator DiscordCoreInternal::AttachmentData() {
            DiscordCoreInternal::AttachmentData newData;
            newData.contentType = this->contentType;
            newData.filename = this->filename;
            newData.height = this->height;
            newData.id = this->id;
            newData.proxyUrl = this->proxyUrl;
            newData.size = this->size;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
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
        operator DiscordCoreInternal::EmbedFooterData() {
            DiscordCoreInternal::EmbedFooterData footerData;
            footerData.iconUrl = this->iconUrl;
            footerData.proxyIconUrl = this->proxyIconUrl;
            footerData.text = this->text;
            return footerData;
        };
        string iconUrl;
        string text;
        string proxyIconUrl;
    };

    struct EmbedImageData {
        operator DiscordCoreInternal::EmbedImageData() {
            DiscordCoreInternal::EmbedImageData imageData;
            imageData.height = this->height;
            imageData.proxyUrl = this->proxyUrl;
            imageData.url = this->url;
            imageData.width = this->width;
            return imageData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedThumbnailData {
        operator DiscordCoreInternal::EmbedThumbnailData() {
            DiscordCoreInternal::EmbedThumbnailData newData;
            newData.height = this->height;
            newData.proxyUrl = this->proxyUrl;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedVideoData {
        operator DiscordCoreInternal::EmbedVideoData() {
            DiscordCoreInternal::EmbedVideoData newData;
            newData.height = this->height;
            newData.proxyUrl = this->proxyUrl;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedProviderData {
        operator DiscordCoreInternal::EmbedProviderData() {
            DiscordCoreInternal::EmbedProviderData providerData;
            providerData.name = this->name;
            providerData.url = this->url;
            return providerData;
        }
        string name;
        string url;
    };

    struct EmbedAuthorData {
        operator DiscordCoreInternal::EmbedAuthorData() {
            DiscordCoreInternal::EmbedAuthorData authorData;
            authorData.iconUrl = this->iconUrl;
            authorData.name = this->name;
            authorData.proxyIconUrl = this->proxyIconUrl;
            authorData.url = this->url;
            return authorData;
        };
        string name;
        string url;
        string iconUrl;
        string proxyIconUrl;
    };

    struct EmbedFieldData {
        operator DiscordCoreInternal::EmbedFieldData() {
            DiscordCoreInternal::EmbedFieldData embedFieldData;
            embedFieldData.Inline = this->Inline;
            embedFieldData.name = this->name;
            embedFieldData.value = this->value;
            return embedFieldData;
        }
        string name;
        string value;
        bool Inline;
    };

    struct EmbedData {
        operator DiscordCoreInternal::EmbedData() {
            DiscordCoreInternal::EmbedData embedData;
            embedData.author = this->author;
            embedData.description = this->description;
            for (auto value : this->fields) {
                embedData.fields.push_back(value);
            }
            embedData.footer = this->footer;
            embedData.hexColorValue = this->hexColorValue;
            embedData.image = this->image;
            embedData.provider = this->provider;
            embedData.thumbnail = this->thumbnail;
            embedData.timestamp = this->timestamp;
            embedData.title = this->title;
            embedData.type = this->type;
            embedData.url = this->url;
            embedData.video = this->video;
            return embedData;
        };
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
        EmbedData setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
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
            this->timestamp = timeStamp;
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
        string hexColorValue;
        EmbedFooterData footer;
        EmbedImageData image;
        EmbedThumbnailData thumbnail;
        EmbedVideoData video;
        EmbedProviderData provider;
        EmbedAuthorData author;
        vector<EmbedFieldData> fields;
    };

    struct ActionRowData {
        operator DiscordCoreInternal::ActionRowData() {
            DiscordCoreInternal::ActionRowData newData;
            for (auto value : this->components) {
                DiscordCoreInternal::ComponentData component;
                component = value;
                newData.components.push_back(value);
            }
            return newData;
        }
        vector<ComponentData> components;
    };

    enum class InputEventResponseType {
        UNSET = 0,
        REGULAR_MESSAGE_RESPONSE = 1,
        REGULAR_MESSAGE_EDIT = 2,
        REGULAR_MESSAGE_DELETE = 3,
        INTERACTION_RESPONSE = 4,
        INTERACTION_RESPONSE_DEFERRED = 5,
        INTERACTION_RESPONSE_EPHEMERAL = 6,
        INTERACTION_RESPONSE_EDIT = 7,
        INTERACTION_RESPONSE_DELETE = 8,
        INTERACTION_FOLLOW_UP_MESSAGE = 9,
        INTERACTION_FOLLOW_UP_MESSAGE_EDIT = 10,
        INTERACTION_FOLLOW_UP_MESSAGE_DELETE = 11,
        DEFER_BUTTON_RESPONSE = 12
    };

    struct AllowedMentionsData {
        operator DiscordCoreInternal::AllowedMentionsData() {
            DiscordCoreInternal::AllowedMentionsData newData;
            newData.parse = this->parse;
            newData.repliedUser = this->repliedUser;
            newData.roles = this->roles;
            newData.users = this->users;
            return newData;
        }
        vector<string> parse;
        vector<string> roles;
        vector<string> users;
        bool repliedUser;
    };

    struct InteractionApplicationCommandCallbackData {
        operator DiscordCoreInternal::InteractionApplicationCommandCallbackData() {
            DiscordCoreInternal::InteractionApplicationCommandCallbackData newData;
            newData.allowedMentions = this->allowedMentions;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            newData.content = this->content;
            for (auto value : this->embeds) {
                newData.embeds.push_back(value);
            }
            newData.flags = this->flags;
            newData.tts = this->tts;
            return newData;
        }
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags = 0;
        vector<ActionRowData> components;
    };

    enum class InteractionCallbackType {
        Pong = 1,
        Acknowledge = 2,
        ChannelMessage = 3,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5,
        DeferredUpdateMessage = 6,
        UpdateMessage = 7
    };

    struct MessageReferenceData {
        operator DiscordCoreInternal::MessageReferenceData() {
            DiscordCoreInternal::MessageReferenceData newData;
            newData.channelId = this->channelId;
            newData.failIfNotExists = this->failIfNotExists;
            newData.guildId = this->guildId;
            newData.messageId = this->messageId;
            return newData;
        }
        string messageId;
        string channelId;
        string guildId;
        bool failIfNotExists;
    };

    struct InputEventResponseData {
        operator DiscordCoreInternal::InputEventResponseData() {
            DiscordCoreInternal::InputEventResponseData newData((DiscordCoreInternal::InputEventResponseType)this->inputEventResponseType);
            newData.allowedMentions = this->allowedMentions;
            newData.applicationId = this->applicationId;
            newData.channelId = this->channelId;
            for (auto value : this->components) {
                DiscordCoreInternal::ActionRowData actionRow;
                actionRow = value;
                newData.components.push_back(actionRow);
            }
            newData.content = this->content;
            for (auto value : this->embeds) {
                DiscordCoreInternal::EmbedData embedData;
                embedData = value;
                newData.embeds.push_back(embedData);
            }
            newData.eventResponseType = (DiscordCoreInternal::InputEventResponseType)this->inputEventResponseType;
            newData.finalContent = this->finalContent;
            newData.flags = this->flags;
            newData.interactionId = this->interactionId;
            newData.interactionToken = this->interactionToken;
            newData.messageId = this->messageId;
            newData.messageReference = this->messageReference;
            newData.requesterId = this->requesterId;
            newData.tts = this->tts;
            newData.type = (DiscordCoreInternal::InteractionCallbackType)this->type;
            return newData;
        }
        InputEventResponseData(InputEventResponseType responseType) {
            if (responseType == InputEventResponseType::UNSET) {
                throw exception("You forgot to set the event response type!");
            }
            else {
                this->inputEventResponseType = responseType;
            }
        }
        InputEventResponseData() = delete;
        InputEventResponseType inputEventResponseType;
        InteractionCallbackType type;
        string channelId;
        string messageId;
        string interactionToken;
        string applicationId;
        string interactionId;
        string finalContent;
        string requesterId;
        MessageReferenceData messageReference;
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags = 0;
        vector<ActionRowData> components;
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct OverWriteData {
        operator DiscordCoreInternal::OverWriteData() {
            DiscordCoreInternal::OverWriteData newData;
            newData.allow = this->allow;
            newData.deny = this->deny;
            newData.id = this->id;
            newData.type = this->type;
            return newData;
        }
        string id;
        int type;	           //	either 0 (role) or 1 (member)
        string allow;
        string deny;
    };

    enum class ChannelType {
        GUILD_TEXT = 0,
        DM = 1,
        GUILD_VOICE = 2,
        GROUP_DM = 3,
        GUILD_CATEGORY = 4,
        GUILD_NEWS = 5,
        GUILD_STORE = 6,
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
        GUILD_STAGE_VOICE = 13
    };

    struct ChannelData {
        operator DiscordCoreInternal::ChannelData() {
            DiscordCoreInternal::ChannelData newData;
            newData.applicationId = this->applicationId;
            newData.bitrate = this->bitrate;
            newData.guildId = this->guildId;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.lastMessageId = this->lastMessageId;
            newData.lastPinTimestamp = this->lastPinTimestamp;
            newData.name = this->name;
            newData.nsfw = this->nsfw;
            newData.ownerId = this->ownerId;
            newData.parentId = this->parentId;
            for (auto [key, value] : this->permissionOverwrites) {
                newData.permissionOverwrites.insert(make_pair(key, value));
            }
            newData.position = this->position;
            newData.rateLimitPerUser = this->rateLimitPerUser;
            for (auto value : this->recipients) {
                newData.recipients.push_back(value);
            }
            newData.rtcRegion = this->rtcRegion;
            newData.topic = this->topic;
            newData.type = (DiscordCoreInternal::ChannelType)this->type;
            newData.userLimit = this->userLimit;
            newData.videoQualityMode = this->videoQualityMode;
            return newData;
        }
        string id;
        ChannelType type = ChannelType::DM;
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

    struct GuildMemberData {
        operator DiscordCoreInternal::GuildMemberData() {
            DiscordCoreInternal::GuildMemberData newData;
            newData.deaf = this->deaf;
            newData.guildId = this->guildId;
            newData.joined_at = this->joined_at;
            newData.mute = this->mute;
            newData.nick = this->nick;
            newData.pending = this->pending;
            newData.permissions = this->permissions;
            newData.premium_since = this->premium_since;
            newData.roles = this->roles;
            newData.user = this->user;
            return newData;
        }
        string guildId;
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

    struct ApplicationCommandInteractionDataResolved {
        operator DiscordCoreInternal::ApplicationCommandInteractionDataResolved() {
            DiscordCoreInternal::ApplicationCommandInteractionDataResolved newData;
            for (auto [key, value] : this->channels) {
                newData.channels.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->members) {
                newData.members.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->roles) {
                newData.roles.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->users) {
                newData.users.insert(make_pair(key, value));
            }
            return newData;
        }
        map<string, UserData> users{};
        map<string, GuildMemberData> members{};
        map<string, RoleData> roles{};
        map<string, ChannelData> channels{};
    };

    enum class ApplicationCommandOptionType {
        SUB_COMMAND = 1,
        SUB_COMMAND_GROUP = 2,
        STRING = 3,
        INTEGER = 4,
        BOOLEAN = 5,
        USER = 6,
        CHANNEL = 7,
        ROLE = 8,
        MENTIONABLE = 9
    };

    struct ApplicationCommandInteractionDataOption;

    vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    struct ApplicationCommandInteractionDataOption {
        operator DiscordCoreInternal::ApplicationCommandInteractionDataOption() {
            DiscordCoreInternal::ApplicationCommandInteractionDataOption newData;
            newData.name = this->name;
            newData.options = convertAppCommandInteractionDataOptions(this->options);
            newData.type = (DiscordCoreInternal::ApplicationCommandOptionType)this->type;
            newData.valueBool = this->valueBool;
            newData.valueInt = this->valueInt;
            newData.valueString = this->valueString;
            return newData;
        }
        string name;
        ApplicationCommandOptionType type;
        bool valueBool;
        string valueString;
        int valueInt;
        vector<ApplicationCommandInteractionDataOption> options;
    };

    vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions) {
        vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> newVector;
        for (auto value : originalOptions) {
            DiscordCoreInternal::ApplicationCommandInteractionDataOption newItem;
            newItem.name = value.name;
            newItem.type = (DiscordCoreInternal::ApplicationCommandOptionType)value.type;
            newItem.valueBool = value.valueBool;
            newItem.valueInt = value.valueInt;
            newItem.valueString = value.valueString;
            newItem.options = convertAppCommandInteractionDataOptions(value.options);
            newVector.push_back(newItem);
        }
        return newVector;
    }

    struct ApplicationCommandInteractionData {
        operator DiscordCoreInternal::ApplicationCommandInteractionData() {
            DiscordCoreInternal::ApplicationCommandInteractionData newData;
            newData.componentType = this->componentType;
            newData.customId = this->customId;
            newData.id = this->id;
            newData.name = this->name;
            for (auto value : this->options) {
                DiscordCoreInternal::ApplicationCommandInteractionDataOption newOption;
                newOption = value;
                newData.options.push_back(newOption);
            }
            newData.resolved = this->resolved;
            return newData;
        }
        string name;
        string id;
        ApplicationCommandInteractionDataResolved resolved;
        vector<ApplicationCommandInteractionDataOption> options;
        string customId;
        int componentType;
    };

    struct ChannelMentionData {
        operator DiscordCoreInternal::ChannelMentionData() {
            DiscordCoreInternal::ChannelMentionData newData;
            newData.guildId = this->guildId;
            newData.id = this->id;
            newData.name = this->name;
            newData.type = this->type;
            return newData;
        }
        string id;
        string guildId;
        int type;
        string name;
    };

    struct ReactionData {
        operator DiscordCoreInternal::ReactionData() {
            DiscordCoreInternal::ReactionData newData;
            newData.channelId = this->channelId;
            newData.count = this->count;
            newData.emoji = this->emoji;
            newData.guildId = this->guildId;
            newData.me = this->me;
            newData.member = this->member;
            newData.messageId = this->messageId;
            newData.userId = this->userId;
            return newData;
        }
        int count;
        bool me;
        EmojiData emoji;
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
    };

    enum class MessageType {
        DEFAULT = 0,
        RECIPIENT_ADD = 1,
        RECIPIENT_REMOVE = 2,
        CALL = 3,
        CHANNEL_NAME_CHANGE = 4,
        CHANNEL_ICON_CHANGE = 5,
        CHANNEL_PINNED_MESSAGE = 6,
        GUILD_MEMBER_JOIN = 7,
        USER_PREMIUM_GUILD_SUBSCRIPTION = 8,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
        CHANNEL_FOLLOW_ADD = 12,
        GUILD_DISCOVERY_DISQUALIFIED = 14,
        GUILD_DISCOVERY_REQUALIFIED = 15,
        GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING = 16,
        GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING = 17,
        THREAD_CREATED = 18,
        REPLY = 19,
        APPLICATION_COMMAND = 20,
        THREAD_STARTER_MESSAGE = 21,
        GUILD_INVITE_REMINDER = 22
    };

    struct MessageActivityData {
        int type;
        string partyId;
    };

    struct MessageInteractionData {
        operator DiscordCoreInternal::MessageInteractionData() {
            DiscordCoreInternal::MessageInteractionData newData;
            newData.id = this->id;
            newData.name = this->name;
            newData.type = (DiscordCoreInternal::InteractionType)this->type;
            newData.user = this->user;
            return newData;
        }
        string id;
        InteractionType type;
        string name;
        UserData user;
    };

    struct MessageStickerData {
        operator DiscordCoreInternal::MessageStickerData() {
            DiscordCoreInternal::MessageStickerData newData;
            newData.asset = this->asset;
            newData.description = this->description;
            newData.formatType = this->formatType;
            newData.id = this->id;
            newData.name = this->name;
            newData.packId = this->packId;
            newData.tags = this->tags;
            return newData;
        }
        string id;
        string packId;
        string name;
        string description;
        string tags;
        string asset;
        int formatType;
    };

    struct TeamMembersObjectData {
        operator DiscordCoreInternal::TeamMembersObjectData() {
            DiscordCoreInternal::TeamMembersObjectData newData;
            newData.membershipState = this->membershipState;
            newData.permissions = this->permissions;
            newData.teamId = this->teamId;
            newData.user = this->user;
            return newData;
        }
        int membershipState;
        vector<string> permissions;
        string teamId;
        UserData user;
    };

    struct TeamObjectData {
        operator DiscordCoreInternal::TeamObjectData() {
            DiscordCoreInternal::TeamObjectData newData;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.ownerUserId = this->ownerUserId;
            return newData;
        }
        string icon;
        string id;
        vector<TeamMembersObjectData> members;
        string ownerUserId;
    };

    struct ApplicationData {
        operator DiscordCoreInternal::ApplicationData() {
            DiscordCoreInternal::ApplicationData newData;
            newData.botPublic = this->botPublic;
            newData.botRequireCodeGrant = this->botRequireCodeGrant;
            newData.coverImage = this->coverImage;
            newData.description = this->description;
            newData.flags = this->flags;
            newData.guildId = this->guildId;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.name = this->name;
            newData.owner = this->owner;
            newData.primarySkuId = this->primarySkuId;
            newData.privacyPolicyUrl = this->privacyPolicyUrl;
            newData.rpcOrigins = this->rpcOrigins;
            newData.slug = this->slug;
            newData.summary = this->summary;
            newData.team = this->team;
            newData.termsOfServiceUrl = this->termsOfServiceUrl;
            newData.verifyKey = this->verifyKey;
            return newData;
        }
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
        int flags = 0;
    };

    struct MessageDataOld {
        operator DiscordCoreInternal::MessageDataOld() {
            DiscordCoreInternal::MessageDataOld newData;
            newData.activity.partyId = this->activity.partyId;
            newData.activity.type = this->activity.type;
            newData.application = this->application;
            for (auto value : this->attachments) {
                newData.attachments.push_back(value);
            }
            newData.author = this->author;
            newData.channelId = this->channelId;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            newData.content = this->content;
            newData.editedTimestamp = this->editedTimestamp;
            for (auto value : this->embeds) {
                newData.embeds.push_back(value);
            }
            newData.flags = this->flags;
            newData.guildId = this->guildId;
            newData.id = this->id;
            newData.interaction = this->interaction;
            newData.member = this->member;
            for (auto value : this->mentionChannels) {
                newData.mentionChannels.push_back(value);
            }
            newData.mentionEveryone = this->mentionEveryone;
            newData.mentionRoles = this->mentionRoles;
            for (auto value : this->mentions) {
                newData.mentions.push_back(value);
            }
            newData.messageReference = this->messageReference;
            newData.nonce = this->nonce;
            newData.pinned = this->pinned;
            for (auto value : this->reactions) {
                newData.reactions.push_back(value);
            }
            for (auto value : this->stickers) {
                newData.stickers.push_back(value);
            }
            newData.timestamp = this->timestamp;
            newData.tts = this->tts;
            newData.type = (DiscordCoreInternal::MessageType)this->type;
            newData.webhookId = this->webhookId;
            return newData;
        }
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
        MessageType type;
        MessageActivityData activity;
        ApplicationData application;
        MessageReferenceData messageReference;
        int flags = 0;
        vector<MessageStickerData> stickers;
        MessageInteractionData interaction;
        vector<ActionRowData> components;
    };

    struct MessageData : MessageDataOld {
        operator DiscordCoreInternal::MessageData() {
            DiscordCoreInternal::MessageData newData;
            newData.referencedMessage = this->referencedMessage;
            return newData;
        }
        MessageDataOld referencedMessage;
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

    struct InteractionData {
        operator DiscordCoreInternal::InteractionData() {
            DiscordCoreInternal::InteractionData newData;
            newData.applicationId = this->applicationId;
            newData.channelId = this->channelId;
            newData.customId = this->customId;
            newData.data = this->data;
            return newData;
        }
        string id;
        string applicationId;
        InteractionType type;
        ApplicationCommandInteractionData data;
        json dataRaw;
        string guildId;
        string channelId;
        GuildMemberData member;
        UserData user;
        string token;
        int version;
        MessageData message;
        string customId;
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
        int flags = 0;
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

    enum class PremiumTier {
        NONE = 0,
        TIER_2 = 2,
        TIER_1 = 1,
        TIER_3 = 3
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

    struct ReactionAddEventData {
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
        EmojiData emoji;
    };

    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data;
        InteractionCallbackType type;
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3
    };

    struct InputEventData {
        InputEventData() {}
        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType) {
            this->messageData = messageData;
            this->interactionData = interactionData;
            this->eventType = eventType;
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
        }
        DiscordCoreAPI::DiscordCoreClient* discordCoreClient;
        InteractionData interactionData;
        MessageData messageData;
        InputEventType eventType;
        string requesterId;
        string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else {
                return this->interactionData.applicationId;
            }
        }
        string getInteractionToken() {
            if (this->interactionData.token == "") {
                return this->interactionData.token;
            }
            else {
                return this->interactionData.token;
            }
        }
        string getInteractionId() {
            if (this->interactionData.id == "") {
                return this->messageData.interaction.id;
            }
            else {
                return this->interactionData.id;
            }
        }
        string getUserName() {
            if (this->messageData.author.username == "") {
                return this->interactionData.member.user.username;
            }
            else {
                return this->messageData.author.username;
            }
        }
        string getAvatarURL() {
            if (this->messageData.author.getAvatarURL() == "") {

                return this->interactionData.member.user.getAvatarURL();
            }
            else {
                return this->messageData.author.getAvatarURL();
            }
        }
        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else {
                return this->interactionData.channelId;
            }
        };
        string getMessageId() {
            if (this->messageData.id == "") {
                return this->interactionData.message.id;
            }
            else {
                return this->messageData.id;
            }
        }
        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else {
                return this->messageData.author.id;
            }
        }
        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else {
                return this->messageData.guildId;
            }
        }
    };

};

#endif
