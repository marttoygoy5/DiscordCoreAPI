// Commands.hpp - Wrangler header for the commands.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COMMANDS_
#define _COMMANDS

#include "../pch.h"
#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	string commandPrefix{ "!" };

	struct DiscordCoreFunctionBaseArguments {
		vector<string> argumentsArray;
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> pClient;
		Message message;
	};

	class BaseFunction {
	public:
		virtual void execute(DiscordCoreFunctionBaseArguments args) = 0;
		string commandName;
	};

	class CommandController {
	public:

		static void addCommand(BaseFunction* newFunction, string newFunctionName) {
			newFunction->commandName = newFunctionName;
			DiscordCoreAPI::CommandController::commands.insert(make_pair(newFunction->commandName, newFunction));
		}

		static void checkForAndRunCommand(CommandData commandData) {
			BaseFunction* functionPointer = getCommand(commandData.message.data.content);

			if (functionPointer == nullptr) {
				return;
			}

			DiscordCoreFunctionBaseArguments args = parseArguments(commandData.message.data.content);
			args.pClient = commandData.pClient;
			args.message = commandData.message;

			functionPointer->execute(args);
		}
		
	protected:
		static map<string, BaseFunction*> commands;

		static BaseFunction* getCommand(string messageContents) {
			if (messageContents[0] == commandPrefix[0]) {
				for (auto const& [key, value] : DiscordCoreAPI::CommandController::commands) {
					if (messageContents.find(key) != string::npos) {
						return value;
					}
					else {
						return nullptr;
					}
				}
			}
			else {
				return nullptr;
			}
			return nullptr;
		}

		static DiscordCoreFunctionBaseArguments parseArguments(string messageContents) {
			size_t startingPosition = messageContents.find("=");
			DiscordCoreFunctionBaseArguments args;
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
					args.argumentsArray.push_back(argument);
					return args;
				}
				argument = newString.substr(0, endingPosition);
				newString = newString.substr(endingPosition + 1);
				args.argumentsArray.push_back(argument);
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
					args.argumentsArray.push_back(argument);
					return args;
				}
				argument = newString.substr(0, endingPosition);
				newString = newString.substr(endingPosition + 1);
				args.argumentsArray.push_back(argument);
				if (newString.find(",") == string::npos) {
					startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					endingPosition = newString.find(",");
					if (endingPosition == string::npos) {
						argument = newString.substr(0);
						args.argumentsArray.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					args.argumentsArray.push_back(argument);
				}
			}
			return args;
		}
	};
	
	map<string, BaseFunction*> CommandController::commands;
};
#endif
