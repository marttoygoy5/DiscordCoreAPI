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

    struct DeleteInteractionResponseData {
        string applicationId;
        string interactionToken;
        unsigned int timeDelay = 0;
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
        bool isItForHere;
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
        static unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* requestPostDeferredInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* requestPostFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* requestPatchFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>* requestDeleteInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* requestPostInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* requestPatchInteractionResponseBuffer;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
        static unbounded_buffer<MessageData>* outInteractionResponseBuffer;
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
            InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>;
            InteractionManagerAgent::requestPostFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>;
            InteractionManagerAgent::requestPatchFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>;
            InteractionManagerAgent::requestDeleteInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>;
            InteractionManagerAgent::requestPostInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>;
            InteractionManagerAgent::requestPatchInteractionResponseBuffer = new unbounded_buffer <DiscordCoreInternal::EditInteractionResponseData>;
            InteractionManagerAgent::outInteractionResponseBuffer = new unbounded_buffer<MessageData>;
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
            cout << "MESSAGE CONTENT: " << workload.content << endl;
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

        task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
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
            co_return;
        }

        task<void> deleteObjectDataTimerAsync(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
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
                DiscordCoreInternal::EditInteractionResponseData dataPackage01;
                if (try_receive(InteractionManagerAgent::requestPatchInteractionResponseBuffer, dataPackage01)) {
                    MessageData messageData = patchObjectDataAsync(dataPackage01).get();
                    send(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateInteractionResponseData dataPackage02;
                if (try_receive(InteractionManagerAgent::requestPostInteractionResponseBuffer, dataPackage02)) {
                    MessageData messageData = postObjectDataAsync(dataPackage02).get();
                    send(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateFollowUpMessageData dataPackage03;
                if (try_receive(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackage03)) {
                    MessageData messageData = postObjectDataAsync(dataPackage03).get();
                    send(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionResponseData dataPackage04;
                if (try_receive(InteractionManagerAgent::requestDeleteInteractionResponseBuffer, dataPackage04)) {
                    deleteObjectDataTimerAsync(dataPackage04).get();
                }
                DiscordCoreInternal::EditFollowUpMessageData dataPackage05;
                if (try_receive(InteractionManagerAgent::requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    MessageData messageData = patchObjectDataAsync(dataPackage05).get();
                    send(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackage06;
                if (try_receive(InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer, dataPackage06)) {
                    MessageData messageData = postObjectDataAsync(dataPackage06).get();
                    send(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
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

		InteractionManager() {}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threads = threadsNew;
        }

        static task<MessageData> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.threadContext = InteractionManager::threads->at(6);
            dataPackageNew.content = DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createDeferredInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        static task<MessageData> createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::CreateInteractionResponseData dataPackageNew;
            dataPackageNew.data = dataPackage.data;
            dataPackageNew.interactionId = dataPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.threadContext = InteractionManager::threads->at(6);
            dataPackageNew.content = DiscordCoreInternal::getCreateInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        static task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::EditInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.applicationId;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            for (auto value : dataPackage.attachments) {
                dataPackageNew.attachments.push_back(value);
            }
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.threadContext = InteractionManager::threads->at(4);
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.finalContent = DiscordCoreInternal::getEditInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(3).scheduler);
            send(InteractionManagerAgent::requestPatchInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            co_await mainThread;
            co_return messageData;
        }

        static task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::DeleteInteractionResponseData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.threadContext = InteractionManager::threads->at(8);
            dataPackageNew.applicationId = dataPackage.applicationId;;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(7).scheduler);
            send(InteractionManagerAgent::requestDeleteInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::deleteInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            co_return;
        }

        static task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::CreateFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.threadContext = InteractionManager::threads->at(6);
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            dataPackageNew.applicationId = dataPackage.applicationId;
            dataPackageNew.avatarURL = dataPackage.avatarUrl;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.tts = dataPackage.tts;
            dataPackageNew.username = dataPackage.username;
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(5).scheduler);
            send(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createFollowUpMessage() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            co_await mainThread;
            co_return messageData;
        }

        static task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            DiscordCoreInternal::EditFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.threadContext = InteractionManager::threads->at(4);
            dataPackageNew.applicationId = dataPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            dataPackageNew.avatarUrl = dataPackage.avatarUrl;
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.messageId = dataPackage.messageId;
            dataPackageNew.tts = dataPackage.tts;
            dataPackageNew.username = dataPackage.username;
            dataPackageNew.finalContent = DiscordCoreInternal::getEditFollowUpMessagePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(InteractionManager::threads->at(3).scheduler);
            send(InteractionManagerAgent::requestPatchFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editFollowUpMessageAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        protected:
        friend struct Button;
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
    };
    
    unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* InteractionManagerAgent::requestPostInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* InteractionManagerAgent::requestPatchInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* InteractionManagerAgent::requestPostFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* InteractionManagerAgent::requestPatchFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>* InteractionManagerAgent::requestDeleteInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer;
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManagerAgent::threads;
    unbounded_buffer<MessageData>* InteractionManagerAgent::outInteractionResponseBuffer;
    unbounded_buffer<exception>* InteractionManagerAgent::errorBuffer;
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;

    struct Button : public agent{
    public:

        static unbounded_buffer<ButtonInteractionData>* buttonInteractionBuffer;
        unbounded_buffer<ButtonInteractionData>* buttonIncomingInteractionBuffer;
        string channelId;
        string messageId;
        string userId;
        string buttonId = "";

        Button(InputEventData dataPackage) {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.requesterId;
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<ButtonInteractionData>([this](ButtonInteractionData interactionData)->bool{
                if (this->channelId != interactionData.channelId || this->messageId != interactionData.message.id) {
                    return false;
                }
                else {
                    return true;
                }
                });
            Button::buttonInteractionBuffer->link_target(this->buttonIncomingInteractionBuffer);
        }

        ~Button() {
            Button::buttonInteractionBuffer->unlink_target(this->buttonIncomingInteractionBuffer);
        };

        static void initialize(DiscordCoreClient* discordCoreClientNew) {
            Button::buttonInteractionBuffer = new unbounded_buffer<ButtonInteractionData>;
            Button::discordCoreClient = discordCoreClientNew;
        }

        string getButtonId() {
            if (buttonId == "") {
                return "";
            }
            else {
                return this->buttonId;
            }
        }

        task<ButtonInteractionData> getOurButtonData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew) {
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            co_return this->interactionData;
        }

    protected:
        static DiscordCoreClient* discordCoreClient;
        unsigned long long startTime;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        bool doWeQuit = false;
        ButtonInteractionData interactionData;

        bool checkIfTimeHasPassed(unsigned long long maxWaitTimeInMs) {
            unsigned long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            if (currentTime - this->startTime > maxWaitTimeInMs) {
                return true;
            }
            else {
                return false;
            }
        }

        void run() {
            this->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            while (doWeQuit == false) {
                ButtonInteractionData buttonInteractionData;
                if (checkIfTimeHasPassed(this->maxTimeInMs)) {
                    this->buttonId = "exit";
                    doWeQuit = true;
                }
                else {
                    if (this->getButtonDataForAll == false) {
                        if (try_receive(this->buttonIncomingInteractionBuffer, buttonInteractionData)) {
                            if (buttonInteractionData.user.id != this->userId) {
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
                            else {
                                this->interactionData = buttonInteractionData;
                                this->buttonId = buttonInteractionData.customId;
                                break;
                            }
                        }
                    }
                    else {
                        if (try_receive(this->buttonIncomingInteractionBuffer, buttonInteractionData)) {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            break;
                        }
                    }
                }
            }
            done();
        }
    };
    DiscordCoreClient* Button::discordCoreClient;
    unbounded_buffer<ButtonInteractionData>* Button::buttonInteractionBuffer;

};
#endif
