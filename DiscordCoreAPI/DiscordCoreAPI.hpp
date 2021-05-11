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

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, winrt::Windows::Foundation::IInspectable> {

	public:

		com_ptr<ClientClasses::Client> Client{ nullptr };
		com_ptr<EventMachine> pEventMachine{ nullptr };
		com_ptr<SystemThreads> pSystemThreads{ nullptr };

		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->pSystemThreads = make_self<SystemThreads>();
			this->pSystemThreads->initialize().get();
			this->pWebSocketReceiver = make_self<WebSocketReceiver>(this->pSystemThreads->Threads.at(0).scheduler);
			this->pWebSocketConnection = make_self<WebSocketConnection>(this->pWebSocketReceiver->buffer00, this->pSystemThreads->Threads.at(0).scheduler);
			this->botToken = botToken;
			this->pRestAPI = make_self<RestAPI>(this->botToken, this->baseURL, &pWebSocketConnection->socketPath, this->pSystemThreads);
			this->Client = make_self<ClientClasses::Client>(this->pRestAPI);
			this->pEventMachine = make_self<EventMachine>();
			this->pWebSocketConnection->initialize(botToken, this->pEventMachine, this->pSystemThreads, this->pRestAPI, this->Client);
			this->pWebSocketReceiver->initialize(this->pEventMachine, this->pSystemThreads, this->pRestAPI, this->Client);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		task<int> login() {
			int returnVal;
			this->pSystemThreads->Threads.at(0).taskGroup->run_and_wait([this, &returnVal] {returnVal = loginToWrap(); });
			co_return returnVal;
		}

	protected:
		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocketReceiver> pWebSocketReceiver{ nullptr };
		com_ptr<WebSocketConnection> pWebSocketConnection{ nullptr };
		com_ptr<RestAPI> pRestAPI{ nullptr };

		task<void> connect() {
			try {
				this->pWebSocketConnection->start();
				this->pWebSocketReceiver->start();
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
				return TRUE;

				// CTRL-CLOSE: confirm that the user wants to exit.
			case CTRL_CLOSE_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Close event\n";
				return TRUE;

				// Pass other signals to the next handler.
			case CTRL_BREAK_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Break event\n";
				return FALSE;

			case CTRL_LOGOFF_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-LogOff event\n";
				return FALSE;

			case CTRL_SHUTDOWN_EVENT:
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				std::cout << "Ctrl-Shutdown event\n";
				return FALSE;

			default:
				return FALSE;
			}
		}
};
#endif
