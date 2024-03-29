// Commands.hpp - Wrangler header for the commands.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COMMANDS_
#define _COMMANDS_

#include "../pch.h"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClient;

	struct BaseFunctionArguments {
		InputEventData eventData;
		vector<string> argumentsArray;
	};

	class BaseFunction {
	public:
		virtual task<void> execute(BaseFunctionArguments* args) = 0;
		string commandName;
		string helpDescription;
	};

	class CommandController {
	public:

		static void addCommand(BaseFunction* newFunction, string newFunctionName) {
			newFunction->commandName = newFunctionName;
			DiscordCoreAPI::CommandController::commands.insert(make_pair(newFunction->commandName, newFunction));
		}

		static void checkForAndRunCommand(CommandData commandData) {
			try {
				BaseFunction* functionPointer{ nullptr };
				bool messageOption = false;
				if (commandData.eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					functionPointer = CommandController::getCommand(convertToLowerCase(commandData.eventData.messageData.content), commandData);
					messageOption = true;
				}
				else if (commandData.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					DiscordCoreInternal::CommandData commandDataNew;
					DiscordCoreInternal::parseObject(commandData.eventData.interactionData.dataRaw, &commandDataNew);
					string newCommandName = commandData.eventData.discordCoreClient->discordUser->data.prefix + commandDataNew.commandName;
					functionPointer = CommandController::getCommand(convertToLowerCase(newCommandName), commandData);
					messageOption = false;
				}

				if (functionPointer == nullptr) {
					return;
				}

				BaseFunctionArguments args(commandData.eventData);

				if (messageOption == true) {
					args.argumentsArray = CommandController::parseArguments(commandData.eventData.messageData.content);
					args.eventData.messageData = commandData.eventData.messageData;
				}
				else if (messageOption == false) {
					DiscordCoreInternal::CommandData commandDataNew;
					DiscordCoreInternal::parseObject(commandData.eventData.interactionData.dataRaw, &commandDataNew);
					args.argumentsArray = commandDataNew.optionsArgs;
					args.eventData.interactionData = commandData.eventData.interactionData;
				}

				functionPointer->execute(&args).get();
			}
			catch (exception& e) {
				cout << "CommandController::checkForAndRunCommands() Error: " << e.what() << endl << endl;
			}

		}

		static map<string, BaseFunction*> commands;

	protected:

		static BaseFunction* getCommand(string messageContents, CommandData commandData) {
			try {
				unsigned int currentPosition = INFINITE;
				BaseFunction* lowestValue{ nullptr };
				for (auto const& [key, value] : DiscordCoreAPI::CommandController::commands) {
					if (messageContents[0] == commandData.eventData.discordCoreClient->discordUser->data.prefix[0]) {
						if (messageContents.find(key) != string::npos && messageContents.find(key) < currentPosition) {
							currentPosition = messageContents.find(key);
							lowestValue = value;
						}
					}
				}
				return lowestValue;
			}
			catch (exception& e) {
				cout << "CommandController::getCommand() Error: " << e.what() << endl << endl;
			}
			
		}

		static vector<string >parseArguments(string messageContents) {
			vector<string> args;
			try {
				size_t startingPosition = messageContents.find("=");
				if (startingPosition == string::npos) {
					return args;
				}
				string newString = messageContents.substr(startingPosition + 1);
				if (newString.find(",") == string::npos) {
					size_t startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					size_t endingPosition = newString.find(",");
					string argument;
					if (endingPosition == string::npos) {
						argument = newString.substr(0);
						args.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					newString = newString.substr(endingPosition + 1);
					args.push_back(argument);
					return args;
				}
				while (newString.find(",") != string::npos) {
					size_t startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					size_t endingPosition = newString.find(",");
					string argument;
					if (endingPosition == string::npos) {
						argument = newString.substr(0);
						args.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					newString = newString.substr(endingPosition + 1);
					args.push_back(argument);
					if (newString.find(",") == string::npos) {
						startingPositionNew = newString.find_first_not_of(" ");
						if (startingPositionNew == string::npos) {
							return args;
						}
						newString = newString.substr(startingPositionNew);
						endingPosition = newString.find(",");
						if (endingPosition == string::npos) {
							argument = newString.substr(0);
							args.push_back(argument);
							return args;
						}
						argument = newString.substr(0, endingPosition);
						args.push_back(argument);
					}
				}
				return args;
			}
			catch (exception& e) {
				cout << "CommandController::parseArguments() Error: " << e.what() << endl << endl;
				return args;
			}
		}
	};
	map<string, BaseFunction*> CommandController::commands;
};
#endif
