// InteractionManager.hpp - Header for the interaction manager class.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INTERACTION_MANAGER_
#define _INTERACTION_MANAGER_

#include "pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

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

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3
    };

    struct InputEventData {
        InputEventData() {};
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
        DiscordCoreAPI::DiscordCoreClient* pDiscordCoreClient;
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

    struct DeleteInteractionData {
        string applicationId;
        string interactionToken;
    };

    struct CreateFollowUpMessageData {
        string applicationId;
        string interactionToken;
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags;
    };

    struct EditFollowUpMessageData {
        string applicationId;
        string interactionToken;
        string messageId;
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags;
    };

    struct CreateInteractionResponseData {
        InteractionCallbackType type;
        InteractionApplicationCommandCallbackData data;
        string interactionId;
        string interactionToken;
    };

    struct CreateDeferredInteractionResponseData {
        InteractionCallbackType type;
        string interactionId;
        string interactionToken;
    };

    struct EditInteractionResponseData {
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<AttachmentData> attachments;
        vector<ActionRowData>components;
        string applicationId;
        string interactionToken;
        InteractionCallbackType type;
        int flags;
    };

    struct ButtonInteractionData {
        string token;
        string id;
        string applicationId;
        InteractionType type;
        string customId;
        GuildMemberData member;
        UserData user;
        MessageData message;
        string channelId;
        string guildId;
    };

    class InteractionManagerAgent : public agent {
    public:
        static unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* requestPostDeferredBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* requestPostFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* requestEditFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::DeleteInteractionData>* requestDeleteInteractionBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* requestPostBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* requestPatchBuffer;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
        static unbounded_buffer<MessageData>* outBuffer;
        static unbounded_buffer<exception>* errorBuffer;

        InteractionManagerAgent(Scheduler* pSchedulerNew)
            :agent(*pSchedulerNew) {}

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
        }

        static void initialize(concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            InteractionManagerAgent::requestPostDeferredBuffer = new unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>;
            InteractionManagerAgent::requestPostFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>;
            InteractionManagerAgent::requestEditFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>;
            InteractionManagerAgent::requestDeleteInteractionBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteInteractionData>;
            InteractionManagerAgent::requestPostBuffer = new unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>;
            InteractionManagerAgent::requestPatchBuffer = new unbounded_buffer <DiscordCoreInternal::EditInteractionResponseData>;
            InteractionManagerAgent::outBuffer = new unbounded_buffer<MessageData>;
            InteractionManagerAgent::errorBuffer = new unbounded_buffer<exception>;
            InteractionManagerAgent::threads = threadsNew;
        }

        task<MessageData> patchObjectDataAsync(DiscordCoreInternal::EditFollowUpMessageData dataPackage){
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_FOLLOW_UP_MESSAGE;
            workload.content = dataPackage.finalContent;
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
            DiscordCoreAPI::MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            co_return messageData;
        }

        task<MessageData> patchObjectDataAsync(DiscordCoreInternal::EditInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            workload.content = dataPackage.finalContent;
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
            MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            co_return messageData;
        }

        task<MessageData> postObjectDataAsync(DiscordCoreInternal::CreateInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
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
            MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            co_return messageData;
        }

        task<MessageData> postObjectDataAsync(DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_DEFERRED_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
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
            MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            co_return messageData;
        }

        task<MessageData> postObjectDataAsync(DiscordCoreInternal::CreateFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_FOLLOW_UP_MESSAGE;
            workload.content = DiscordCoreInternal::getPostFollowUpMessagePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::postObjectDataAsync() Error 02: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectDataAsync() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectDataAsync() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            co_return messageData;
        }

        task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteInteractionData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_INTERACTION;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::deleteObjectDataAsync() Error 02: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::deleteObjectDataAsync() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::deleteObjectDataAsync() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            cout << returnData.data << endl;
            co_return;
        }

        task<void> deleteObjectAsync(DiscordCoreInternal::DeleteInteractionData dataPackage) {
            if (dataPackage.timeDelayInMs> 0){
                DispatcherQueueTimer timer = InteractionManagerAgent::threads->at(9).threadQueue.get()->CreateTimer();
                timer.Interval(chrono::milliseconds(dataPackage.timeDelayInMs));
                timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
                    deleteObjectDataAsync(dataPackage).get();
                    timer.Stop();
                    });
                timer.Start();
            }
            else {
                deleteObjectDataAsync(dataPackage).get();
            }
            co_return;
        }

    protected:

        void run() {
            try {
                DiscordCoreInternal::EditInteractionResponseData dataPackage;
                if (try_receive(InteractionManagerAgent::requestPatchBuffer, dataPackage)) {
                    MessageData messageData = patchObjectDataAsync(dataPackage).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
                }
                DiscordCoreInternal::CreateInteractionResponseData dataPackageNew;
                if (try_receive(InteractionManagerAgent::requestPostBuffer, dataPackageNew)) {
                    MessageData messageData = postObjectDataAsync(dataPackageNew).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
                }
                DiscordCoreInternal::CreateFollowUpMessageData dataPackage2;
                if (try_receive(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackage2)) {
                    MessageData messageData = postObjectDataAsync(dataPackage2).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionData deleteDataPackage;
                if (try_receive(InteractionManagerAgent::requestDeleteInteractionBuffer, deleteDataPackage)) {
                    deleteObjectAsync(deleteDataPackage).get();
                }
                DiscordCoreInternal::EditFollowUpMessageData editFollowUpMessagePackage;
                if (try_receive(InteractionManagerAgent::requestEditFollowUpMessageBuffer, editFollowUpMessagePackage)) {
                    MessageData messageData = patchObjectDataAsync(editFollowUpMessagePackage).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
                }
                DiscordCoreInternal::CreateDeferredInteractionResponseData deferredData;
                if (try_receive(InteractionManagerAgent::requestPostDeferredBuffer, deferredData)) {
                    MessageData messageData = postObjectDataAsync(deferredData).get();
                    send(InteractionManagerAgent::outBuffer, messageData);
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

        static task<MessageData> createInteractionResponseAsync(CreateInteractionResponseData interactionData) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::CreateInteractionResponseData interactionDataNew;
            interactionDataNew.data = interactionData.data;
            interactionDataNew.interactionId = interactionData.interactionId;
            interactionDataNew.interactionToken = interactionData.interactionToken;
            interactionDataNew.agentResources = InteractionManager::agentResources;
            interactionDataNew.type = (DiscordCoreInternal::InteractionCallbackType)interactionData.type;
            interactionDataNew.threadContext = InteractionManager::threads->at(6);
            interactionDataNew.content = DiscordCoreInternal::getCreateInteractionPayload(interactionDataNew);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostBuffer, interactionDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

        static task<MessageData> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData interactionData) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::CreateDeferredInteractionResponseData interactionDataNew;
            interactionDataNew.interactionId = interactionData.interactionId;
            interactionDataNew.interactionToken = interactionData.interactionToken;
            interactionDataNew.type = (DiscordCoreInternal::InteractionCallbackType)interactionData.type;
            interactionDataNew.agentResources = InteractionManager::agentResources;
            interactionDataNew.threadContext = InteractionManager::threads->at(6);
            interactionDataNew.content = DiscordCoreInternal::getCreateInteractionPayload(interactionDataNew);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostDeferredBuffer, interactionDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::createDeferredInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

        static task<void> deleteInteractionResponseAsync(DeleteInteractionData interactionData, unsigned int timeDelayInMsNew) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::DeleteInteractionData interactionDataNew;
            interactionDataNew.agentResources = InteractionManager::agentResources;
            interactionDataNew.threadContext = InteractionManager::threads->at(6);
            interactionDataNew.applicationId = interactionData.applicationId;;
            interactionDataNew.interactionToken = interactionData.interactionToken;
            interactionDataNew.timeDelayInMs = timeDelayInMsNew;
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestDeleteInteractionBuffer, interactionDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::deleteInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            co_return;
        }

        static task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData editData) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::EditFollowUpMessageData interactionDataNew;
            interactionDataNew.agentResources = InteractionManager::agentResources;
            interactionDataNew.threadContext = InteractionManager::threads->at(6);
            interactionDataNew.applicationId = editData.applicationId;
            interactionDataNew.interactionToken = editData.interactionToken;
            interactionDataNew.allowedMentions = editData.allowedMentions;
            interactionDataNew.avatarUrl = editData.avatarUrl;
            for (auto value : editData.components) {
                interactionDataNew.components.push_back(value);
            }
            for (auto value : editData.embeds) {
                interactionDataNew.embeds.push_back(value);
            }
            interactionDataNew.content = editData.content;
            interactionDataNew.flags = editData.flags;
            interactionDataNew.interactionToken = editData.interactionToken;
            interactionDataNew.messageId = editData.messageId;
            interactionDataNew.tts = editData.tts;
            interactionDataNew.username = editData.username;
            interactionDataNew.finalContent = DiscordCoreInternal::getEditFollowUpMessagePayload(interactionDataNew);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestEditFollowUpMessageBuffer, interactionDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::editFollowUpMessageAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

        static task<MessageData> editInteractionResponseAsync(EditInteractionResponseData editInteractionData) {
            DiscordCoreInternal::EditInteractionResponseData editInteractionResponseDataNew;
            editInteractionResponseDataNew.applicationId = editInteractionData.applicationId;
            editInteractionResponseDataNew.agentResources = InteractionManager::agentResources;
            editInteractionResponseDataNew.allowedMentions = editInteractionData.allowedMentions;
            for (auto value : editInteractionData.attachments) {
                editInteractionResponseDataNew.attachments.push_back(value);
            }
            for (auto value : editInteractionData.components) {
                editInteractionResponseDataNew.components.push_back(value);
            }
            editInteractionResponseDataNew.content = editInteractionData.content;
            for (auto value : editInteractionData.embeds) {
                editInteractionResponseDataNew.embeds.push_back(value);
            }
            editInteractionResponseDataNew.flags = editInteractionData.flags;
            editInteractionResponseDataNew.interactionToken = editInteractionData.interactionToken;
            editInteractionResponseDataNew.threadContext = InteractionManager::threads->at(6);
            editInteractionResponseDataNew.type = (DiscordCoreInternal::InteractionCallbackType)editInteractionData.type;
            editInteractionResponseDataNew.finalContent = DiscordCoreInternal::getEditInteractionPayload(editInteractionResponseDataNew);
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPatchBuffer, editInteractionResponseDataNew);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

        static task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData createFollowUpMessageData) {
            DiscordCoreInternal::CreateFollowUpMessageData dataPackage;
            dataPackage.agentResources = InteractionManager::agentResources;
            dataPackage.threadContext = InteractionManager::threads->at(3);
            dataPackage.allowedMentions = createFollowUpMessageData.allowedMentions;
            dataPackage.applicationId = createFollowUpMessageData.applicationId;
            dataPackage.avatarURL = createFollowUpMessageData.avatarUrl;
            dataPackage.interactionToken = createFollowUpMessageData.interactionToken;
            for (auto value : createFollowUpMessageData.components) {
                dataPackage.components.push_back(value);
            }
            dataPackage.content = createFollowUpMessageData.content;
            for (auto value : createFollowUpMessageData.embeds) {
                dataPackage.embeds.push_back(value);
            }
            dataPackage.flags = createFollowUpMessageData.flags;
            dataPackage.interactionToken = createFollowUpMessageData.interactionToken;
            dataPackage.tts = createFollowUpMessageData.tts;
            dataPackage.username = createFollowUpMessageData.username;
            InteractionManagerAgent interactionManagerAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackage);
            interactionManagerAgent.start();
            agent::wait(&interactionManagerAgent);
            exception error;
            while (interactionManagerAgent.getError(error)) {
                cout << "InteractionManager::createFollowUpMessage() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outBuffer, messageData);
            co_return messageData;
        }

	protected:
        friend struct Button;
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

    };
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;
    unbounded_buffer<InteractionData>* InteractionManager::inputInteractionBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* InteractionManagerAgent::requestPostBuffer;
    unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* InteractionManagerAgent::requestPatchBuffer;
    unbounded_buffer<exception>* InteractionManagerAgent::errorBuffer;
    unbounded_buffer<ButtonInteractionData>* InteractionManager::buttonInteractionBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* InteractionManagerAgent::requestPostFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* InteractionManagerAgent::requestEditFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::DeleteInteractionData>* InteractionManagerAgent::requestDeleteInteractionBuffer;
    unbounded_buffer<MessageData>* InteractionManagerAgent::outBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* InteractionManagerAgent::requestPostDeferredBuffer;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;
    map<string, Button> InteractionManager::activeButtons;
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManagerAgent::threads;

    struct Button {
    public:

        Button(InputEventData eventData) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->channelId = eventData.getChannelId();
            this->messageId = eventData.getMessageId();
            this->userId = eventData.requesterId;
            InteractionManager::activeButtons.insert(make_pair(this->channelId + this->messageId + this->userId, *this));
        }

        static void initialize(DiscordCoreClient* pDiscordCoreClientNew) {
            Button::pDiscordCoreClient = pDiscordCoreClientNew;
        }

        string getButtonId() {
            if (buttonId == "") {
                return "";
            }
            else {
                return this->buttonId;
                this->buttonId = "";
            }
        }

        ButtonInteractionData getOurButtonData(unsigned long long maxWaitTimeInMs) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
            while (doWeQuit == false) {
                ButtonInteractionData buttonInteractionData;
                if (checkIfTimeHasPassed(maxWaitTimeInMs)) {
                    this->buttonId = "exit";
                    doWeQuit = true;
                }
                else {
                    if (try_receive(InteractionManager::buttonInteractionBuffer, buttonInteractionData)) {
                        if ((buttonInteractionData.channelId != this->channelId) || (buttonInteractionData.message.id != this->messageId)||(this->userId != buttonInteractionData.user.id||this->userId != buttonInteractionData.member.user.id)) {
                            if (buttonInteractionData.applicationId != "") {
                                CreateInteractionResponseData createResponseData;
                                createResponseData.interactionId = buttonInteractionData.id;
                                createResponseData.interactionToken = buttonInteractionData.token;
                                createResponseData.type = InteractionCallbackType::ChannelMessageWithSource;
                                createResponseData.data.flags = 64;
                                EmbedData embedData;
                                embedData.setColor("FEFEFE");
                                embedData.setTitle("__**Button Issue**__");
                                embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                                embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                                createResponseData.data.embeds.push_back(embedData);
                                InteractionManager::createInteractionResponseAsync(createResponseData).get();
                            }
                        }
                        else {
                            this->buttonId = buttonInteractionData.customId;
                            return buttonInteractionData;
                        }
                    }
                }                
            }
            return ButtonInteractionData();
        }

    protected:
        static DiscordCoreClient* pDiscordCoreClient;
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
    DiscordCoreClient* Button::pDiscordCoreClient;
};
#endif
