// DiscordCoreAPI.hpp - Header for the main input/output class of the Discord API.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "WebSocket.hpp"
#include "RestAPI.hpp"
#include "DataParsingFunctions.hpp"
#include "EventMachine.hpp"
#include "DataManipFunctions.hpp"
#include "EventDataTypes.hpp"
#include "FoundationClasses.hpp"
#include "ClientClasses.hpp"

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, IInspectable> {

	public:

		winrt::com_ptr<EventMachine> eventMachine;

		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->botToken = botToken;
			this->webSocket = winrt::make_self<WebSocket>();
			this->restAPI = make_self<RestAPI>(this->botToken, this->baseURL, &webSocket->socketPath);
			this->Client = ClientClasses::Client(this->restAPI);
			this->eventMachine = make_self<EventMachine>();
			this->webSocket->initialize(botToken, this->eventMachine, this->restAPI, &this->Client);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		void login() {
			this->run();
		}

	protected:

		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		winrt::com_ptr<WebSocket> webSocket;
		winrt::com_ptr<RestAPI> restAPI;
		ClientClasses::Client Client;

		void connect() {
			try {
				this->webSocket->connectAsync().get();
			}
			catch (winrt::hresult result) {
				std::cout << result.value << std::endl;
			}
		}

		void run() {
			this->connect();
			while (DiscordCoreAPI::doWeQuit == false) {
				std::cout << "Name: " << this->Client.Guilds.Fetch("782757641540730900").get().Members.GetGuildMember("641183425977516042").get().Data.nick<< std::endl;
			}
			std::cout << "Goodbye!" << std::endl;
		}
	};

		bool DiscordCoreAPI::doWeQuit = false;

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
