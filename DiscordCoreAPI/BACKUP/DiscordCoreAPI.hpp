// DiscordCoreAPI.hpp - Header for the main input/output class of the Discord API.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_API_
#define _DISCORD_CORE_API_

#include "pch.h"
#include "RestAPI.hpp"
#include "WebSocket.hpp"
#include "DataParsingFunctions.hpp"
#include "EventMachine.hpp"
#include "DataManipFunctions.hpp"
#include "EventDataTypes.hpp"
#include "FoundationClasses.hpp"
#include "ClientDataTypes.hpp"
#include "SystemThreads.hpp"
#include "ClientClasses.hpp"
#include "HttpAgents.hpp"

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, winrt::Windows::Foundation::IInspectable> {

	public:

		com_ptr<ClientClasses::Client> client{ nullptr };
		com_ptr<EventMachine> eventMachine{ nullptr };
		com_ptr<SystemThreads> systemThreads{ nullptr };
		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->systemThreads = make_self<SystemThreads>();
			this->systemThreads->initialize().get();
			this->webSocket = winrt::make_self<WebSocket>();
			this->botToken = botToken;
			//this->httpHandler = make_shared<HttpAgents::HTTPHandler>(this->systemThreads->mainThreadContext.scheduler, this->restAPI);
			this->restAPI = make_self<RestAPI>(this->botToken, this->baseURL, &webSocket->socketPath, this->systemThreads);
			this->client = make_self<ClientClasses::Client>(this->restAPI, this->httpHandler, this->systemThreads);
			this->eventMachine = make_self<EventMachine>();
			this->webSocket->initialize(botToken, this->eventMachine, this->systemThreads, this->restAPI, this->client, this->httpHandler);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		task<void> login() {
			this->systemThreads->mainThreadContext.taskGroup->run_and_wait([this] {loginToWrap(); });
			co_return;
		}

	protected:
		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocket> webSocket{ nullptr };
		com_ptr<RestAPI> restAPI{ nullptr };
		shared_ptr<HttpAgents::HTTPHandler> httpHandler{ nullptr };

		void connect() {
			this->webSocket->connectAsync().get();
			try {
			}
			catch (winrt::hresult result) {

				std::cout << result.value << std::endl;
			}
		}

		task<void> run() {
			this->connect();
			while (DiscordCoreAPI::doWeQuit == false) {
				//mmanderNS::ClientClasses::Guild guild = this->client->Guilds.FetchAsync("782757641540730900").get();
				//cout << guild.Members.GetGuildMemberAsync("821912684878364723").get().Data.user.username << endl;
				vector<CommanderNS::ClientDataTypes::RoleData> roleData;
				ClientDataTypes::GuildData guildData;
				FoundationClasses::RateLimitData ratelimitdata;
				//DataManipFunctions::getObjectDataAsync(this->restAPI, &ratelimitdata, "782757641540730900", &roleData).get();
				for (unsigned int x = 0; x < roleData.size(); x += 1) {
					cout << roleData.at(x).name << endl;
				}
				//cout << "Name: " << this->Client->Guilds.GetGuild("782757641540730900").get().Members.GetGuildMember("644754671088566275").get().Data.user.username << endl;
			}
			std::cout << "Goodbye!" << std::endl;
			co_return;
		}

		void loginToWrap() {
			this->run().get();
		}
	};

	bool DiscordCoreAPI::doWeQuit;

		BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
			switch (fdwCtrlType)
			{
				// Handle the CTRL-C signal.
			case CTRL_C_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				std::cout << "Ctrl-C event\n";
				return TRUE;

				// CTRL-CLOSE: confirm that the user wants to exit.
			case CTRL_CLOSE_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				std::cout << "Ctrl-Close event\n";
				return TRUE;

				// Pass other signals to the next handler.
			case CTRL_BREAK_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				std::cout << "Ctrl-Break event\n";
				return FALSE;

			case CTRL_LOGOFF_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				std::cout << "Ctrl-LogOff event\n";
				return FALSE;

			case CTRL_SHUTDOWN_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				std::cout << "Ctrl-Shutdown event\n";
				return FALSE;

			default:
				return FALSE;
			}
		}
};
#endif