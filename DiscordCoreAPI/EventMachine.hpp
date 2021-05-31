// EventMachine.hpp - Header for the events class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MACHINE_
#define _EVENT_MACHINE_

#include "pch.h"

namespace DiscordCoreAPI {

	struct OnGuildMemberAddData {
		DiscordCoreAPI::GuildMember guildMember;
	};

	struct OnMessageCreationData {
		DiscordCoreAPI::Message message;
	};

	struct OnMessageDeletionData {
		string messageId;
		string channelId;
		string guildId;
	};

	struct OnReactionAddData {
		DiscordCoreAPI::Reaction reaction;
	};

	struct OnGuildCreationData {
		DiscordCoreAPI::Guild guild;
	};

	struct OnInteractionCreateData {
		DiscordCoreAPI::InteractionData interactionData;
	};

	class EventMachine  {
	public:

		winrt::event_token onInteractionCreate(winrt::delegate<OnInteractionCreateData> const& handler) {
			return onInteractionCreateEvent.add(handler);
		}

		void onInteractionCreate(winrt::event_token const& token) {
			onInteractionCreateEvent.remove(token);
		}

		winrt::event_token onGuildCreation(winrt::delegate<OnGuildCreationData> const& handler) {
			return onGuildCreationEvent.add(handler);
		}

		void onGuildCreation(winrt::event_token const& token) {
			onGuildCreationEvent.remove(token);
		}

		winrt::event_token onGuildMemberAdd(winrt::delegate<OnGuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(winrt::event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		winrt::event_token onMessageCreation(winrt::delegate<OnMessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(winrt::event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

		winrt::event_token onMessageDeletion(winrt::delegate<OnMessageDeletionData> const& handler) {
			return onMessageDeletionEvent.add(handler);
		}

		void onMessageDeletion(winrt::event_token const& token) {
			onMessageDeletionEvent.remove(token);
		}

		winrt::event_token onReactionAdd(winrt::delegate<OnReactionAddData> const& handler) {
			return onReactionAddEvent.add(handler);
		}

		void onReactionAdd(winrt::event_token const& token) {
			onReactionAddEvent.remove(token);
		}

	protected:

		friend class DiscordCoreClient;

		winrt::event<winrt::delegate<OnInteractionCreateData>> onInteractionCreateEvent;

		winrt::event<winrt::delegate<OnGuildCreationData>> onGuildCreationEvent;

		winrt::event <winrt::delegate<OnMessageCreationData>> onMessageCreationEvent;

		winrt::event <winrt::delegate<OnMessageDeletionData>> onMessageDeletionEvent;

		winrt::event <winrt::delegate<OnGuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event <winrt::delegate<OnReactionAddData>> onReactionAddEvent;
	};
}
#endif
