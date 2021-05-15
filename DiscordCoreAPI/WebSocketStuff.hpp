// WebSocketStuff.hpp - Header for the Websocket things of the library.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WEBSOCKET_STUFF_
#define _WEBSOCKET_STUFF_

#include "pch.h"

namespace DiscordCoreInternal {

	enum class WebSocketEventType {
		GUILD_CREATE = 0,
		GUILD_MEMBER_ADD = 1,
		ROLE_UPDATE = 2,
		MESSAGE_CREATE = 3,
		MESSAGE_DELETE = 4,
		CHANNEL_UPDATE = 5,
		USER_UPDATE = 6,
		REACTION_ADD = 7
	};

	struct WebSocketWorkload {
		json payLoad;
		WebSocketEventType eventType;
	};

	struct WebSocketReceiverAgent : agent, implements<WebSocketReceiverAgent, winrt::Windows::Foundation::IInspectable> {
	public:
		WebSocketReceiverAgent(ISource<json>& pWorkloadSource, ITarget<WebSocketWorkload>& pWorkloadTarget, ThreadContext threadContextNew = {nullptr, nullptr, nullptr})
			:workloadSource(pWorkloadSource),
			workloadTarget(pWorkloadTarget),
			agent(*threadContextNew.scheduler)
		{
			this->threadContext = threadContextNew;
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

		void run() {
			while (doWeQuit == false) {
				json payload;
				if (try_receive(this->workloadSource, payload)) {
					this->onMessageReceived(payload);
				}
			}
			done();
		}

		fire_and_forget onGuildCreate(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload;
			workload.eventType = WebSocketEventType::GUILD_CREATE;
			send(this->workloadTarget, workload);
			co_return;
		}

		fire_and_forget onMessageCreate(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload;
			workload.eventType = WebSocketEventType::MESSAGE_CREATE;
			send(this->workloadTarget, workload);
			co_return;
		}

		fire_and_forget onMessageDelete(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload;
			workload.eventType = WebSocketEventType::MESSAGE_DELETE;
			send(this->workloadTarget, workload);
			co_return;
		}

		fire_and_forget onMessageReactionAdd(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload;
			workload.eventType = WebSocketEventType::REACTION_ADD;
			send(this->workloadTarget, workload);
			co_return;
		}

		fire_and_forget onGuildMemberAdd(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload;
			workload.eventType = WebSocketEventType::GUILD_MEMBER_ADD;
			send(this->workloadTarget, workload);
			co_return;
		}

		void onMessageReceived(json payload) {
			try {

				if (payload.at("t") == "MESSAGE_CREATE") {
					onMessageCreate(payload.at("d"));
				}

				if (payload.at("t") == "MESSAGE_DELETE") {
					onMessageDelete(payload.at("d"));
				}

				if (payload.at("t") == "GUILD_CREATE") {
					onGuildCreate(payload.at("d"));
				}

				if (payload.at("t") == "MESSAGE_REACTION_ADD") {
					onMessageReactionAdd(payload.at("d"));
				}

				if (payload.at("t") == "GUILD_MEMBER_ADD") {
					onGuildMemberAdd(payload.at("d"));
				}
			}
			catch (winrt::hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;

			}
		}

	};

	struct WebSocketConnectionAgent :agent, implements<WebSocketConnectionAgent, winrt::Windows::Foundation::IInspectable> {
	public:

		WebSocketConnectionAgent(ITarget<json>& target, ThreadContext threadContextNew = {nullptr ,nullptr, nullptr})
			:
			agent(*threadContext.scheduler),
			webSocketMessageTarget(target)
		{
			this->threadContext = threadContext;
		}

		void initialize(hstring botTokenNew) {
			this->botToken = botTokenNew;
		}

		hstring* returnSocketPathPointer(){
			return &this->socketPath;
		}

		void terminate() {
			done();
		}

		~WebSocketConnectionAgent() {
			this->cleanup();
		}

	protected:
		ThreadContext threadContext;
		event_token messageReceivedToken;
		event_token closedToken;
		MessageWebSocket webSocket;
		DataWriter messageWriter;
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

		void run() {
			this->connect();
		}

		void cleanup() {
			this->terminate();

			if (this->messageWriter != nullptr) {
				this->messageWriter.DetachStream();
				this->messageWriter.Close();
				this->messageWriter = nullptr;
			}

			if (this->webSocket != nullptr) {
				this->webSocket.Close(1000, to_hstring("Closed due to user request."));
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
			}
		}

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args) {
			wcout << L"WebSocket_Closed; Code: " << args.Code() << ", Reason: " << args.Reason().c_str() << endl;
		}

		task<void> sendAsync(string& text) {

			string message = text;
			if (message.empty()) {
				cout << "Please specify text to send" << endl;
				co_return;
			}

			cout << "Sending Message: ";
			cout << message << endl;

			// Buffer any data we want to send.
			this->messageWriter.WriteString(to_hstring(message));

			try {
				// Send the data as one complete message.
				this->messageWriter.StoreAsync().get();

			}
			catch (hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;
				co_return;
			}

			cout << "Send Complete" << endl;
		}

		void OnHeartbeat(IInspectable const& sender, IInspectable const& args) {
			this->sendHeartBeat();
		}

		fire_and_forget sendHeartBeat() {
			try {
				if (this->didWeReceiveHeartbeatAck == false) {
					this->cleanup();
					this->connect();
					co_return;
				}
				string heartbeat = getHeartbeatPayload(this->lastNumberReceived);
				this->sendAsync(heartbeat).get();
				this->didWeReceiveHeartbeatAck = false;
			}
			catch (hresult_error error) {
				wcout << error.message().c_str() << endl;
			}
		}

		void onMessageReceived(MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs const& args) {
			try {
				DataReader dataReader{ args.GetDataReader() };
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				auto message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
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

				if (payload.at("op") == 6) {
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->sendAsync(resume);
				}

				if (payload.at("op") == 7) {
					this->cleanup();
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->sendAsync(resume);
					this->connect();
				}

				if (payload.at("op") == 9) {
					string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
					this->sendAsync(resume);
					this->connect();
				}

				if (payload.at("t") == "READY") {
					string sessionIDTemp;
					sessionIDTemp = payload.at("d").at("session_id");
					this->sessionID = to_hstring(sessionIDTemp);
				}

				if (payload.at("op") == 1) {
					this->sendHeartBeat();
				}

				if (payload.at("op") == 11) {
					this->didWeReceiveHeartbeatAck = true;
				}

				if (payload.at("op") == 10) {
					this->heartbeatInterval = payload.at("d").at("heartbeat_interval");
					this->heartbeatTimer.Interval(chrono::milliseconds(this->heartbeatInterval));
					this->heartbeatTimer.Tick({ this, &WebSocketConnectionAgent::OnHeartbeat });
					this->heartbeatTimer.Start();
					std::string identity = getIdentifyPayload(to_string(this->botToken), this->intentsValue);
					this->sendAsync(identity);
				}
				cout << "Message received from MessageWebSocket: " << to_string(message) << endl << endl;
			}
			catch (hresult_error const& ex) {
				wcout << ex.message().c_str() << endl;
			}
		};
	};

}
#endif