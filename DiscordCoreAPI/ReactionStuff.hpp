// ReactionStuff.hpp - Reaction related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_STUFF_
#define _REACTION_STUFF_

#include "pch.h"
#include "MessageStuff.hpp"

namespace DiscordCoreAPI {

	class Guild;

	class Message;

	class ReactionManager;

	class Reaction {
	public:
		DiscordCoreInternal::ReactionData data;
		Guild* guild{ nullptr };
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

	enum class ReactionDeletionType {
		SELF_DELETE = 0,
		USER_DELETE = 1,
		EMOJI_DELETE = 2,
		ALL_DELETE = 3
	};

	struct PutReactionData {
		string channelId;
		string messageId;
		string emoji;
	};

	struct DeleteReactionData {
		string channelId;
		string messageId;
		string encodedEmoji;
		string userId;
	};

	struct DeleteAllReactionsByEmojiData {
		string channelId;
		string messageId;
		string encodedEmoji;
	};

	struct DeleteAllReactionsData {
		string channelId;
		string messageId;
	};

	struct DeleteOwnReactionData {
		string channelId;
		string messageId;
		string encodedEmoji;
	};

	class ReactionManagerAgent : public agent {
	protected:
		friend struct DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<PutReactionData>* requestPutBuffer;
		static unbounded_buffer<DeleteReactionData>* requestDeleteBuffer;
		static unbounded_buffer<DeleteAllReactionsByEmojiData>* requestDeleteAllByEmojiBuffer;
		static unbounded_buffer<DeleteAllReactionsData>* requestDeleteAllReactionsBuffer;
		static unbounded_buffer<DeleteOwnReactionData>* requestDeleteOwnReactionBuffer;
		static unbounded_buffer<Reaction>* outBuffer;
		static concurrent_queue<Reaction> reactionsToInser;
		static map<string, Reaction> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreAPI::ReactionManager* pReactionManager{ nullptr };
		Guild* pGuild{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ReactionManager* pReactionManagerNew, Guild* pGuildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew)
			:agent(*threadsNew->at(0).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pReactionManager = pReactionManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			ReactionManagerAgent::requestDeleteBuffer = new unbounded_buffer<DeleteReactionData>;
			ReactionManagerAgent::requestDeleteOwnReactionBuffer = new unbounded_buffer<DeleteOwnReactionData>;
			ReactionManagerAgent::requestDeleteAllReactionsBuffer = new unbounded_buffer<DeleteAllReactionsData>;
			ReactionManagerAgent::requestDeleteAllByEmojiBuffer = new unbounded_buffer<DeleteAllReactionsByEmojiData>;
			ReactionManagerAgent::requestPutBuffer = new unbounded_buffer<PutReactionData>;
			ReactionManagerAgent::outBuffer = new unbounded_buffer<Reaction>;
			co_return;
		}

		task<void> putObjectAsync(PutReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(4).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			co_return;
		}

		task<void> deleteObjectData(DeleteOwnReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(7).scheduler);
			requestAgent.start();
			send(requestAgent.workSubmissionBuffer, workload);
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			co_return;
		}

		void run() {
			try {
				PutReactionData dataPackage = receive(ReactionManagerAgent::requestPutBuffer, 1U);
				this->putObjectAsync(dataPackage).get();
				DiscordCoreInternal::ReactionData reactionData;
				send(ReactionManagerAgent::outBuffer, Reaction(this->agentResources, reactionData, this->pGuild));
			}
			catch (exception error) {}
			try {
				DeleteOwnReactionData dataPackage = receive(ReactionManagerAgent::requestDeleteOwnReactionBuffer, 1u);
				this->deleteObjectData(dataPackage).get();
			}
			catch (exception error) {}
			done();
		}

		~ReactionManagerAgent() {}
	};

	class ReactionManager {
	public:		

		Guild* guild;

		ReactionManager() {}

		task<void> createReactionAsync(DiscordCoreInternal::CreateReactionData createReactionData){
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			PutReactionData putReactionData;
			putReactionData.channelId = this->channelId;
			putReactionData.messageId = this->messageId;
			string emoji;
			if (createReactionData.id != string()) {
				emoji += ":" + createReactionData.name + ":" + createReactionData.id;
			}
			else {
				emoji = createReactionData.name;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			putReactionData.emoji = output;
			send(ReactionManagerAgent::requestPutBuffer, putReactionData);
			reactionManagerAgent.start();
			Reaction reaction = receive(ReactionManagerAgent::outBuffer);
			agent::wait(&reactionManagerAgent);
			co_await resume_foreground(*this->threads->at(0).threadQueue.get());
			co_return;
		}

		task<void> deleteOwnReactionAsync(DiscordCoreInternal::DeleteOwnReactionData createReactionData) {
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			DeleteOwnReactionData deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			string emoji;
			if (createReactionData.emojiId != string()) {
				emoji += ":" + createReactionData.emojiName + ":" + createReactionData.emojiId;
			}
			else {
				emoji = createReactionData.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			send(ReactionManagerAgent::requestDeleteOwnReactionBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;
		string messageId, channelId;

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, Guild* guildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, string channalIdNew, string messageIdNew) {
			this->agentResources = agentResourcesNew;
			this->guild = guildNew;
			this->threads = threadsNew;
			this->messageId = messageIdNew;
			this->channelId = channalIdNew;
		}
	};
	unbounded_buffer<PutReactionData>* ReactionManagerAgent::requestPutBuffer;
	unbounded_buffer<DeleteReactionData>* ReactionManagerAgent::requestDeleteBuffer;
	unbounded_buffer<DeleteAllReactionsByEmojiData>* ReactionManagerAgent::requestDeleteAllByEmojiBuffer;
	unbounded_buffer<DeleteAllReactionsData>* ReactionManagerAgent::requestDeleteAllReactionsBuffer;
	unbounded_buffer<DeleteOwnReactionData>* ReactionManagerAgent::requestDeleteOwnReactionBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInser;
	map<string, Reaction> ReactionManagerAgent::cache;
}
#endif
