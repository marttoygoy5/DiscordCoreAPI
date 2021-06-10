// WebSocketStuff.hpp - Header for the Websocket things of the library.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WEBSOCKET_STUFF_
#define _WEBSOCKET_STUFF_

#include "pch.h"
#include "JSONifier.hpp"

namespace DiscordCoreInternal {

	enum class WebSocketEventType {
		CHANNEL_CREATE = 0,
		CHANNEL_UPDATE = 1,
		CHANNEL_DELETE = 2,
		GUILD_CREATE = 3,
		GUILD_UPDATE = 4,
		GUILD_DELETE = 5,
		GUILD_BAN_ADD = 6,
		GUILD_BAN_REMOVE = 7,
		GUILD_MEMBER_ADD = 8,
		GUILD_MEMBER_REMOVE = 9,
		GUILD_MEMBER_UPDATE = 10,
		ROLE_CREATE = 11,
		ROLE_UPDATE = 12,
		ROLE_DELETE = 13,
		INVITE_CREATE = 14,
		INVITE_DELETE = 15,
		MESSAGE_CREATE = 16,
		MESSAGE_UPDATE = 17,
		MESSAGE_DELETE = 18,
		USER_UPDATE = 19,
		REACTION_ADD = 20,
		INTERACTION_CREATE = 21
	};

	struct WebSocketWorkload {
		json payLoad;
		WebSocketEventType eventType;
	};

