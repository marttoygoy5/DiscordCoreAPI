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

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, winrt::Windows::Foundation::IInspectable> {

	public:

		ClientClasses::Client Client;
		com_ptr<EventMachine> eventMachine{ nullptr };
		SystemThreads* systemThreads{ nullptr };

		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->systemThreads = new SystemThreads;
			this->systemThreads->initialize().get();
			this->pWebSocket = winrt::make_self<WebSocket>();
			this->botToken = botToken;
			this->pRestAPI = make_self<RestAPI>(this->botToken, this->baseURL, &pWebSocket->socketPath);
			this->Client = ClientClasses::Client(this->pRestAPI);
			this->eventMachine = make_self<EventMachine>();
			this->pWebSocket->initialize(botToken, this->eventMachine, this->systemThreads, this->pRestAPI, &this->Client);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		task<void> login() {
			co_await resume_foreground(*this->systemThreads->Threads.at(1).threadQueue.get());
			task_handle taskHandle = make_task([this]() {this->loginToWrap().get(); });
			this->systemThreads->Threads.at(1).taskGroup->run_and_wait(taskHandle);
			co_return;
		}

	protected:
		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocket> pWebSocket;
		com_ptr<RestAPI> pRestAPI;

		void connect() {
			try {
				this->pWebSocket->connectAsync().get();
			}
			catch (winrt::hresult result) {

				std::cout << result.value << std::endl;
			}
		}

		task<void> run() {
			co_await resume_foreground(*this->systemThreads->Threads.at(3).threadQueue.get());
			this->connect();
			int value = 0;
			while (DiscordCoreAPI::doWeQuit == false) {
				value += 1;
				CommanderNS::ClientClasses::Guild guild = this->Client.Guilds.Fetch("782757641540730900").get();
				cout << guild.Members.GetGuildMember("821912684878364723").get().Data.user.username << endl;
				vector<CommanderNS::ClientDataTypes::RoleData> roleData;
				shared_ptr<FoundationClasses::RateLimitation>rateLimitData = make_shared<FoundationClasses::RateLimitation>();
				ClientDataTypes::GuildData guildData;
				DataManipFunctions::getObjectDataAsync(this->pRestAPI, rateLimitData, "782757641540730900", &roleData).get();
				for (unsigned int x = 0; x < roleData.size(); x += 1) {
					//cout << roleData.at(x).name << endl;
				}
				cout << "Name: " << this->Client.Guilds.Fetch("782757641540730900").get().Members.Fetch("644754671088566275").get().Data.user.username << endl;
			}
			std::cout << "Goodbye!" << std::endl;
		}

		task<void> loginToWrap() {
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