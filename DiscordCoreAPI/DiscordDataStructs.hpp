// DiscordDataStructs.hpp - Header for all of the Discord API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_DATA_STRUCTS_
#define _DISCORD_DATA_STRUCTS_

#include "pch.h"

namespace DiscordCoreInternal {

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
        string content;
        int nonce;
        bool tts = false;
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

}
#endif