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

    struct EditApplicationCommandData {
        string applicationId;
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
        bool defaultPermission = true;
    };

    struct DisplayApplicationCommandData {
        string applicationId;
    };

    struct DeleteApplicationCommandData {
        string applicationId;
        string name;
    };

    struct GetApplicationCommandsData {
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

		task<ApplicationCommand> createApplicationCommandAsync(CreateApplicationCommandData createApplicationCommandData) {
            DiscordCoreInternal::CreateApplicationCommandData createApplicationCommandDataNew;
            createApplicationCommandDataNew.applicationId = createApplicationCommandData.applicationId;
            createApplicationCommandDataNew.defaultPermission = createApplicationCommandData.defaultPermission;
            createApplicationCommandDataNew.description = createApplicationCommandData.description;
            createApplicationCommandDataNew.name = createApplicationCommandData.name;
            copyOptionsData(&createApplicationCommandDataNew.options, createApplicationCommandData.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.relativePath = "/applications/" + createApplicationCommandData.applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_APPLICATION_COMMAND;
            workload.content = DiscordCoreInternal::getCreateApplicationCommandPayload(createApplicationCommandDataNew);
            cout << "SLASH COMMAND: " << workload.content << endl;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(4).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            try {
                agent::wait(&httpRequestAgent);
            }
            catch (const exception& e) {
                cout << "SlashCommandManager::createApplicationCommandAsync() Error: " << e.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::createApplicationCommandAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            if (returnData.returnCode == 201) {
                cout << "SlashCommandManager::createApplicationCommandAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommandData;
		}

        task<vector<ApplicationCommand>> getApplicationCommandsAsync(GetApplicationCommandsData getApplicationCommandsData) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + getApplicationCommandsData.applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SLASH_COMMANDS;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(2).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            try {
                agent::wait(&httpRequestAgent);
            }
            catch (const exception& e) {
                cout << "SlashCommandManager::getApplicationCommandAsync() Error: " << e.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::getApplicationCommandsAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            if (returnData.returnCode == 201) {
                cout << "SlashCommandManager::getApplicationCommandsAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
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

        task<ApplicationCommand> editApplicationCommandAsync(EditApplicationCommandData editApplicationCommandData) {
            vector<ApplicationCommand> appCommands = getApplicationCommandsAsync({ .applicationId = editApplicationCommandData.applicationId }).get();
            bool isItFound = false;
            string appCommandId;
            for (auto const& [value] : appCommands) {
                cout << value.name << endl;
                if (value.name == editApplicationCommandData.name) {
                    appCommandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "SlashCommandManager::editApplicationCommand() Error: Sorry, it could not be found!" << endl;
                ApplicationCommand appCommand;
                co_return appCommand;
            }
            DiscordCoreInternal::EditApplicationCommandData editAppCommandData;
            editAppCommandData.defaultPermission = editApplicationCommandData.defaultPermission;
            editAppCommandData.description = editApplicationCommandData.description;
            editAppCommandData.name = editApplicationCommandData.name;
            copyOptionsData(&editAppCommandData.options, editApplicationCommandData.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.relativePath = "/applications/" + editApplicationCommandData.applicationId + "/commands/" + appCommandId;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_SLASH_COMMAND;
            workload.content = DiscordCoreInternal::getEditApplicationCommandPayload(editAppCommandData);
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(4).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            try {
                agent::wait(&httpRequestAgent);
            }
            catch (const exception& e) {
                cout << "SlashCommandManager::editApplicationCommandAsync() Error: " << e.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::editApplicationCommandsAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            if (returnData.returnCode == 200) {
                cout << "SlashCommandManager::editApplicationCommandsAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommand;
        }

        task<void> deleteApplicationCommand(DeleteApplicationCommandData deleteApplicationCommandData) {
            vector<ApplicationCommand> appCommands = getApplicationCommandsAsync({ .applicationId = deleteApplicationCommandData.applicationId }).get();
            string commandId;
            bool isItFound = false;
            for (auto const& [value] : appCommands) {
                if (value.name == deleteApplicationCommandData.name) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "SlashCommandManager::deleteApplicationCommand() Error: Sorry, it could not be found!" << endl;
                co_return;
            }
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/applications/" + deleteApplicationCommandData.applicationId + "/commands/" + commandId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_SLASH_COMMAND;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(8).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            try {
                agent::wait(&httpRequestAgent);
            }
            catch (const exception& e) {
                cout << "SlashCommandManager::deleteApplicationCommandAsync() Error: " << e.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::deleteApplicationCommandAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
            if (returnData.returnCode == 201) {
                cout << "SlashCommandManager::deleteApplicationCommandAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
            }
        }

        task<void> displayApplicationCommandsAsync(DisplayApplicationCommandData displayApplicationCommandData) {
            vector<ApplicationCommand> applicationCommands = getApplicationCommandsAsync({ displayApplicationCommandData.applicationId }).get();
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
