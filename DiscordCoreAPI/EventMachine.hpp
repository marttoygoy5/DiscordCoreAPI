// EventMachine.hpp - Header for the events class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "ClientDataTypes.hpp"
#include "EventDataTypes.hpp"

namespace CommanderNS {

	struct EventMachine : implements<EventMachine, IInspectable> {
	public:

		winrt::event_token onGuildMemberAdd(winrt::delegate<EventDataTypes::GuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(winrt::event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		winrt::event_token onMessageCreation(winrt::delegate<EventDataTypes::MessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(winrt::event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

	protected:
		friend struct WebSocket;

		winrt::event<winrt::delegate<EventDataTypes::MessageCreationData>> onMessageCreationEvent;

		winrt::event<winrt::delegate<EventDataTypes::GuildMemberAddData>> onGuildMemberAddEvent;
	};

}