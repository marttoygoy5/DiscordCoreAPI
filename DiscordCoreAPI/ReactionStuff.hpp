// ReactionStuff.hpp - Reaction related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_STUFF_
#define _REACTION_STUFF_

#include "pch.h"
#include "MessageStuff.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class Channel;

	class Guild;

	class Message;

	class ReactionManager;
	
	class Reaction {
	public:

		DiscordCoreInternal::ReactionData data;

		Reaction() {}

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;

		DiscordCoreInternal::HttpAgentResources agentResources;

		Reaction(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ReactionData reactionData) {
			this->agentResources = agentResourcesNew;
			this->data = reactionData;
		}
	};

	struct CreateReactionData {
		string emojiName;
		string emojiId;
	};

	struct PutReactionData {
		string emojiName;
		string emojiId;
	};

	struct DeleteUserReactionData {
		string emojiName;
		string emojiId;
		string userId;
	};

	struct DeleteOwnReactionData {
		string emojiName;
		string emojiId;
	};

	struct DeleteAllReactionsByEmojiData {
		string emojiName;
		string emojiId;
	};

	struct DeleteAllReactionsData {};

	class ReactionManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<DiscordCoreInternal::PutReactionData>* requestPutBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* requestDeleteBuffer;
		static unbounded_buffer<Reaction>* outBuffer;
		static concurrent_queue<Reaction> reactionsToInser;
		static overwrite_buffer<map<string, Reaction>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreAPI::ReactionManager* pReactionManager{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreAPI::ReactionManager* pReactionManagerNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, Scheduler* pScheduler)
			:agent(*pScheduler)
		{
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->pReactionManager = pReactionManagerNew;
		}

		static task<void> initialize() {
			ReactionManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>;
			ReactionManagerAgent::requestPutBuffer = new unbounded_buffer<DiscordCoreInternal::PutReactionData>;
			ReactionManagerAgent::outBuffer = new unbounded_buffer<Reaction>;
			co_return;
		}

		task<void> putObjectAsync(DiscordCoreInternal::PutReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
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

		task<void> deleteObjectAsync(DiscordCoreInternal::DeleteReactionDataAll dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::SELF_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/@me";
			}
			else if(dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE){
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji;
			}
			else if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::ALL_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
			}
			else if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::USER_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/" + dataPackage.userId;
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
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
			DiscordCoreInternal::PutReactionData dataPackage;
			if (try_receive(ReactionManagerAgent::requestPutBuffer, dataPackage)) {
				this->putObjectAsync(dataPackage).get();
				DiscordCoreInternal::ReactionData reactionData;
				send(ReactionManagerAgent::outBuffer, Reaction(this->agentResources, reactionData));
			}
			DiscordCoreInternal::DeleteReactionDataAll dataPackageDelete;
			if (try_receive(ReactionManagerAgent::requestDeleteBuffer, dataPackageDelete)) {
				this->deleteObjectAsync(dataPackageDelete).get();
			}
			done();
		}
	};

	class ReactionManager {
	public:

		string messageId, channelId;

		task<void> createReactionAsync(PutReactionData createReactionData){
			DiscordCoreInternal::PutReactionData putReactionData;
			putReactionData.channelId = this->channelId;
			putReactionData.messageId = this->messageId;
			putReactionData.agentResources = this->agentResources;
			putReactionData.threadContext = this->threads->at(4);
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
			ReactionManagerAgent reactionManagerAgent(putReactionData.agentResources, this, this->threads, this->threads->at(5).scheduler);
			send(ReactionManagerAgent::requestPutBuffer, putReactionData);
			reactionManagerAgent.start();
			Reaction reaction = receive(ReactionManagerAgent::outBuffer);
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteUserReactionAsync(DeleteUserReactionData deleteUserReactionData) {
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.userId = deleteUserReactionData.userId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::USER_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(7);
			string emoji;
			if (deleteUserReactionData.emojiId != string()) {
				emoji += ":" + deleteUserReactionData.emojiName + ":" + deleteUserReactionData.emojiId;
				
			}
			else {
				emoji = deleteUserReactionData.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			ReactionManagerAgent reactionManagerAgent(deleteReactionData.agentResources, this, this->threads, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteOwnReactionAsync(DeleteOwnReactionData deleteOwnReactionData) {
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::SELF_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(7);
			string emoji;
			if (deleteOwnReactionData.emojiId != string()) {
				emoji += ":" + deleteOwnReactionData.emojiName + ":" + deleteOwnReactionData.emojiId;
			}
			else {
				emoji = deleteOwnReactionData.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			deleteReactionData.encodedEmoji = output;
			ReactionManagerAgent reactionManagerAgent(deleteReactionData.agentResources, this, this->threads, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteReactionByEmojiAsync(DeleteAllReactionsByEmojiData deleteReactionDataOld) {
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(7);
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
			ReactionManagerAgent reactionManagerAgent(deleteReactionData.agentResources, this, this->threads, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

		task<void> deleteAllReactionsAsync(DeleteAllReactionsData deleteReactionDataOld) {
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = this->channelId;
			deleteReactionData.messageId = this->messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::ALL_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(7);
			ReactionManagerAgent reactionManagerAgent(deleteReactionData.agentResources, this, this->threads, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			co_return;
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads;

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, string channalIdNew, string messageIdNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->messageId = messageIdNew;
			this->channelId = channalIdNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::PutReactionData>* ReactionManagerAgent::requestPutBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* ReactionManagerAgent::requestDeleteBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInser;
	overwrite_buffer<map<string, Reaction>> ReactionManagerAgent::cache;
}
#endif
