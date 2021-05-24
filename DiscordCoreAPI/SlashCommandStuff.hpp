// SlashCommandStuff.hpp - Stuff for slash commands.
// May 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SLASH_COMMAND_STUFF_
#define _SLASH_COMMAND_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
#include "HttpStuff.hpp"
#include "UserStuff.hpp"

namespace DiscordCoreAPI {

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
        string name;
        int	valueInt;
        string valueString;
    };

    struct ApplicationCommandOptionData {
        ApplicationCommandOptionType type;
        string name;
        string description;
        bool required;
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

    struct CreateSlashCommandData {
        string applicationId;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission = true;
    };

    struct DisplaySlashCommandData {
        string applicationId;
    };

    struct GetSlashCommandsData {
        string applicationId;
    };

    class ApplicationCommand {
    public:
        DiscordCoreInternal::ApplicationCommandData data;

        ApplicationCommand() {};

    protected:
        friend class SlashCommandManager;
        
        ApplicationCommand(DiscordCoreInternal::ApplicationCommandData dataNew) {
            this->data = dataNew;
        }
    };

	class SlashCommandManager {
	public:
		SlashCommandManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew) {
            this->agentResources = agentResourcesNew;
            this->threads = threadsNew;
		}

		task<ApplicationCommand> createSlashCommandAsync(CreateSlashCommandData createSlashCommandData) {
            DiscordCoreInternal::CreateSlashCommandData createSlashCommandDataNew;
            createSlashCommandDataNew.applicationId = createSlashCommandData.applicationId;
            createSlashCommandDataNew.defaultPermission = createSlashCommandData.defaultPermission;
            createSlashCommandDataNew.description = createSlashCommandData.description;
            createSlashCommandDataNew.name = createSlashCommandData.name;
            copyOptionsData(&createSlashCommandDataNew.options, createSlashCommandData.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.relativePath = "/applications/" + createSlashCommandData.applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_APPLICATION_COMMAND;
            workload.content = DiscordCoreInternal::getCreateSlashCommandPayload(createSlashCommandDataNew);
            cout << "SLASH COMMAND: " << workload.content << endl;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(4).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::createSlashCommandAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommandData;
		}

        task<vector<ApplicationCommand>> getSlashCommandsAsync(GetSlashCommandsData getSlashCommandsData) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + getSlashCommandsData.applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SLASH_COMMANDS;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(2).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::getSlashCommandsAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            vector<ApplicationCommand> appCommands;
            for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
                DiscordCoreInternal::ApplicationCommandData appCommandData;
                DiscordCoreInternal::parseObject(returnData.data.at(x), &appCommandData);
                ApplicationCommand appCommand(appCommandData);
                appCommands.push_back(appCommand);
            }
            co_return appCommands;
        }

        task<void> displaySlashCommandsAsync(DisplaySlashCommandData displaySlashCommandData) {
            vector<ApplicationCommand> applicationCommands = getSlashCommandsAsync({ displaySlashCommandData.applicationId }).get();
            for (unsigned int x=0; x< applicationCommands.size(); x+=1){
                cout << "Command Name: " << applicationCommands.at(x).data.name << endl;
                cout << "Command Description: " << applicationCommands.at(x).data.description << endl;
                displayOptions(applicationCommands.at(x).data.options);
            }
            co_return;
        }

	protected:
        DiscordCoreInternal::HttpAgentResources agentResources;
        concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

        void displayOptions(vector<DiscordCoreInternal::ApplicationCommandOptionData> applicationCommandOptionData) {
            for (unsigned int x = 0; x < applicationCommandOptionData.size(); x += 1) {
                cout << "Command Option Name: " << applicationCommandOptionData.at(x).name << endl;
                cout << "Command Option Description: " << applicationCommandOptionData.at(x).description << endl;
                if (applicationCommandOptionData.at(x).choices.size() > 0) {
                    for (unsigned int y = 0; y < applicationCommandOptionData.at(x).choices.size(); y += 1) {
                        cout << "Command Option Choice #" << y << " Name: " << applicationCommandOptionData.at(x).choices.at(y).name << endl;
                        if (applicationCommandOptionData.at(x).choices.at(y).valueString != "") {
                            cout << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueString << endl;
                        }
                        else {
                            cout << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueInt << endl;
                        }                        
                    }
                }
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND || applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    displayOptions(applicationCommandOptionData.at(x).options);
                }
            }
        }

        void copyOptionsData(vector<DiscordCoreInternal::ApplicationCommandOptionData>* appCommandOptionData, vector<ApplicationCommandOptionData> appCommandOptionDataSource) {
            
            if (appCommandOptionDataSource.size() > 0) {
                for (unsigned int x = 0; x < appCommandOptionDataSource.size(); x += 1) {
                    DiscordCoreInternal::ApplicationCommandOptionData appCommandOptionDataNew;
                    appCommandOptionDataNew.description = appCommandOptionDataSource.at(x).description;
                    appCommandOptionDataNew.name = appCommandOptionDataSource.at(x).name;
                    appCommandOptionDataNew.required = appCommandOptionDataSource.at(x).required;
                    appCommandOptionDataNew.type = (DiscordCoreInternal::ApplicationCommandOptionType)appCommandOptionDataSource.at(x).type;
                    if (appCommandOptionDataSource.at(x).choices.size() > 0) {
                        for (unsigned int y = 0; y < appCommandOptionDataSource.at(x).choices.size(); y += 1) {
                            DiscordCoreInternal::ApplicationCommandOptionChoiceData choiceData;
                            choiceData.name = appCommandOptionDataSource.at(x).choices.at(y).name;
                            choiceData.valueInt = appCommandOptionDataSource.at(x).choices.at(y).valueInt;
                            choiceData.valueString = appCommandOptionDataSource.at(x).choices.at(y).valueString;
                            appCommandOptionDataNew.choices.push_back(choiceData);
                        }
                    }
                    if (appCommandOptionDataNew.type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND || appCommandOptionDataNew.type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                        copyOptionsData(&appCommandOptionDataNew.options, appCommandOptionDataSource.at(x).options);
                    }
                    appCommandOptionData->push_back(appCommandOptionDataNew);
                    
                }

            }            
                
        }

	};
}
#endif
