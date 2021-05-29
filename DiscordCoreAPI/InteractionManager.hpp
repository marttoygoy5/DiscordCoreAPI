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

	string constructStringContent(DiscordCoreInternal::InteractionIncData interactionIncData) {
        string finalCommandString;
        finalCommandString = interactionIncData.commandName + " = ";
		for (auto& value : interactionIncData.optionsArgs) {
            finalCommandString += value + ", ";
		}
        return finalCommandString;
	}

	class Interaction {
	public:
        DiscordCoreInternal::InteractionData data;
		Interaction(DiscordCoreInternal::InteractionData dataNew) {
            this->data = dataNew;
		}

	};

	class InteractionManager {
	public:
		InteractionManager() {

		}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threads = threadsNew;
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
                cout << "SlashCommandManager::createInteractionResponseDeferral() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::createInteractionResponseDeferralAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::createInteractionResponseDeferralAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_return;
        }

	protected:
        static map<string, Interaction> waitingInteractions;
        static DiscordCoreInternal::HttpAgentResources agentResources;
        static concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

	};
    map<string, Interaction> InteractionManager::waitingInteractions;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;
    concurrent_vector<DiscordCoreInternal::ThreadContext>* InteractionManager::threads;

};
#endif