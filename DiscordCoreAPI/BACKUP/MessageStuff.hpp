// MessageStuff.hpp - Header for Message classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _MESSAGE_STUFF_
#define _MESSAGE_STUFF_

#include "pch.h"
#include "ReactionStuff.hpp"

namespace DiscordCoreAPI {

	class Message : public DiscordCoreInternal::MessageData{
	public:
		string content, guildId, channelId, messageId, userId, authorAvatarURL;
		Message(DiscordCoreInternal::MessageData messageData) {
			*this = messageData;
			delete& this->pinned;
		}
		
	protected:

	};

	class MessageManager {

	};
}
#endif
