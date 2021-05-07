// WebSocket.hpp - Header for the Websocket class.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WEB_SOCKET_
#define _WEB_SOCKET_

#include "pch.h"
#include "DataParsingFunctions.hpp"
#include "EventMachine.hpp"
#include "DataManipFunctions.hpp"
#include "SystemThreads.hpp"
#include "ClientClasses.hpp"

namespace CommanderNS {


	struct WebSocket :public concurrency::agent,  implements<WebSocket, winrt::Windows::Foundation::IInspectable> {
	public:

		WebSocket(ITarget<hstring>& target, Scheduler* pScheduler) 
			: _target(target),
			agent(*pScheduler)
		{}

		~WebSocket() {
			this->cleanup();
		}

	protected:
		
		friend struct DiscordCoreAPI;
		ITarget<hstring>& _target;
		com_ptr<SystemThreads> pSystemThreads;
		com_ptr<EventMachine> pEventMachine;
		com_ptr<RestAPI> pRestAPI;
		com_ptr<ClientClasses::Client> pClient;
		shared_ptr<HttpAgents::HTTPHandler> pHttpHandler;
		event_token messageReceivedToken;
		event_token closedToken;
		MessageWebSocket webSocket;
		DataWriter messageWriter;
		hstring socketPath = L"";
		hstring botToken = L"";
		hstring sessionID = L"";
		int heartbeatInterval = 0;
		int lastNumberReceived = 0;
		int intentsValue = 0;
		bool didWeReceiveHeartbeatAck = true;
		DispatcherQueueController dispatchQueueController = nullptr;
		DispatcherQueue dispatchQueueForHB = nullptr;
		DispatcherQueueTimer heartbeatTimer = nullptr;

		void run() {
			this->connectAsync();
		}

