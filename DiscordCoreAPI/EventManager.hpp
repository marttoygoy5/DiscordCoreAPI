// EventManager.hpp - Header for the event-manager class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MANAGER_
#define _EVENT_MANAGER_

#include "pch.h"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase;

	struct OnChannelCreationData {
		Channel channel;
	};

	struct OnChannelUpdateData {
		Channel channel;
	};

	struct OnChannelDeletionData {
		Channel channel;
	};

	struct OnGuildCreationData {
		Guild guild;
	};

	struct OnGuildUpdateData {
		Guild guild;
	};

	struct OnGuildDeletionData {
		Guild guild;
	};

	struct OnGuildMemberAddData {
		GuildMember guildMember;
	};

	struct OnMessageCreationData {
		Message message;
	};

	struct OnMessageDeletionData {
		string messageId;
		string channelId;
		string guildId;
	};

	struct OnReactionAddData {
		Reaction reaction;
	};

	struct OnInputEventCreateData {
		InputEventData eventData;
	};

	class EventManager  {
	public:

		winrt::event_token onChannelCreation(winrt::delegate<OnChannelCreationData> const& handler) {
			return onChannelCreationEvent.add(handler);
		}

		void onChannelCreation(winrt::event_token const& token) {
			onChannelCreationEvent.remove(token);
		}

		winrt::event_token onChannelUpdate(winrt::delegate<OnChannelUpdateData> const& handler) {
			return onChannelUpdateEvent.add(handler);
		}

		void onChannelUpdate(winrt::event_token const& token) {
			onChannelUpdateEvent.remove(token);
		}

		winrt::event_token onChannelDeletion(winrt::delegate<OnChannelDeletionData> const& handler) {
			return onChannelDeletionEvent.add(handler);
		}

		void onChannelDeletion(winrt::event_token const& token) {
			onChannelDeletionEvent.remove(token);
		}

		winrt::event_token onGuildCreation(winrt::delegate<OnGuildCreationData> const& handler) {
			return onGuildCreationEvent.add(handler);
		}

		void onGuildCreation(winrt::event_token const& token) {
			onGuildCreationEvent.remove(token);
		}

		winrt::event_token onGuildUpdate(winrt::delegate<OnGuildUpdateData> const& handler) {
			return onGuildUpdateEvent.add(handler);
		}

		void onGuildUpdate(winrt::event_token const& token) {
			onGuildUpdateEvent.remove(token);
		}

		winrt::event_token onGuildDeletion(winrt::delegate<OnGuildDeletionData> const& handler) {
			return onGuildDeletionEvent.add(handler);
		}

		void onGuildDeletion(winrt::event_token const& token) {
			onGuildDeletionEvent.remove(token);
		}

		winrt::event_token onInputEventCreate(winrt::delegate<OnInputEventCreateData> const& handler) {
			return onInputEventCreationEvent.add(handler);
		}

		void onInputEventCreate(winrt::event_token const& token) {
			onInputEventCreationEvent.remove(token);
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

		winrt::event<winrt::delegate<OnChannelCreationData>> onChannelCreationEvent;

		winrt::event<winrt::delegate<OnChannelUpdateData>> onChannelUpdateEvent;

		winrt::event<winrt::delegate<OnChannelDeletionData>> onChannelDeletionEvent;

		winrt::event<winrt::delegate<OnGuildCreationData>> onGuildCreationEvent;

		winrt::event<winrt::delegate<OnGuildUpdateData>> onGuildUpdateEvent;

		winrt::event<winrt::delegate<OnGuildDeletionData>> onGuildDeletionEvent;

		winrt::event<winrt::delegate<OnInputEventCreateData>> onInputEventCreationEvent;

		winrt::event <winrt::delegate<OnMessageCreationData>> onMessageCreationEvent;

		winrt::event <winrt::delegate<OnMessageDeletionData>> onMessageDeletionEvent;

		winrt::event <winrt::delegate<OnGuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event <winrt::delegate<OnReactionAddData>> onReactionAddEvent;
	};
}
#endif
