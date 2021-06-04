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

    struct Button;

    string constructStringContent(DiscordCoreInternal::CommandData commandData) {
        string finalCommandString;
        finalCommandString = commandData.commandName + " = ";
        for (auto& value : commandData.optionsArgs) {
            finalCommandString += value + ", ";
        }
        return finalCommandString;
    };    

    struct InteractionResponseData {
        DiscordCoreInternal::InteractionCallbackType type;
        DiscordCoreInternal::InteractionApplicationCommandCallbackData data;
        string channelId;
        string applicationId;
        string interactionId;
        string guildId;
        string userId;
        string token;
    };

    struct InteractionData {
        string id;
        string applicationId;
        DiscordCoreInternal::InteractionType type;
        DiscordCoreInternal::ApplicationCommandInteractionData data;
        string guildId;
        string channelId;
        DiscordCoreInternal::GuildMemberData member;
        DiscordCoreInternal::UserData user;
        string token;
        int version;
        DiscordCoreInternal::MessageData message;
        string customId;
    };

	class InteractionManager {
    public:
        static unbounded_buffer<InteractionData>* inputInteractionBuffer;
        static map<string, Button> activeButtons;
        static bool areWeRunning;

		InteractionManager() {

		}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threads = threadsNew;
            InteractionManager::inputInteractionBuffer = new unbounded_buffer<InteractionData>;
        }

        static task<void> createInteractionResponseAsync(InteractionResponseData interactionData) {
            DiscordCoreInternal::InteractionResponseData interactionDataNew;
            interactionDataNew.applicationId = interactionData.applicationId;
            interactionDataNew.channelId = interactionData.channelId;
            interactionDataNew.data = interactionData.data;
            interactionDataNew.guildId = interactionData.guildId;
            interactionDataNew.interactionId = interactionData.interactionId;
            interactionDataNew.token = interactionData.token;
            interactionDataNew.type = interactionData.type;
            interactionDataNew.userId = interactionData.userId;
            interactionDataNew.type = interactionData.type;
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + interactionData.interactionId + "/" + interactionData.token + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getCreateInteractionPayload(interactionDataNew);
            DiscordCoreInternal::HttpRequestAgent requestAgent(InteractionManager::agentResources, InteractionManager::threads->at(5).scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManager::createInteractionResponseAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_return;
        }

        static task<DiscordCoreInternal::MessageData> editInteractionResponseAsync(InteractionResponseData editInteractionData) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + editInteractionData.applicationId + "/" + editInteractionData.token + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            DiscordCoreInternal::InteractionResponseData editInteractionResponseDataNew;
            editInteractionResponseDataNew.data.allowedMentions = editInteractionData.data.allowedMentions;
            editInteractionResponseDataNew.applicationId = editInteractionData.applicationId;
            editInteractionResponseDataNew.originalAuthorId = editInteractionData.userId;
            for (auto& value : editInteractionData.data.components) {
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
                editInteractionResponseDataNew.data.components.push_back(actionRow);
            }
            editInteractionResponseDataNew.data.content = editInteractionData.data.content;
            editInteractionResponseDataNew.token = editInteractionData.token;
            if (editInteractionData.data.embeds.size() > 0) {
                DiscordCoreInternal::EmbedData embedData;
                embedData.actualColorVal = editInteractionData.data.embeds.at(0).actualColorVal;
                embedData.author = editInteractionData.data.embeds.at(0).author;
                embedData.color[0] = editInteractionData.data.embeds.at(0).color[0];
                embedData.color[1] = editInteractionData.data.embeds.at(0).color[1];
                embedData.color[2] = editInteractionData.data.embeds.at(0).color[2];
                embedData.description = editInteractionData.data.embeds.at(0).description;
                embedData.fields = editInteractionData.data.embeds.at(0).fields;
                embedData.footer = editInteractionData.data.embeds.at(0).footer;
                embedData.image = editInteractionData.data.embeds.at(0).image;
                embedData.provider = editInteractionData.data.embeds.at(0).provider;
                embedData.thumbnail = editInteractionData.data.embeds.at(0).thumbnail;
                embedData.timestamp = editInteractionData.data.embeds.at(0).timestamp;
                embedData.title = editInteractionData.data.embeds.at(0).title;
                embedData.type = editInteractionData.data.embeds.at(0).type;
                embedData.url = editInteractionData.data.embeds.at(0).url;
                embedData.video = editInteractionData.data.embeds.at(0).video;
                editInteractionResponseDataNew.data.embeds.push_back(embedData);
            }
            editInteractionResponseDataNew.userId = editInteractionData.userId;
            editInteractionResponseDataNew.channelId = editInteractionData.channelId;
            editInteractionResponseDataNew.guildId = editInteractionData.guildId;
            editInteractionResponseDataNew.interactionId = editInteractionData.interactionId;
            editInteractionResponseDataNew.type = DiscordCoreInternal::InteractionCallbackType::UpdateMessage;
            workload.content = DiscordCoreInternal::getCreateInteractionPayload(editInteractionResponseDataNew);
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
            parseObject(returnData.data, &messageData);
            messageData.messageType = DiscordCoreInternal::MessageTypeReal::INTERACTION;
            messageData.author.id = editInteractionResponseDataNew.originalAuthorId;
            co_return messageData;
        }
        
	protected:
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

    };
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;
    unbounded_buffer<InteractionData>* InteractionManager::inputInteractionBuffer;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;
    map<string, Button> InteractionManager::activeButtons;
    bool InteractionManager::areWeRunning;

    struct Button {
    public:
        string buttonId;
        string channelId;
        string messageId;
        string userId;

        Button(DiscordCoreInternal::MessageData messageData, unsigned long long maxTimeInMsNew) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->channelId = messageData.channelId;
            this->messageId = messageData.id;
            this->userId = messageData.author.id;
            this->maxTimeInMs = maxTimeInMsNew;
            InteractionManager::activeButtons.insert(make_pair(this->channelId + this->messageId + this->userId, *this));
        }
    protected:
        unsigned long long startTime;
        unsigned long long maxTimeInMs;
    };

    bool checkIfButtonIsActive(string channelId, string messageId, string userId) {
        if (InteractionManager::activeButtons.contains(channelId + messageId + userId)) {
            InteractionManager::activeButtons.erase(channelId + messageId + userId);
            return true;
        }
        else {
            return false;
        }
    }

};
#endif
