// Help.hpp - Header for the "help" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELP_
#define _HELP_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {

	class Help : public  BaseFunction {
	public:
		Help() {
			this->commandName = "help";
			this->helpDescription = "__**Help:**__ Enter !help or /help for help!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

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

				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Help help{};
}
#endif
