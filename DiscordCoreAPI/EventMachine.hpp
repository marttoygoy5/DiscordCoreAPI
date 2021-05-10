// EventMachine.hpp - Header for the events class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MACHINE_
#define _EVENT_MACHINE_

#include "pch.h"
#include "SystemThreads.hpp"
#include "ClientDataTypes.hpp"
#include "EventDataTypes.hpp"

namespace CommanderNS {

	struct EventMachine : implements<EventMachine, winrt::Windows::Foundation::IInspectable> {
	public:

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

		winrt::event_token onMessageDeletion(winrt::delegate<CommanderNS::EventDataTypes::MessageDeletionData> const& handler) {
			return onMessageDeletionEvent.add(handler);
		}

		void onMessageDeletion(winrt::event_token const& token) {
			onMessageDeletionEvent.remove(token);
		}

		winrt::event_token onReactionAdd(winrt::delegate<CommanderNS::EventDataTypes::ReactionAddData> const& handler) {
			return onReactionAddEvent.add(handler);
		}

		void onReactionAdd(winrt::event_token const& token) {
			onReactionAddEvent.remove(token);
		}

	protected:
		friend struct WebSocket;
		friend struct WebSocketConnection;
		friend struct WebSocketReceiver;

		winrt::event <winrt::delegate<CommanderNS::EventDataTypes::MessageCreationData>> onMessageCreationEvent;

		winrt::event <winrt::delegate<CommanderNS::EventDataTypes::MessageDeletionData>> onMessageDeletionEvent;

		winrt::event <winrt::delegate<CommanderNS::EventDataTypes::GuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event <winrt::delegate<CommanderNS::EventDataTypes::ReactionAddData>> onReactionAddEvent;
	};
}
#endif
