// DiscordCoreAPI.hpp - Header for the main input/output class of the Discord API.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_API_
#define _DISCORD_CORE_API_

#include "pch.h"
#include "HTTPController.hpp"
#include "ClientDataTypes.hpp"
#include "EventMachine.hpp"
#include "WebSocketAgents.hpp"
#include "DataManipFunctions.hpp"
#include "EventDataTypes.hpp"
#include "ClientClasses.hpp"
#include "FoundationClasses.hpp"
#include "SystemThreads.hpp"

namespace CommanderNS {

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	struct DiscordCoreAPI : public implements<DiscordCoreAPI, winrt::Windows::Foundation::IInspectable> {

	public:

		com_ptr<ClientClasses::Client> client{ nullptr };
		com_ptr<EventMachine> eventMachine{ nullptr };
		com_ptr<SystemThreads> systemThreads{ nullptr };
		com_ptr<RestAPI> restAPI{ nullptr };
		static bool doWeQuit;

		DiscordCoreAPI(hstring botToken) {
			this->systemThreads = make_self<SystemThreads>();
			this->systemThreads->initialize().get();
			this->webSocketConnection = winrt::make_self<WebSocketConnection>(this->buffer1, this->systemThreads->Threads->at(0).scheduler);
			this->webSocketReceiver = make_self<WebSocketReceiver>(this->buffer1, this->systemThreads->Threads->at(1).scheduler);
			this->botToken = botToken;
			this->restAPI = make_self<RestAPI>(this->botToken, this->baseURL, &webSocketConnection->socketPath);
			this->pHttpController = make_self<HTTPController>(this->systemThreads->Threads);
			this->client = make_self<ClientClasses::Client>(this->restAPI, this->systemThreads, this->pHttpController);
			this->eventMachine = make_self<EventMachine>();
			this->webSocketConnection->initialize(botToken, this->eventMachine, this->systemThreads, this->restAPI, this->client, this->pHttpController);
			this->webSocketReceiver->initialize(this->eventMachine, this->systemThreads, this->restAPI, this->client, this->pHttpController);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		int login() {
			int returnVal;
			this->systemThreads->mainThreadContext.taskGroup->run_and_wait([this, &returnVal] {returnVal = loginToWrap(); });
			return returnVal;
		}

	protected:

		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocketReceiver> webSocketReceiver{ nullptr };
		com_ptr<HTTPController> pHttpController;
		com_ptr<WebSocketConnection> webSocketConnection{ nullptr };
		unbounded_buffer<hstring> buffer1;

		int run() {
			this->webSocketConnection->start();
			this->webSocketReceiver->start();
			while (DiscordCoreAPI::doWeQuit == false) {
				if (doWeQuit == true) {
					this->pHttpController->getHTTPHandler->doWeQuit = true;
					this->pHttpController->getRequestSender->doWeQuit = true;
					this->pHttpController->deleteHTTPHandler->doWeQuit = true;
					this->pHttpController->deleteRequestSender->doWeQuit = true;
					this->pHttpController->putHTTPHandler->doWeQuit = true;
					this->pHttpController->putRequestSender->doWeQuit = true;
					this->pHttpController->postHTTPHandler->doWeQuit = true;
					this->pHttpController->postRequestSender->doWeQuit = true;
				}
				//agent::wait(this->webSocketReceiver.get());
				//CommanderNS::ClientClasses::Guild guild = this->client->Guilds.fetchAsync("782757641540730900").get();
				//cout << guild.Data.name << endl;
				//cout << guild.Data.name << endl;
				//cout << guild.Members.getGuildMemberAsync("821912684878364723").get().Data.user.username << endl;
				//vector<CommanderNS::ClientDataTypes::RoleData> roleData;
				//ClientDataTypes::GuildData guildData;
				//FoundationClasses::RateLimitData ratelimitdata;
				//for (unsigned int x = 0; x < roleData.size(); x += 1) {
					//cout << roleData.at(x).name << endl;
				
				//cout << "Name: " << this->Client->Guilds.GetGuild("782757641540730900").get().Members.GetGuildMember("644754671088566275").get().Data.user.username << endl;
			}
			std::cout << "Goodbye!" << std::endl;
			return 25;
		}

		int loginToWrap() {
			return this->run();
		}
	};

	bool DiscordCoreAPI::doWeQuit = false;

		BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
			switch (fdwCtrlType)
			{
				// Handle the CTRL-C signal.
			case CTRL_C_EVENT:
				std::cout << "Ctrl-C event\n";
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				return TRUE;

				// CTRL-CLOSE: confirm that the user wants to exit.
			case CTRL_CLOSE_EVENT:
				std::cout << "Ctrl-Close event\n";
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				return TRUE;

				// Pass other signals to the next handler.
			case CTRL_BREAK_EVENT:
				std::cout << "Ctrl-Break event\n";
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				return FALSE;

			case CTRL_LOGOFF_EVENT:
				std::cout << "Ctrl-LogOff event\n";
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				return FALSE;

			case CTRL_SHUTDOWN_EVENT:
				std::cout << "Ctrl-Shutdown event\n";
				DiscordCoreAPI::doWeQuit = true;
				WebSocketReceiver::doWeQuit = true;
				return FALSE;

			default:
				return FALSE;
			}
		}
};
#endif