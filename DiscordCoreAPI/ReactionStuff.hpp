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

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;

		DiscordCoreInternal::HttpAgentResources agentResources;

		Reaction(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ReactionData reactionData, Guild* guildNew) {
			this->agentResources = agentResourcesNew;
			this->data = reactionData;
			this->guild = guildNew;
		}
	};

	struct PutReactionData {
		string channelId;
		string messageId;
		string emoji;
	};

	enum class ReactionDeletionType {
		SELF_DELETE = 0,
		USER_DELETE = 1,
		EMOJI_DELETE = 2,
		ALL_DELETE = 3
	};

	struct DeleteReactionDataAll {
		string channelId;
		string messageId;
		string encodedEmoji;
		string userId;
		ReactionDeletionType deletionType;
	};

	class ReactionManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<PutReactionData>* requestPutBuffer;
		static unbounded_buffer<DeleteReactionDataAll>* requestDeleteBuffer;
		static unbounded_buffer<Reaction>* outBuffer;
		static concurrent_queue<Reaction> reactionsToInser;
		static map<string, Reaction> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreAPI::ReactionManager* pReactionManager{ nullptr };
		Guild* pGuild{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ReactionManager* pReactionManagerNew, Guild* pGuildNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew)
			:agent(*threadsNew->at(8).scheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pReactionManager = pReactionManagerNew;
			this->pGuild = pGuildNew;
		}

		static task<void> initialize() {
			ReactionManagerAgent::requestDeleteBuffer = new unbounded_buffer<DeleteReactionDataAll>;
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
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			exception error;
			if (requestAgent.get_error(error)) {
				cout << "Error Message: " << error.what() << endl;
			}
			co_return;
		}

		task<void> deleteObjectAsync(DeleteReactionDataAll dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			if (dataPackage.deletionType == DiscordCoreAPI::ReactionDeletionType::SELF_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/@me";
			}
			else if(dataPackage.deletionType == DiscordCoreAPI::ReactionDeletionType::EMOJI_DELETE){
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji;
			}
			else if (dataPackage.deletionType == DiscordCoreAPI::ReactionDeletionType::ALL_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
			}
			else if (dataPackage.deletionType == DiscordCoreAPI::ReactionDeletionType::USER_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/" + dataPackage.userId;
			}			
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources, this->threads->at(7).scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			json jsonValue = receive(requestAgent.workReturnBuffer);
			agent::wait(&requestAgent);
			exception error;
			if (requestAgent.get_error(error)) {
				cout << "Error Message: " << error.what() << endl;
			}
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
				DeleteReactionDataAll dataPackage = receive(ReactionManagerAgent::requestDeleteBuffer, 1U);
				this->deleteObjectAsync(dataPackage).get();
			}
			catch (exception error) {}
			done();
		}
	};

	class ReactionManager {
	public:		

		Guild* guild;

		task<void> createReactionAsync(DiscordCoreInternal::CreateReactionData createReactionData){
			co_await resume_foreground(*this->threads->at(4).threadQueue.get());
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			PutReactionData putReactionData;
			putReactionData.channelId = this->channelId;
			putReactionData.messageId = this->messageId;
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
			putReactionData.emoji = output;
			send(ReactionManagerAgent::requestPutBuffer, putReactionData);
			reactionManagerAgent.start();
			Reaction reaction = receive(ReactionManagerAgent::outBuffer);
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteUserReactionAsync(DiscordCoreInternal::DeleteReactionData deleteReactionDataOld) {
			co_await resume_foreground(*this->threads->at(7).threadQueue.get());
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.userId = deleteReactionData.userId;
			deleteReactionData.deletionType = DiscordCoreAPI::ReactionDeletionType::USER_DELETE;
			string emoji;
			if (deleteReactionDataOld.emojiId != string()) {
				emoji += ":" + deleteReactionDataOld.emojiName + ":" + deleteReactionDataOld.emojiId;
				
			}
			else {
				emoji = deleteReactionDataOld.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteOwnReactionAsync(DiscordCoreInternal::DeleteOwnReactionData deleteReactionDataOld) {
			co_await resume_foreground(*this->threads->at(7).threadQueue.get());
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreAPI::ReactionDeletionType::SELF_DELETE;
			string emoji;
			if (deleteReactionDataOld.emojiId != string()) {
				emoji += ":" + deleteReactionDataOld.emojiName + ":" + deleteReactionDataOld.emojiId;
			}
			else {
				emoji = deleteReactionDataOld.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteReactionByEmojiAsync(DiscordCoreInternal::DeleteAllReactionsByEmojiData deleteReactionDataOld) {
			co_await resume_foreground(*this->threads->at(7).threadQueue.get());
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreAPI::ReactionDeletionType::EMOJI_DELETE;
			string emoji;
			if (deleteReactionDataOld.emojiId != string()) {
				emoji += ":" + deleteReactionDataOld.emojiName + ":" + deleteReactionDataOld.emojiId;
			}
			else {
				emoji = deleteReactionDataOld.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteAllReactionsAsync(DiscordCoreInternal::DeleteAllReactionsData deleteReactionDataOld) {
			cout << "TESTING HERE TESTING TESTING HERE" << endl;
			co_await resume_foreground(*this->threads->at(7).threadQueue.get());
			cout << "TESTING HERE TESTING TESTING HERE22222" << endl;
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this, this->guild, this->threads);
			DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreAPI::ReactionDeletionType::ALL_DELETE;
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			cout << "TESTING HERE TESTING TESTING HERE333333" << endl;
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
	unbounded_buffer<DeleteReactionDataAll>* ReactionManagerAgent::requestDeleteBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInser;
	map<string, Reaction> ReactionManagerAgent::cache;
}
#endif