	class WebSocketReceiverAgent : public agent {
	public:
		WebSocketReceiverAgent(ISource<json>& pWorkloadSource, ITarget<WebSocketWorkload>& pWorkloadTarget, ThreadContext threadContextNew = {nullptr, nullptr, nullptr})
			:workloadSource(pWorkloadSource),
			workloadTarget(pWorkloadTarget),
			agent(*threadContextNew.scheduler)
		{
			this->threadContext = threadContextNew;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		void terminate() {
			this->doWeQuit = true;
		}

		~WebSocketReceiverAgent(){
			this->terminate();
		}

	protected:
		bool doWeQuit = false;
		ThreadContext threadContext;
		ISource<json>& workloadSource;
		ITarget<WebSocketWorkload>& workloadTarget;
		unbounded_buffer<exception> errorBuffer;

		void run() {
			try {
				while (doWeQuit == false) {
					json payload;
					if (try_receive(this->workloadSource, payload)) {
						this->onMessageReceived(payload);
					}
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
				run();
			}
			done();
		}

		void onMessageReceived(json payload) {
			try {

				WebSocketWorkload workload;
				workload.payLoad = payload.at("d");

				if (payload.at("t") == "CHANNEL_CREATE") {
					workload.eventType = WebSocketEventType::CHANNEL_CREATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "CHANNEL_UPDATE") {
					workload.eventType = WebSocketEventType::CHANNEL_UPDATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "CHANNEL_DELETE") {
					workload.eventType = WebSocketEventType::CHANNEL_DELETE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_CREATE") {
					workload.eventType = WebSocketEventType::GUILD_CREATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_UPDATE") {
					workload.eventType = WebSocketEventType::GUILD_UPDATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_DELETE") {
					workload.eventType = WebSocketEventType::GUILD_DELETE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_BAN_ADD") {
					workload.eventType = WebSocketEventType::GUILD_BAN_ADD;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_BAN_REMOVE") {
					workload.eventType = WebSocketEventType::GUILD_BAN_REMOVE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_MEMBER_ADD") {
					workload.eventType = WebSocketEventType::GUILD_MEMBER_ADD;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_MEMBER_REMOVE") {
					workload.eventType = WebSocketEventType::GUILD_MEMBER_REMOVE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_MEMBER_UPDATE") {
					workload.eventType = WebSocketEventType::GUILD_MEMBER_UPDATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_ROLE_CREATE") {
					workload.eventType = WebSocketEventType::ROLE_CREATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_ROLE_UPDATE") {
					workload.eventType = WebSocketEventType::ROLE_UPDATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "GUILD_ROLE_DELETE") {
					workload.eventType = WebSocketEventType::ROLE_DELETE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "INVITE_CREATE") {
					workload.eventType = WebSocketEventType::INVITE_CREATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "INVITE_DELETE") {
					workload.eventType = WebSocketEventType::INVITE_DELETE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "MESSAGE_CREATE") {
					workload.eventType = WebSocketEventType::MESSAGE_CREATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "MESSAGE_UPDATE") {
					workload.eventType = WebSocketEventType::MESSAGE_UPDATE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "MESSAGE_DELETE") {
					workload.eventType = WebSocketEventType::MESSAGE_DELETE;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "MESSAGE_REACTION_ADD") {
					workload.eventType = WebSocketEventType::REACTION_ADD;
					send(this->workloadTarget, workload);
				}

				if (payload.at("t") == "INTERACTION_CREATE") {
					workload.eventType = WebSocketEventType::INTERACTION_CREATE;
					send(this->workloadTarget, workload);
				}
			}
			catch (winrt::hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;
				return;

			}
		}

	};

	class WebSocketConnectionAgent :public agent {
	public:

		WebSocketConnectionAgent(ITarget<json>& target,  hstring botTokenNew, ThreadContext threadContextNew = {nullptr ,nullptr, nullptr})
			:
			agent(*threadContextNew.scheduler),
			webSocketMessageTarget(target) {
			this->threadContext = threadContextNew;
			this->botToken = botTokenNew;
		}

		void setSocketPath(string socketPathBase){
			std::wstringstream stream;
			stream << DiscordCoreInternal::getSocketPath(to_hstring(socketPathBase)).c_str();
			stream << L"/?v=9&encoding=json";
			this->socketPath = stream.str();
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		void terminate() {
			done();
			return;
		}

		~WebSocketConnectionAgent() {
			this->cleanup();
			return;
		}

	protected:
		ThreadContext threadContext;
		event_token messageReceivedToken;
		event_token closedToken;
		MessageWebSocket webSocket{ nullptr };
		DataWriter messageWriter{ nullptr };
		hstring socketPath = L"";
		hstring botToken = L"";
		hstring sessionID = L"";
		int heartbeatInterval = 0;
		int lastNumberReceived = 0;
		int intentsValue = ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14));
		bool didWeReceiveHeartbeatAck = true;
		DispatcherQueueController dispatchQueueController{ nullptr };
		DispatcherQueue dispatchQueueForHB{ nullptr };
		DispatcherQueueTimer heartbeatTimer{ nullptr };
		ITarget<json>& webSocketMessageTarget;
		unbounded_buffer<exception> errorBuffer;

		void run() {
			try {
				this->connect();
				return;
			}
			catch (const exception& e) {
				cout << e.what() << endl;
				concurrency::send(errorBuffer, e);
				this->connect();
				return;
			}
			catch (hresult& e) {
				cout << e.value << endl;
				exception eNew(to_string(e.value).c_str());
				concurrency::send(errorBuffer, eNew);
				this->connect();
				return;
			}
			
		}

		void cleanup() {
			this->terminate();

			if (this->messageWriter != nullptr) {
				this->messageWriter.DetachStream();
				this->messageWriter.Close();
				this->messageWriter = nullptr;
			}

			if (this->webSocket != nullptr) {
				this->webSocket.Close(1000, L"Closed due to user request.");
				this->webSocket = nullptr;
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

		void connect() {
			try {
				this->dispatchQueueController = this->dispatchQueueController.CreateOnDedicatedThread();
				this->dispatchQueueForHB = this->dispatchQueueController.DispatcherQueue();
				this->heartbeatTimer = this->dispatchQueueForHB.CreateTimer();
				this->webSocket = MessageWebSocket();
				this->webSocket.Control().MessageType(SocketMessageType::Utf8);
				this->messageWriter = DataWriter(this->webSocket.OutputStream());
				this->messageWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
				this->closedToken = this->webSocket.Closed({ this, &WebSocketConnectionAgent::onClosed });
				this->messageReceivedToken = this->webSocket.MessageReceived({ this, &WebSocketConnectionAgent::onMessageReceived });
				this->webSocket.ConnectAsync(Uri(this->socketPath)).get();
			}
			catch (hresult result) {
				cout << result.value << endl;
				return;
			}
		}

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args) {
			wcout << L"WebSocket_Closed; Code: " << args.Code() << ", Reason: " << args.Reason().c_str() << endl;
		}

		void send(string& text) {

			string message = text;
			if (message.empty()) {
				cout << "Please specify text to send" << endl;
				return;
			}

			cout << "Sending Message: ";
			cout << message << endl;

			// Buffer any data we want to send.
			if (this->messageWriter != nullptr) {
				this->messageWriter.WriteString(to_hstring(message));
			}
			
			try {
				// Send the data as one complete message.
				if (this->messageWriter != nullptr) {
					this->messageWriter.StoreAsync().get();
				}
			}
			catch (hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;
			}

			cout << "Send Complete" << endl;
			return;
		}

		void OnHeartbeat(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
			this->sendHeartBeat();
		}

		void sendHeartBeat() {
			try {
				if (this->didWeReceiveHeartbeatAck == false) {
					this->cleanup();
					this->connect();
					this->didWeReceiveHeartbeatAck = true;
					return;
				}
				string heartbeat = getHeartbeatPayload(this->lastNumberReceived);
				this->send(heartbeat);
				this->didWeReceiveHeartbeatAck = false;
			}
			catch (hresult_error error) {
				wcout << error.message().c_str() << endl;
				return;
			}
		}

		void onMessageReceived(MessageWebSocket const& msgWebSocket, MessageWebSocketMessageReceivedEventArgs const& args) {
			try {
				DataReader dataReader{ nullptr };
				hstring message;
				if (args.IsMessageComplete()) {
					if (args.GetDataReader() != nullptr) {
						dataReader = args.GetDataReader();
						dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
						if (dataReader.UnconsumedBufferLength() > 0) {
							message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
						}
					}
				}				
				json payload = payload.parse(message);

				asend(&this->webSocketMessageTarget, payload);

				if (payload.at("s") >= 0) {
					this->lastNumberReceived = payload.at("s");
				}

				if (payload.at("t") == "PRESENCE_UPDATE") {
					return;
				}

				if (payload.at("t") == "GUILD_CREATE") {
					return;
				}

				if (payload.at("t") == "READY") {
					string sessionIDTemp;
					sessionIDTemp = payload.at("d").at("session_id");
					this->sessionID = to_hstring(sessionIDTemp);
				}

				if (payload.at("op") == 1) {
					this->sendHeartBeat();
				}

				if (payload.at("op") == 6) {
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->send(resume);
				}

				if (payload.at("op") == 7) {
					this->cleanup();
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->send(resume);
					this->connect();
				}

				if (payload.at("op") == 9) {
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->send(resume);
					this->connect();
				}

				if (payload.at("op") == 10) {
					this->heartbeatInterval = payload.at("d").at("heartbeat_interval");
					this->heartbeatTimer.Interval(chrono::milliseconds(this->heartbeatInterval));
					this->heartbeatTimer.Tick({ this, &WebSocketConnectionAgent::OnHeartbeat });
					this->heartbeatTimer.Start();
					std::string identity = getIdentifyPayload(to_string(this->botToken), this->intentsValue);
					this->send(identity);
				}

				if (payload.at("op") == 11) {
					this->didWeReceiveHeartbeatAck = true;
				}
				
				cout << "Message received from MessageWebSocket: " << to_string(message) << endl << endl;
			}
			catch (hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;
				return;
			}
		};
	};

}
#endif