		void initialize(hstring botTokenNew, winrt::com_ptr<EventMachine> pEventMachineNew, com_ptr<SystemThreads> pSystemThreadsNew, com_ptr<RestAPI> pRestAPINew, com_ptr<ClientClasses::Client> pClientNew, shared_ptr<HttpAgents::HTTPHandler> pHttpHandler) {
			this->pSystemThreads = pSystemThreadsNew;
			this->pClient = pClientNew;
			this->pRestAPI = pRestAPINew;
			this->pEventMachine = pEventMachineNew;
			this->botToken = botTokenNew;
			this->pHttpHandler = pHttpHandler;
			this->intentsValue = ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14));
		}
		
		void cleanup() {
			if (this->messageWriter) {
				try {
					this->messageWriter.DetachStream();
					this->messageWriter.Close();
					this->messageWriter = nullptr;
				}
				catch (winrt::hresult_error error) {
					std::wcout << error.message().c_str() << std::endl;
				}
			}

			if (this->webSocket) {
				try {
					this->webSocket.Close(1000, winrt::to_hstring("Closed due to user request."));
					this->webSocket = nullptr;
				}
				catch (winrt::hresult_error error) {
					std::wcout << error.message().c_str() << std::endl;
				}
			}

			if (this->heartbeatTimer) {
				this->heartbeatTimer.Stop();
				this->heartbeatTimer = nullptr;
			}

			if (this->dispatchQueueForHB) {
				this->dispatchQueueForHB = nullptr;
			}

			if (this->dispatchQueueController) {
				this->dispatchQueueController = nullptr;
			}
		}

		IAsyncAction connectAsync() {
			try {
				this->dispatchQueueController = this->dispatchQueueController.CreateOnDedicatedThread();
				this->dispatchQueueForHB = this->dispatchQueueController.DispatcherQueue();
				this->heartbeatTimer = this->dispatchQueueForHB.CreateTimer();
				this->webSocket = MessageWebSocket();
				this->webSocket.Control().MessageType(SocketMessageType::Utf8);
				this->messageWriter = DataWriter(this->webSocket.OutputStream());
				this->messageWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
				this->closedToken = this->webSocket.Closed({ this, &WebSocket::onClosed });
				this->messageReceivedToken = this->webSocket.MessageReceived({ this, &WebSocket::onMessageReceived });
				co_await this->webSocket.ConnectAsync(Uri(this->socketPath));
			}
			catch (winrt::hresult result) {
				std::cout << result.value << std::endl;
			}
		}

		void onClosed(IWebSocket const& /* sender */, WebSocketClosedEventArgs const& args) {
			std::wcout << L"WebSocket_Closed; Code: " << args.Code() << ", Reason: \"" << args.Reason().c_str() << "\"" << std::endl;
		}

		IAsyncAction sendAsync(std::string& text) {

			std::string message = text;
			if (message.empty()) {
				std::cout << "Please specify text to send" << std::endl;
				co_return;
			}

			std::cout << "Sending Message: ";
			std::cout << message << std::endl;

			// Buffer any data we want to send.
			this->messageWriter.WriteString(winrt::to_hstring(message));

			try {
				// Send the data as one complete message.
				co_await this->messageWriter.StoreAsync();

			}
			catch (winrt::hresult_error const& ex) {
				std::wcout << ex.message().c_str() << std::endl;
				co_return;
			}

			std::cout << "Send Complete" << std::endl;
		}

		void OnHeartbeat(IInspectable const& sender, IInspectable const& args) {
			this->sendHeartBeat();
		}

		fire_and_forget onGuildCreate(json payload) {
			CommanderNS::ClientDataTypes::GuildData guildData;
			string id = payload.at("d").at("id");
			CommanderNS::DataParsingFunctions::parseObject(payload.at("d"), &guildData);
			ClientClasses::Guild guild(guildData, this->pRestAPI, this->pSystemThreads, this->pHttpHandler);
			this->pClient->Guilds.insert(std::make_pair(id, guild));
			for (unsigned int y = 0; y < guild.Data.members.size(); y += 1) {
				ClientClasses::User user(guild.Data.members.at(y).user);
				this->pClient->Users.insert(make_pair(user.Data.id, user));
			}
			co_return;
		}

		fire_and_forget sendHeartBeat() {
			try {
				if (this->didWeReceiveHeartbeatAck == false) {
					this->cleanup();
					this->connectAsync();
					co_return;
				}
				std::string heartbeat = JSONifier::getHeartbeatPayload(this->lastNumberReceived);
				this->sendAsync(heartbeat);
				this->didWeReceiveHeartbeatAck = false;
			}
			catch (hresult_error error) {
				wcout << error.message().c_str() << std::endl;
			}
		}

		void onMessageReceived(MessageWebSocket const& , MessageWebSocketMessageReceivedEventArgs const& args) {
			try {
				DataReader dataReader{ args.GetDataReader() };
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				auto message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
				asend(_target, message);
				
				json payload = payload.parse(message);

				if (payload.at("s") >= 0) {
					this->lastNumberReceived = payload.at("s");
				}
				
				if (payload.at("t") == "PRESENCE_UPDATE") {
					return;
				}
			
				if (payload.at("op") == 6) {
					std::string resume = JSONifier::getResumePayload(winrt::to_string(this->botToken), winrt::to_string(this->sessionID), this->lastNumberReceived);
					this->sendAsync(resume);
				}

				if (payload.at("op") == 7) {
					this->cleanup();
					std::string resume = JSONifier::getResumePayload(winrt::to_string(this->botToken), winrt::to_string(this->sessionID), this->lastNumberReceived);
					this->sendAsync(resume);
					this->connectAsync().get();
				}

				if (payload.at("op") == 9) {

				}

				if (payload.at("t") == "READY") {
					std::string sessionIDTemp;
					sessionIDTemp = payload.at("d").at("session_id");
					this->sessionID = winrt::to_hstring(sessionIDTemp);
				}

				if (payload.at("op") == 1) {
					this->sendHeartBeat();
				}

				if (payload.at("op") == 11) {
					this->didWeReceiveHeartbeatAck = true;
				}

				if (payload.at("op") == 10) {
					this->heartbeatInterval = payload.at("d").at("heartbeat_interval");
					this->heartbeatTimer.Interval(std::chrono::milliseconds(this->heartbeatInterval));
					this->heartbeatTimer.Tick({ this, &WebSocket::OnHeartbeat });
					this->heartbeatTimer.Start();
					std::string identity = JSONifier::getIdentifyPayload(winrt::to_string(this->botToken), this->intentsValue);
					this->sendAsync(identity);
				}
				
				std::cout << "Message received from MessageWebSocket: " << winrt::to_string(message) << std::endl << std::endl;
			
			}
			
			catch (winrt::hresult_error const& ex) {
				std::wcout << ex.message().c_str() << std::endl;
				
			}
			
		};
	};
};
#endif
