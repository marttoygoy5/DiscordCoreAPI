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

	struct CommandData {
		Message message;
		DiscordCoreClient* coreClient;
	};

	struct BaseFunctionArguments {
		Message message;
		vector<string> argumentsArray;
		DiscordCoreAPI::DiscordCoreClient* coreClient;
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

		static void checkForAndRunCommands(CommandData commandData) {
			try {
				BaseFunction* functionPointer = getCommand(convertToLowerCase(commandData.message.data.content));

				if (functionPointer == nullptr) {
					return;
				}

				BaseFunctionArguments args(commandData.message);
				args.argumentsArray = parseArguments(commandData.message.data.content);
				args.coreClient = commandData.coreClient;
				args.message = commandData.message;

				functionPointer->execute(&args).get();
			}
			catch (exception& e) {
				cout << "CommandController::checkForAndRunCommands() Error: " << e.what() << endl << endl;
			}

		}

		static map<string, BaseFunction*> commands;

	protected:

		static BaseFunction* getCommand(string messageContents) {
			try {
				if (messageContents[0] == commandPrefix[0]) {
					for (auto const& [key, value] : DiscordCoreAPI::CommandController::commands) {
						if (messageContents.find(key) != string::npos) {
							return value;
						}
					}
				}
				else {
					return nullptr;
				}
				return nullptr;
			}
			catch (exception& e) {
				cout << "CommandController::getCommand() Error: " << e.what() << endl << endl;
			}
			
		}

		static vector<string >parseArguments(string messageContents) {
			try {
				size_t startingPosition = messageContents.find("=");
				vector<string> args;
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
			}
			
		}
	};
	map<string, BaseFunction*> CommandController::commands;
};
#endif
