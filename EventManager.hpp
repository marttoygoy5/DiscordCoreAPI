// EventManager.hpp - Header for the winrt::event-manager class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_MANAGER_
#define _EVENT_MANAGER_

#include "pch.h"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

	struct OnChannelCreationData {
		Channel channel;
	};

	struct OnChannelUpdateData {
		Channel channelOld;
		Channel channelNew;
	};

	struct OnChannelDeletionData {
		Channel channel;
	};

	struct OnGuildCreationData {
		Guild guild;
	};

	struct OnGuildUpdateData {
		Guild guildOld;
		Guild guildNew;
	};

	struct OnGuildDeletionData {
		Guild guild;
	};

	struct OnGuildBanAddData {
		string guildId;
		User user;
	};

	struct OnGuildBanRemoveData {
		string guildId;
		User user;
	};

	struct OnGuildMemberAddData {
		GuildMember guildMember;
	};

	struct OnGuildMemberRemoveData {
		User user;
		string guildId;
	};

	struct OnGuildMemberUpdateData {
		GuildMember guildMemberOld;
		GuildMember guildMemberNew;
	};

	struct OnRoleCreationData {
		Role role;
		string guildId;
	};

	struct OnRoleUpdateData {
		Role roleOld;
		Role roleNew;
		string guildId;
	};

	struct OnRoleDeletionData {
		Role roleOld;
		string guildId;
	};

	struct OnInviteCreationData {
		InviteData invite;
	};

	struct OnInviteDeletionData {
		string channelId;
		string guildId;
		string code;
	};

	struct OnMessageCreationData {
		Message message;
	};

	struct OnMessageUpdateData {
		Message messageOld;
		Message messageNew;
	};

	struct OnMessageDeletionData {
		DiscordCoreClient* discordCoreClient;
		string messageId;
		string channelId;
		string guildId;
	};

	struct OnReactionAddData {
		Reaction reaction;
	};

	struct OnInputEventCreationData {
		InputEventData eventData;
	};

	class EventManager  {
	public:

		event_token onChannelCreation(delegate<OnChannelCreationData> const& handler) {
			return onChannelCreationEvent.add(handler);
		}

		void onChannelCreation(event_token const& token) {
			onChannelCreationEvent.remove(token);
		}

		event_token onChannelUpdate(delegate<OnChannelUpdateData> const& handler) {
			return onChannelUpdateEvent.add(handler);
		}

		void onChannelUpdate(event_token const& token) {
			onChannelUpdateEvent.remove(token);
		}

		event_token onChannelDeletion(delegate<OnChannelDeletionData> const& handler) {
			return onChannelDeletionEvent.add(handler);
		}

		void onChannelDeletion(event_token const& token) {
			onChannelDeletionEvent.remove(token);
		}

		event_token onGuildCreation(delegate<OnGuildCreationData> const& handler) {
			return onGuildCreationEvent.add(handler);
		}

		void onGuildCreation(event_token const& token) {
			onGuildCreationEvent.remove(token);
		}

		event_token onGuildUpdate(delegate<OnGuildUpdateData> const& handler) {
			return onGuildUpdateEvent.add(handler);
		}

		void onGuildUpdate(event_token const& token) {
			onGuildUpdateEvent.remove(token);
		}

		event_token onGuildDeletion(delegate<OnGuildDeletionData> const& handler) {
			return onGuildDeletionEvent.add(handler);
		}

		void onGuildDeletion(event_token const& token) {
			onGuildDeletionEvent.remove(token);
		}

		event_token onGuildBanAdd(delegate<OnGuildBanAddData> const& handler) {
			return onGuildBanAddEvent.add(handler);
		}

		void onGuildBanAdd(event_token const& token) {
			onGuildBanAddEvent.remove(token);
		}

		event_token onGuildBanRemove(delegate<OnGuildBanRemoveData> const& handler) {
			return onGuildBanRemoveEvent.add(handler);
		}

		void onGuildBanRemove(event_token const& token) {
			onGuildBanRemoveEvent.remove(token);
		}

		event_token onGuildMemberAdd(delegate<OnGuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		event_token onGuildMemberRemove(delegate<OnGuildMemberRemoveData> const& handler) {
			return onGuildMemberRemoveEvent.add(handler);
		}

		void onGuildMemberRemove(event_token const& token) {
			onGuildMemberRemoveEvent.remove(token);
		}

		event_token onGuildMemberUpdate(delegate<OnGuildMemberUpdateData> const& handler) {
			return onGuildMemberUpdateEvent.add(handler);
		}

		void onGuildMemberUpdate(event_token const& token) {
			onGuildMemberUpdateEvent.remove(token);
		}

		event_token onRoleCreation(delegate<OnRoleCreationData> const& handler) {
			return onRoleCreationEvent.add(handler);
		}

		void onRoleCreation(event_token const& token) {
			onRoleCreationEvent.remove(token);
		}

		event_token onRoleUpdate(delegate<OnRoleUpdateData> const& handler) {
			return onRoleUpdateEvent.add(handler);
		}

		void onRoleUpdate(event_token const& token) {
			onRoleUpdateEvent.remove(token);
		}

		event_token onRoleDeletion(delegate<OnRoleDeletionData> const& handler) {
			return onRoleDeletionEvent.add(handler);
		}

		void onRoleDeletion(event_token const& token) {
			onRoleDeletionEvent.remove(token);
		}

		event_token onInviteCreation(delegate<OnInviteCreationData> const& handler) {
			return onInviteCreationEvent.add(handler);
		}

		void onInviteCreation(event_token const& token) {
			onInviteCreationEvent.remove(token);
		}

		event_token onInviteDeletion(delegate<OnInviteDeletionData> const& handler) {
			return onInviteDeletionEvent.add(handler);
		}

		void onInviteDeletion(event_token const& token) {
			onInviteDeletionEvent.remove(token);
		}

		event_token onMessageCreation(delegate<OnMessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

		event_token onMessageUpdate(delegate<OnMessageUpdateData> const& handler) {
			return onMessageUpdateEvent.add(handler);
		}

		void onMessageUpdate(event_token const& token) {
			onMessageUpdateEvent.remove(token);
		}

		event_token onMessageDeletion(delegate<OnMessageDeletionData> const& handler) {
			return onMessageDeletionEvent.add(handler);
		}

		void onMessageDeletion(event_token const& token) {
			onMessageDeletionEvent.remove(token);
		}

		event_token onInputEventCreation(delegate<OnInputEventCreationData> const& handler) {
			return onInputEventCreationEvent.add(handler);
		}

		void onInputEventCreation(event_token const& token) {
			onInputEventCreationEvent.remove(token);
		}

		event_token onReactionAdd(delegate<OnReactionAddData> const& handler) {
			return onReactionAddEvent.add(handler);
		}

		void onReactionAdd(event_token const& token) {
			onReactionAddEvent.remove(token);
		}

	protected:

		friend class DiscordCoreClient;

		winrt::event<delegate<OnChannelCreationData>> onChannelCreationEvent;

		winrt::event<delegate<OnChannelUpdateData>> onChannelUpdateEvent;

		winrt::event<delegate<OnChannelDeletionData>> onChannelDeletionEvent;

		winrt::event<delegate<OnGuildCreationData>> onGuildCreationEvent;

		winrt::event<delegate<OnGuildUpdateData>> onGuildUpdateEvent;

		winrt::event<delegate<OnGuildDeletionData>> onGuildDeletionEvent;

		winrt::event<delegate<OnGuildBanAddData>> onGuildBanAddEvent;

		winrt::event<delegate<OnGuildBanRemoveData>> onGuildBanRemoveEvent;

		winrt::event <delegate<OnGuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event <delegate<OnGuildMemberRemoveData>> onGuildMemberRemoveEvent;

		winrt::event <delegate<OnGuildMemberUpdateData>> onGuildMemberUpdateEvent;

		winrt::event<delegate<OnRoleCreationData>>onRoleCreationEvent;

		winrt::event<delegate<OnRoleUpdateData>>onRoleUpdateEvent;

		winrt::event<delegate<OnRoleDeletionData>>onRoleDeletionEvent;

		winrt::event<delegate<OnInviteCreationData>> onInviteCreationEvent;

		winrt::event<delegate<OnInviteDeletionData>> onInviteDeletionEvent;

		winrt::event<delegate<OnMessageCreationData>> onMessageCreationEvent;

		winrt::event<delegate<OnMessageUpdateData>> onMessageUpdateEvent;

		winrt::event <delegate<OnMessageDeletionData>> onMessageDeletionEvent;

		winrt::event<delegate<OnInputEventCreationData>> onInputEventCreationEvent;

		winrt::event <delegate<OnReactionAddData>> onReactionAddEvent;
	};
}
#endif
