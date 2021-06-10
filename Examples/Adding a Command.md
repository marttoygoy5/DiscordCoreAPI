### **Adding a Command**
#### 1. In a new header for the command #include the pch.h,../DiscordCoreClient.hpp, and Commands.hpp files.
#### 2. Create a new class, within the DiscordCoreAPI namespace, derived from the BaseFunction class.
#### 3. Set the commandName and helpDescription members of the class.
#### 4. Add a virtual task<"void"> execute function, with the argument of type DiscordCoreAPI::BaseFunctionArguments*.
#### 5. Create an instance of this new class within the DiscordCoreAPI namespace.
#### 6. CONTINUED FURTHER DOWN.

```C++
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}
	
		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {
				if (args->argumentsArray.at(0) == "test") {
					task<AuditLogData> auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync({ .actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE, .guildId = args->eventData.getGuildId(), .limit = 25,   .userId = args->eventData.getAuthorId() });

					for (auto value : auditLogData.get().auditLogEntries) {
						cout << "ID: " << value.id << endl;
						cout << "TARGET ID: " << value.targetId << endl;
						for (auto value2 : value.changes) {
							cout << value2.newValueString << endl;
							cout << value2.oldValueString << endl;
						}
					}
					args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync({ .deletePinned = false,.channelId = args->eventData.getChannelId(),.limit = 25, .beforeThisId = args->eventData.getMessageId() }).get();
				}
				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}
		}
	};
	Test test{};
}
#endif
```
#### 6. Add the command to the library's command list by using the DiscordCoreAPI::CommandController::addCommand() function.

// Main.cpp - Main source file.
// https://github.com/RealTimeChris

#include "pch.h"
#include "Index.hpp"

int main() {
    winrt::init_apartment();
    string botToken = "BOT_TOKEN_HERE";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::test, DiscordCoreAPI::test.commandName);
    agent::wait((agent*)pDiscordCoreClient.get());
    exception error;
    while (pDiscordCoreClient.get()->getError(error)) {
        cout << "DiscordCoreClient() Error: " << error.what() << endl << endl;
    }
    return 1;
}
