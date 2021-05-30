// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_ENTITIES_
#define _FOUNDATION_ENTITIES_

#include "pch.h"

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
        EmbedData setColor(int colorRed, int colorGreen, int colorBlue) {
            this->color[0] = colorRed;
            this->color[1] = colorGreen;
            this->color[2] = colorBlue;
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
        int color[3] = { 255, 255, 255 };
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
        int actualColorVal;
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
            string avatarURL;
            avatarURL = "https://cdn.discordapp.com/avatars/" + this->id + "/" + this->avatar;
            return avatarURL;
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

    enum class MessageTypeReal {
        INTERACTION = 1,
        REGULAR = 2
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
        POST_ROLE = 28
    };

    struct HttpAgentResources {
        hstring botToken;
        hstring baseURL;
    };

    struct GetApplicationData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
    };

    struct InteractionResponseData {
        ThreadContext threadContext;
        HttpAgentResources agentResources;
        InteractionCallbackType type;
        InteractionApplicationCommandCallbackData data;
        string channelId;
        string applicationId;
        string token;
        string messageId;
        string interactionId;
        string guildId;
        string userId;
        string content;
    };

    struct HttpData {
        unsigned int returnCode = 0;
        string returnMessage = "";
        json data;
    };

    struct HttpWorkload {
        HttpWorkloadClass workloadClass;
        HttpWorkloadType workloadType;
        string relativePath;
        string content;
    };

    struct MessageDataOld {
        MessageTypeReal messageType = MessageTypeReal::REGULAR;
        string id;
        string channelId;
        string guildId;
        UserData author;
        GuildMemberData member;
        string content;
        string timestamp;
        string editedTimestamp;
        string applicationId;
        string interactionId;
        string interactionToken;
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
        InteractionResponseData interaction;
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
        HttpAgentResources agentResources;
        ThreadContext threadContext;
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
    };

    struct GetRoleData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string guildId;
        string roleId;
    };

    struct FetchRoleData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string guildId;
        string roleId;
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
        string content;
    };

    struct PatchMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string content;
        string channelId;
        string messageId;
    };

    struct DeleteMessageData {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        unsigned int timeDelay = 0;
        string channelId;
        string messageId;
    };

    struct EditMessageData {
        string content;
        DiscordCoreInternal::EmbedData embed;
        int flags = 0;
        vector<DiscordCoreInternal::AttachmentData> attachments;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
    };

    struct CreateMessageData {
        string content;
        int nonce;
        bool tts = false;
        DiscordCoreInternal::EmbedData embed;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        DiscordCoreInternal::MessageReferenceData messageReference;
        vector<ActionRowData> components;
    };

    struct ReplyMessageData {
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        DiscordCoreInternal::MessageReferenceData messageReference;
        string content;
        string channelId;
        DiscordCoreInternal::EmbedData embed;
        int nonce;
        bool tts = false;
        vector<ActionRowData> components;
    };

    struct SendDMData {
        DiscordCoreInternal::HttpAgentResources agentResources;
        DiscordCoreInternal::ThreadContext threadContext;
        string userId;
        string channelId;
        string content;
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

    struct UpdateRoleDataInternal {
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string content;
        string guildId;
        string roleId;
    };

    struct PostRoleData {
        string content;
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string guildId;
    };

    struct UpdateRoleData {
        string name;
        string permissions;
        int colorFirst[3];
        int color;
        bool hoist;
        bool mentionable;
        void actualColor() {
            if (this->colorFirst[0] > 255) {
                this->colorFirst[0] = 255;
            }
            else if (this->colorFirst[0] < 0) {
                this->colorFirst[0] = 0;
            }
            if (this->colorFirst[1] > 255) {
                this->colorFirst[1] = 255;
            }
            else if (this->colorFirst[1] < 0) {
                this->colorFirst[1] = 0;
            }
            if (this->colorFirst[2] > 255) {
                this->colorFirst[2] = 255;
            }
            else if (this->colorFirst[2] < 0) {
                this->colorFirst[2] = 0;
            }
            int colorValue = 65536 * this->colorFirst[0] + 256 * this->colorFirst[1] + this->colorFirst[2];
            this->color = colorValue;
        };
    };

    struct CreateRoleData {
        string name;
        string permissions;
        string color;
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

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct CommandData {
        vector<string> optionsArgs;
        string commandName;
    };

    struct ApplicationCommandInteractionDataResolved {
        map<string, UserData> users;
        map<string, GuildMemberData> members;
        map<string, RoleData> roles;
        map<string, ChannelData> channels;
    };

    struct ApplicationCommandInteractionDataOption {
        string name;
        int type;
        ApplicationCommandOptionType value;
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
        InteractionResponseData interactionResponseData;
    };
    /*
    struct PatchInteractionResponseData {
        string applicationId;
        string interactionToken;
        string content = "";
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        HttpAgentResources agentResources;
        ThreadContext threadContext;
        string channelId;
        string messageId;
    };
    */
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
}
#endif