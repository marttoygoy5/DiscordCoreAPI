// EventMachine.hpp - Header for the events class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MACHINE_
#define _EVENT_MACHINE_

#include "pch.h"
#include "GuildStuff.hpp"

namespace DiscordCoreAPI {
	
	struct GuildMemberAddData {
		DiscordCoreAPI::GuildMember guildMember;
	};

	struct MessageCreationData {
		DiscordCoreAPI::Message message;
	};

	struct MessageDeletionData {
		string messageId;
		string channelId;
		string guildId;
	};

	struct ReactionAddData {
		DiscordCoreAPI::Reaction reaction;
	};

	struct GuildCreationData {
		DiscordCoreAPI::Guild guild;
	};

	struct EventMachine : implements<EventMachine, winrt::Windows::Foundation::IInspectable> {
	public:

		winrt::event_token onGuildCreation(winrt::delegate<GuildCreationData> const& handler) {
			return onGuildCreationEvent.add(handler);
		}

		void onGuildCreation(winrt::event_token const& token) {
			onGuildCreationEvent.remove(token);
		}

		winrt::event_token onGuildMemberAdd(winrt::delegate<GuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(winrt::event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		winrt::event_token onMessageCreation(winrt::delegate<MessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(winrt::event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

		winrt::event_token onMessageDeletion(winrt::delegate<MessageDeletionData> const& handler) {
			return onMessageDeletionEvent.add(handler);
		}

		void onMessageDeletion(winrt::event_token const& token) {
			onMessageDeletionEvent.remove(token);
		}

		winrt::event_token onReactionAdd(winrt::delegate<ReactionAddData> const& handler) {
			return onReactionAddEvent.add(handler);
		}

		void onReactionAdd(winrt::event_token const& token) {
			onReactionAddEvent.remove(token);
		}

	protected:

		friend struct DiscordCoreClient;

		winrt::event<winrt::delegate<GuildCreationData>> onGuildCreationEvent;

		winrt::event <winrt::delegate<MessageCreationData>> onMessageCreationEvent;

		winrt::event <winrt::delegate<MessageDeletionData>> onMessageDeletionEvent;

		winrt::event <winrt::delegate<GuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event <winrt::delegate<ReactionAddData>> onReactionAddEvent;
	};
}
#endif
