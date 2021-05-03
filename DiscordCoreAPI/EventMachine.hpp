// EventMachine.hpp - Header for the events class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MACHINE_
#define _EVENT_MACHINE_

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "EventDataTypes.hpp"

namespace CommanderNS {

	struct EventMachine : implements<EventMachine, IInspectable> {
	public:

		IAsyncAction Initialize() {
			co_await resume_background();
			this->onMsgCreateQueueController = DispatcherQueueController::CreateOnDedicatedThread();
			this->onMsgCreateQueue = this->onMsgCreateQueueController.DispatcherQueue();
		}

		winrt::event_token onGuildMemberAdd(winrt::delegate<CommanderNS::EventDataTypes::GuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(winrt::event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		winrt::event_token onMessageCreation(winrt::delegate<CommanderNS::EventDataTypes::MessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(winrt::event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

		DispatcherQueueController onMsgCreateQueueController{ nullptr };
		DispatcherQueue onMsgCreateQueue{ nullptr };

	protected:
		friend struct WebSocket;

		winrt::event < winrt::delegate<CommanderNS::EventDataTypes::MessageCreationData>> onMessageCreationEvent;

		winrt::event < winrt::delegate<CommanderNS::EventDataTypes::GuildMemberAddData>> onGuildMemberAddEvent;
	};
}
#endif