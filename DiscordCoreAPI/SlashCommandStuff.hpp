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
        int	valueInt = 0;
        string valueString = "";
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
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission = true;
    };

    struct DeleteApplicationCommandData {
        string name;
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
		SlashCommandManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, string applicationIdNew) {
            this->agentResources = agentResourcesNew;
            this->threads = threadsNew;
            this->applicationId = applicationIdNew;
		}

		task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateApplicationCommandData createApplicationCommandData) {
            DiscordCoreInternal::CreateApplicationCommandData createApplicationCommandDataNew;
            createApplicationCommandDataNew.applicationId = this->applicationId;
            createApplicationCommandDataNew.defaultPermission = createApplicationCommandData.defaultPermission;
            createApplicationCommandDataNew.description = createApplicationCommandData.description;
            createApplicationCommandDataNew.name = createApplicationCommandData.name;
            copyOptionsData(&createApplicationCommandDataNew.options, createApplicationCommandData.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_APPLICATION_COMMAND;
            workload.content = DiscordCoreInternal::getCreateApplicationCommandPayload(createApplicationCommandDataNew);
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(4).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            exception error;
            while (httpRequestAgent.getError(error)) {
                cout << "SlashCommandManager::createGlobalApplicationCommandAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::createGlobalApplicationCommandAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::createGlobalApplicationCommandAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommandData;
		}

        task<vector<ApplicationCommand>> getGlobalApplicationCommandsAsync() {
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SLASH_COMMANDS;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(2).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            exception error;
            while (httpRequestAgent.getError(error)) {
                cout << "SlashCommandManager::getGlobalApplicationCommandAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::getGlobalApplicationCommandsAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::getGlobalApplicationCommandsAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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

        task<ApplicationCommand> editGlobalApplicationCommandAsync(EditApplicationCommandData editApplicationCommandData) {
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
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
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + appCommandId;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_SLASH_COMMAND;
            workload.content = DiscordCoreInternal::getEditApplicationCommandPayload(editAppCommandData);
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(4).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            exception error;
            while (httpRequestAgent.getError(error)) {
                cout << "SlashCommandManager::editGlobalApplicationCommandAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::editGlobalApplicationCommandsAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::editGlobalApplicationCommandsAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommand;
        }

        task<void> deleteGlobalApplicationCommand(DeleteApplicationCommandData deleteApplicationCommandData) {
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
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
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + commandId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_SLASH_COMMAND;
            DiscordCoreInternal::HttpRequestAgent httpRequestAgent(this->agentResources, this->threads->at(8).scheduler);
            send(httpRequestAgent.workSubmissionBuffer, workload);
            httpRequestAgent.start();
            agent::wait(&httpRequestAgent);
            exception error;
            while (httpRequestAgent.getError(error)) {
                cout << "SlashCommandManager::deleteGlobalApplicationCommandAsync() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(httpRequestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::deleteGlobalApplicationCommandAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::deleteGlobalApplicationCommandAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
        }

        task<void> displayGlobalApplicationCommandsAsync() {
            vector<ApplicationCommand> applicationCommands = getGlobalApplicationCommandsAsync().get();
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
        string applicationId;

        void displayOptions(vector<DiscordCoreInternal::ApplicationCommandOptionData> applicationCommandOptionData) {
            for (unsigned int x = 0; x < applicationCommandOptionData.size(); x += 1) {
                string indentAmount;
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    indentAmount += "   ";
                }
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND) {
                    indentAmount += "      ";
                }
                cout << indentAmount + "Command Option Name: " << applicationCommandOptionData.at(x).name << endl;
                cout << indentAmount + "Command Option Description: " << applicationCommandOptionData.at(x).description << endl;
                if (applicationCommandOptionData.at(x).choices.size() > 0) {
                    for (unsigned int y = 0; y < applicationCommandOptionData.at(x).choices.size(); y += 1) {
                        cout << "Command Option Choice #" << y << " Name: " << applicationCommandOptionData.at(x).choices.at(y).name << endl;
                        if (applicationCommandOptionData.at(x).choices.at(y).valueString != "") {
                            cout << "         " << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueString << endl;
                        }
                        else {
                            cout << "         " << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueInt << endl;
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
