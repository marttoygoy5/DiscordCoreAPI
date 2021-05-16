// ReactionStuff.hpp - Reaction related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_STUFF_
#define _REACTION_STUFF_

#include "pch.h"

namespace DiscordCoreAPI{

	class Guild;

	class Reaction {
	public:
		DiscordCoreInternal::ReactionData data;
		Guild* guild;
		Reaction() {}
		Reaction(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ReactionData reactionData, Guild* guildNew) {
			this->agentResources = agentResourcesNew;
			this->data = reactionData;
			this->guild = guildNew;
		}
		~Reaction() {}
	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
	};

	class ReactionManager: public implements<ReactionManager, winrt::Windows::Foundation::IInspectable> {
	public:
		Guild* guild;
		ReactionManager() {}
		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
		}

		task<void> createReactionAsync(DiscordCoreInternal::CreateReactionData createReactionData){

			co_return;
		}

		~ReactionManager() {}
	protected:
		DiscordCoreInternal::HttpAgentResources agentResources;
	};

}
#endif
