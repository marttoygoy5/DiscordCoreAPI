// DiscordCoreAPI.hpp - Header for the main input/output class of the Discord API.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_API_
#define _DISCORD_CORE_API_

#include "pch.h"
#include "RestAPI.hpp"
#include "EventMachine.hpp"
#include "SystemThreads.hpp"
#include "WebSocketAgents.hpp"
#include "ClientClasses.hpp"
#include "MainAgent.hpp"

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, winrt::Windows::Foundation::IInspectable> {

	public:

		static com_ptr<ClientClasses::Client> Client;
		com_ptr<EventMachine> pEventMachine{ nullptr };
		com_ptr<SystemThreads> pSystemThreads{ nullptr };

		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->pSystemThreads = make_self<SystemThreads>();
			this->pSystemThreads->initialize().get();
			this->botToken = botToken;
			hstring socketPath;
			this->pRestAPI = make_self<RestAPI>(this->botToken, this->baseURL, &socketPath, this->pSystemThreads);
			DiscordCoreAPI::Client = make_self<ClientClasses::Client>(this->pRestAPI);
			this->pClientAgent = make_self<ClientClasses::ClientAgent>(DiscordCoreAPI::Client, this->pSystemThreads->Threads.at(1).scheduler);
			this->pClientAgent->start();
			this->pMainAgent = make_self<ClientClasses::MainAgent>(this->pRestAPI, this->pSystemThreads->Threads.at(0).scheduler);
			this->pMainAgent->start();
			this->pWebSocketReceiver = make_self<WebSocketReceiver>(this->pMainAgent);
			this->pWebSocketConnection = make_self<WebSocketConnection>(this->pWebSocketReceiver->buffer00);
			this->pWebSocketConnection->socketPath = socketPath;
			this->Client->initialize(this->pClientAgent, this->pMainAgent);
			this->pEventMachine = make_self<EventMachine>();
			this->pWebSocketConnection->initialize(botToken, this->pEventMachine, this->pSystemThreads, this->pRestAPI, DiscordCoreAPI::Client);
			this->pWebSocketReceiver->initialize(this->pEventMachine, this->pSystemThreads, this->pRestAPI, DiscordCoreAPI::Client);
			this->pWebSocketConnection->start();
			this->pWebSocketReceiver->start();
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		int login() {
			int returnVal;
			loginToWrap();
			return returnVal;
		}

	protected:
		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocketReceiver> pWebSocketReceiver{ nullptr };
		com_ptr<WebSocketConnection> pWebSocketConnection{ nullptr };
		com_ptr<RestAPI> pRestAPI{ nullptr };
		com_ptr<ClientClasses::ClientAgent> pClientAgent{ nullptr };
		com_ptr<ClientClasses::MainAgent> pMainAgent{ nullptr };

		task<void> connect() {
			try {
				//this->pWebSocketConnection->start();
				//this->pWebSocketReceiver->start();
			}
			catch (winrt::hresult result) {

				cout << "connect() Error: " << result.value << std::endl;
			}
			co_return;
		}

		task<int> run() {
			this->connect().get();
			while (DiscordCoreAPI::doWeQuit == false) {
				
			}
			cout << "Goodbye!" << std::endl;
			co_return 25;
		}

		int loginToWrap() {
			return this->run().get();
		}
	};

	bool DiscordCoreAPI::doWeQuit;

		BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
			switch (fdwCtrlType)
			{
				// Handle the CTRL-C signal.
			case CTRL_C_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-C event\n";
				exit(EXIT_SUCCESS);

				// CTRL-CLOSE: confirm that the user wants to exit.
			case CTRL_CLOSE_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Close event\n";
				exit(EXIT_SUCCESS);

				// Pass other signals to the next handler.
			case CTRL_BREAK_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Break event\n";
				exit(EXIT_SUCCESS);

			case CTRL_LOGOFF_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-LogOff event\n";
				exit(EXIT_SUCCESS);

			case CTRL_SHUTDOWN_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Shutdown event\n";
				exit(EXIT_SUCCESS);

			default:
				return FALSE;
			}
		}
		com_ptr<struct CommanderNS::ClientClasses::Client> CommanderNS::DiscordCoreAPI::Client;
};
#endif
