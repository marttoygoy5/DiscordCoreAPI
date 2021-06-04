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

    class InteractionManager;

    struct Button;

    string constructStringContent(DiscordCoreInternal::CommandData commandData) {
        string finalCommandString;
        finalCommandString = commandData.commandName + " = ";
        for (auto& value : commandData.optionsArgs) {
            finalCommandString += value + ", ";
        }
        return finalCommandString;
    };    

    enum class InteractionResponseType {
        CreateInteractionResponse = 1,
        EditInteractionResponse = 2
    };

    struct InteractionResponseData {
        InteractionResponseType responseType;
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

    struct ButtonInteractionData {
        string token;
        string id;
        string applicationId;
        DiscordCoreInternal::InteractionType type;
        string customId;
        DiscordCoreInternal::GuildMemberData member;
        DiscordCoreInternal::UserData user;
        DiscordCoreInternal::MessageData message;
        string channelId;
        string guildId;
    };

    class InteractionManagerAgent : public agent {
    public:
        static unbounded_buffer<DiscordCoreInternal::InteractionResponseData>* requestPatchBuffer;
        static unbounded_buffer<DiscordCoreInternal::InteractionResponseData>* requestPostBuffer;
        static unbounded_buffer<DiscordCoreInternal::MessageData>* outBuffer;

        static unbounded_buffer<exception>* errorBuffer;
        InteractionManagerAgent(Scheduler* pSchedulerNew)
            :agent(*pSchedulerNew) {}

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
        }

        static void initialize() {
            InteractionManagerAgent::outBuffer = new unbounded_buffer<DiscordCoreInternal::MessageData>;
            InteractionManagerAgent::errorBuffer = new unbounded_buffer<exception>;
            InteractionManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::InteractionResponseData>;
            InteractionManagerAgent::requestPatchBuffer = new unbounded_buffer <DiscordCoreInternal::InteractionResponseData>;
        }

        task<DiscordCoreInternal::MessageData> patchObjectDataAsync(DiscordCoreInternal::InteractionResponseData dataPackage){
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.token + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getCreateInteractionPayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::patchObjectDataAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectDataAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectDataAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::MessageData messageData;
            parseObject(returnData.data, &messageData);
            messageData.messageType = DiscordCoreInternal::MessageTypeReal::INTERACTION;
            messageData.author.id = dataPackage.originalAuthorId;
            co_return messageData;
        }

        task<void> postObjectDataAsync(DiscordCoreInternal::InteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.token + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getCreateInteractionPayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::postObjectDataAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectDataAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectDataAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_return;
        }

    protected:

        void run() {
            try {
                DiscordCoreInternal::InteractionResponseData dataPackage;
                if (try_receive(InteractionManagerAgent::requestPatchBuffer, dataPackage)) {
                    DiscordCoreInternal::MessageData messageData = patchObjectDataAsync(dataPackage).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
                }
                DiscordCoreInternal::InteractionResponseData dataPackageNew;
                if (try_receive(InteractionManagerAgent::requestPostBuffer, dataPackageNew)) {
                    postObjectDataAsync(dataPackageNew).get();
                }
            }
            catch (const exception& e) {
                send(errorBuffer, e);
            }
            done();
        }

    };
	class InteractionManager {
    public:
        friend class InteractionManagerAgent;
        static unbounded_buffer<InteractionData>* inputInteractionBuffer;
        static unbounded_buffer<ButtonInteractionData>* buttonInteractionBuffer;
        static map<string, Button> activeButtons;

		InteractionManager() {

		}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threads = threadsNew;
            InteractionManager::inputInteractionBuffer = new unbounded_buffer<InteractionData>;
            InteractionManager::buttonInteractionBuffer = new unbounded_buffer<ButtonInteractionData>;
        }

        static task<void> createInteractionResponseAsync(InteractionResponseData interactionData) {
            apartment_context mainThread;
            co_await resume_background();
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
            interactionDataNew.agentResources = InteractionManager::agentResources;
            interactionDataNew.threadContext = InteractionManager::threads->at(6);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostBuffer, interactionDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            co_await mainThread;
            co_return;
        }

        static task<DiscordCoreInternal::MessageData> editInteractionResponseAsync(InteractionResponseData editInteractionData) {
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
            editInteractionResponseDataNew.agentResources = InteractionManager::agentResources;
            editInteractionResponseDataNew.threadContext = InteractionManager::threads->at(6);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPatchBuffer, editInteractionResponseDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

	protected:
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

    };
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;
    unbounded_buffer<InteractionData>* InteractionManager::inputInteractionBuffer;
    unbounded_buffer<DiscordCoreInternal::InteractionResponseData>* InteractionManagerAgent::requestPostBuffer;
    unbounded_buffer<DiscordCoreInternal::InteractionResponseData>* InteractionManagerAgent::requestPatchBuffer;
    unbounded_buffer<exception>* InteractionManagerAgent::errorBuffer;
    unbounded_buffer<ButtonInteractionData>* InteractionManager::buttonInteractionBuffer;
    unbounded_buffer<DiscordCoreInternal::MessageData>* InteractionManagerAgent::outBuffer;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;
    map<string, Button> InteractionManager::activeButtons;

    struct Button {
    public:

        Button(DiscordCoreInternal::MessageData messageData) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->channelId = messageData.channelId;
            this->messageId = messageData.id;
            this->userId = messageData.requesterId;
            InteractionManager::activeButtons.insert(make_pair(this->channelId + this->messageId + this->userId, *this));
        }

        string getButtonId() {
            if (buttonId == "") {
                return "";
            }
            else {
                return this->buttonId;
            }
        }

        ButtonInteractionData getOurButtonData(unsigned long long maxWaitTimeInMs) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            ButtonInteractionData buttonInteractionData;
            while (doWeQuit == false) {
                if (checkIfTimeHasPassed(maxWaitTimeInMs)) {
                    doWeQuit = true;
                }
                else {
                    if (try_receive(InteractionManager::buttonInteractionBuffer, buttonInteractionData)) {
                        if ((buttonInteractionData.channelId != this->channelId) || (buttonInteractionData.message.id != this->messageId)|| this->userId != buttonInteractionData.user.id) {
                            send(InteractionManager::buttonInteractionBuffer, buttonInteractionData);
                        }
                        else {
                            this->buttonId = buttonInteractionData.customId;
                            return buttonInteractionData;
                        }
                    }
                }
            }
        }

    protected:
        string channelId;
        string messageId;
        string userId;
        string buttonId = "";
        unsigned long long startTime;
        bool doWeQuit = false;
        
        bool checkIfTimeHasPassed(unsigned long long maxWaitTimeInMs) {
            unsigned long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            if (currentTime - this->startTime > maxWaitTimeInMs) {
                return true;
            }
            else {
                return false;
            }
        }
    };

};
#endif