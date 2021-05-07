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
#include "WebSocketAgent.hpp"

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
			this->webSocket = winrt::make_self<WebSocket>(this->buffer1, this->systemThreads->Threads.at(0).scheduler);
			this->webSocketAgent = make_self<WebSocketAgent>(this->buffer1, this->systemThreads->Threads.at(1).scheduler);
			this->botToken = botToken;
			this->restAPI = make_self<RestAPI>(this->botToken, this->baseURL, &webSocket->socketPath, this->systemThreads);
			this->client = make_self<ClientClasses::Client>(this->restAPI, this->httpHandler, this->systemThreads);
			this->eventMachine = make_self<EventMachine>();
			this->webSocket->initialize(botToken, this->eventMachine, this->systemThreads, this->restAPI, this->client, this->httpHandler);
			this->webSocketAgent->initialize(botToken, this->eventMachine, this->systemThreads, this->restAPI, this->client, this->httpHandler);
			SetConsoleCtrlHandler(CommanderNS::CtrlHandler, TRUE);
		}

		void login() {
			this->systemThreads->mainThreadContext.taskGroup->run_and_wait([this] {loginToWrap(); });
			return;
		}

	protected:
		hstring baseURL = L"https://discord.com/api/v9";
		hstring botToken;
		com_ptr<WebSocket> webSocket{ nullptr };
		com_ptr<WebSocketAgent> webSocketAgent{ nullptr };
		com_ptr<RestAPI> restAPI{ nullptr };
		shared_ptr<HttpAgents::HTTPHandler> httpHandler{ nullptr };
		unbounded_buffer<hstring> buffer1;

		void run() {
			this->webSocket->start();
			this->webSocketAgent->start();
			while (DiscordCoreAPI::doWeQuit == false) {
				agent::wait(this->webSocket.get());
				//CommanderNS::ClientClasses::Guild guild = this->client->Guilds.fetchAsync("782757641540730900").get();
				//cout << guild.Members.GetGuildMemberAsync("821912684878364723").get().Data.user.username << endl;
				//vector<CommanderNS::ClientDataTypes::RoleData> roleData;
				//ClientDataTypes::GuildData guildData;
				//FoundationClasses::RateLimitData ratelimitdata;
				//for (unsigned int x = 0; x < roleData.size(); x += 1) {
					//cout << roleData.at(x).name << endl;
				
				//cout << "Name: " << this->Client->Guilds.GetGuild("782757641540730900").get().Members.GetGuildMember("644754671088566275").get().Data.user.username << endl;
			}
			std::cout << "Goodbye!" << std::endl;
		}

		void loginToWrap() {
			this->run();
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
				return TRUE;

				// CTRL-CLOSE: confirm that the user wants to exit.
			case CTRL_CLOSE_EVENT:
				std::cout << "Ctrl-Close event\n";
				DiscordCoreAPI::doWeQuit = true;
				return TRUE;

				// Pass other signals to the next handler.
			case CTRL_BREAK_EVENT:
				std::cout << "Ctrl-Break event\n";
				DiscordCoreAPI::doWeQuit = true;
				return FALSE;

			case CTRL_LOGOFF_EVENT:
				std::cout << "Ctrl-LogOff event\n";
				DiscordCoreAPI::doWeQuit = true;
				return FALSE;

			case CTRL_SHUTDOWN_EVENT:
				std::cout << "Ctrl-Shutdown event\n";
				DiscordCoreAPI::doWeQuit = true;
				return FALSE;

			default:
				return FALSE;
			}
		}
};
#endif