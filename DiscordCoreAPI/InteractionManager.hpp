// InteractionManager.hpp - Header for the interaction manager class.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INTERACTION_MANAGER_
#define _INTERACTION_MANAGER_

#include "pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    class DiscordCoreClient;

	string constructStringContent(DiscordCoreInternal::InteractionIncData interactionIncData) {
        string finalCommandString;
        finalCommandString = interactionIncData.commandName + " = ";
		for (auto& value : interactionIncData.optionsArgs) {
            finalCommandString += value + ", ";
		}
        return finalCommandString;
	}

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
        EmbedData setColor(unsigned int colorRed, unsigned int colorGreen, unsigned int colorBlue) {
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
            DiscordCoreInternal::EmbedFieldData embedFieldData;
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
        unsigned int color[3] = { 255, 255, 255 };
        DiscordCoreInternal::EmbedFooterData footer;
        DiscordCoreInternal::EmbedImageData image;
        DiscordCoreInternal::EmbedThumbnailData thumbnail;
        DiscordCoreInternal::EmbedVideoData video;
        DiscordCoreInternal::EmbedProviderData provider;
        DiscordCoreInternal::EmbedAuthorData author;
        vector<DiscordCoreInternal::EmbedFieldData> fields;
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
        string label;
        DiscordCoreInternal::EmojiData emoji;
        string customId;
        string url;
        bool disabled;
    };

    struct ActionRowData {
        vector<ComponentData> components;
    };

    struct ButtonResponse {
        vector<string> responseValues;
        string buttonId;
    };

    struct ButtonRequest {
        vector<string> buttonIds;
        string channelId;
        string userId;
    };

    struct EditInteractionResponseData {
        string applicationId;
        string interactionToken;
        string content = "";
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
    };

	class InteractionManager {
    public:
        static unbounded_buffer<ButtonResponse> buttonResponseBuffer;
        static unbounded_buffer<ButtonRequest> buttonRequestBuffer;
        static unbounded_buffer<DiscordCoreInternal::MessageData> requestInteractionBuffer;
        static DiscordCoreClient* pDiscordCoreClient;
        static bool areWeRunning;

		InteractionManager() {

		}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, Scheduler* pSchedulerNew, DiscordCoreClient* pDiscordCoreClientNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threads = threadsNew;
            InteractionManager::pDiscordCoreClient = pDiscordCoreClientNew;
        }
        
        static task<void> createInteractionResponseDeferralAsync(DiscordCoreInternal::InteractionResponseFullData interactionData) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + interactionData.interactionIncData.interactionId + "/" + interactionData.interactionIncData.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getInteractionResponsePayload(interactionData);
            DiscordCoreInternal::HttpRequestAgent requestAgent(InteractionManager::agentResources, InteractionManager::threads->at(5).scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseDeferralAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManager::createInteractionResponseDeferralAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManager::createInteractionResponseDeferralAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_return;
        }

        static task<DiscordCoreInternal::MessageData> editInteractionResponse(EditInteractionResponseData editInteractionData) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + editInteractionData.applicationId + "/" + editInteractionData.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            DiscordCoreInternal::EditInteractionResponseData editInteractionResponseDataNew;
            editInteractionResponseDataNew.allowedMentions = editInteractionData.allowedMentions;
            editInteractionResponseDataNew.applicationId = editInteractionData.applicationId;
            for (auto& value : editInteractionData.components) {
                DiscordCoreInternal::ActionRowData actionRow;
                for (auto& value2 : value.components) {
                    DiscordCoreInternal::ComponentData componentData;
                    componentData.customId = value2.customId;
                    componentData.disabled = value2.disabled;
                    componentData.emoji = value2.emoji;
                    componentData.label = value2.label;
                    componentData.style = (DiscordCoreInternal::ButtonStyle)value2.style;
                    componentData.type = (DiscordCoreInternal::ComponentType)value2.type;
                    componentData.url = value2.url;
                    actionRow.components.push_back(componentData);
                }
                editInteractionResponseDataNew.components.push_back(actionRow);
            }
            editInteractionResponseDataNew.content = editInteractionData.content;
            editInteractionResponseDataNew.interactionToken = editInteractionData.interactionToken;
            if (editInteractionData.embeds.size() > 0) {
                DiscordCoreInternal::EmbedData embedData;
                embedData.actualColorVal = editInteractionData.embeds.at(0).actualColorVal;
                embedData.author = editInteractionData.embeds.at(0).author;
                embedData.color[0] = editInteractionData.embeds.at(0).color[0];
                embedData.color[1] = editInteractionData.embeds.at(0).color[1];
                embedData.color[2] = editInteractionData.embeds.at(0).color[2];
                embedData.description = editInteractionData.embeds.at(0).description;
                embedData.fields = editInteractionData.embeds.at(0).fields;
                embedData.footer = editInteractionData.embeds.at(0).footer;
                embedData.image = editInteractionData.embeds.at(0).image;
                embedData.provider = editInteractionData.embeds.at(0).provider;
                embedData.thumbnail = editInteractionData.embeds.at(0).thumbnail;
                embedData.timestamp = editInteractionData.embeds.at(0).timestamp;
                embedData.title = editInteractionData.embeds.at(0).title;
                embedData.type = editInteractionData.embeds.at(0).type;
                embedData.url = editInteractionData.embeds.at(0).url;
                embedData.video = editInteractionData.embeds.at(0).video;
                editInteractionResponseDataNew.embeds.push_back(embedData);
            }
            workload.content = DiscordCoreInternal::getEditInteractionResponsePayload(editInteractionResponseDataNew);
            DiscordCoreInternal::HttpRequestAgent requestAgent(InteractionManager::agentResources, InteractionManager::threads->at(5).scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponse() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManager::editInteractionResponse() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManager::editInteractionResponse() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::MessageData messageData;
            while (!try_receive(InteractionManager::requestInteractionBuffer, messageData)) {
                try_receive(InteractionManager::requestInteractionBuffer, messageData);
            };
            co_return messageData;
        }
        
	protected:
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

	};
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;
    unbounded_buffer<ButtonResponse> InteractionManager::buttonResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::MessageData> InteractionManager::requestInteractionBuffer;
    unbounded_buffer<ButtonRequest> InteractionManager::buttonRequestBuffer;
    bool InteractionManager::areWeRunning;
    DiscordCoreClient* InteractionManager::pDiscordCoreClient;
};
#endif
