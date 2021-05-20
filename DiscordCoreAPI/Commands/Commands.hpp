// Commands.hpp - Wrangler header for the commands.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COMMANDS_
#define _COMMANDS

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Help.hpp"

namespace DiscordCoreAPI {	

	typedef void (*fncPtr)(DiscordCoreAPI::DiscordCoreFunctionBaseArguments*);

	map<string, DiscordCoreFunction> commands;

	class CommandController {
	public:
		static fncPtr checkForCommand(string messageContents) {
			if (messageContents[0] == commandPrefix[0]) {
				for (auto const& [key, value] : commands) {
					if (messageContents.find(key) == 2) {
						return value.functionPtr;
					}
				}
			}
		}

		static DiscordCoreFunctionBaseArguments parseArguments(string messageContents) {
			unsigned int startingPosition = messageContents.find("=");
			DiscordCoreFunctionBaseArguments args;
			if (startingPosition == string::npos) {
				return args;
			}

			string newString = messageContents.substr(startingPosition + 1);
			cout << "NewString: " << newString << endl;
			while (newString.find(",") != string::npos) {
				unsigned int startingPositionNew = newString.find(",");
				if (startingPositionNew == string::npos) {
					return args;
				}
				string newerString = newString.substr(startingPositionNew);
				unsigned int endingPosition = newerString.find(",");
				if (endingPosition == string::npos) {
					return args;
				}
				string argument = newerString.substr(startingPositionNew, endingPosition);
				newString = newerString.substr(startingPositionNew);
				args.argumentsArray.push_back(argument);
			}
			return args;
		}

		static void checkForAndRunCommand(string messageContents) {
			fncPtr functionPointer = checkForCommand(messageContents);

			DiscordCoreFunctionBaseArguments args = parseArguments(messageContents);

			for (unsigned int x = 0; x < args.argumentsArray.size(); x += 1) {
				cout << args.argumentsArray.at(x) << endl;
			}
		}
	};
	

};
#endif
