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

	class Reaction {
	public:

		DiscordCoreInternal::ReactionData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Reaction() {};

		Reaction(DiscordCoreInternal::ReactionData reactionData, DiscordCoreClient* discordCoreClientNew) {
			this->data = reactionData;
			this->discordCoreClient = discordCoreClientNew;
		}

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;
	};

	struct CreateReactionData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};

	struct GetReactionData {
		string channelId;
		string messageId;
		string userId;
		string emojiName;
	};

	struct DeleteUserReactionData {
		string channelId;
		string messageId;
		string userId;
		string emojiName;
		string emojiId;
	};

	struct DeleteOwnReactionData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};
	
	struct DeleteReactionsByEmojiData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};

	struct DeleteAllReactionsData {
		string channelId;
		string messageId;
	};

	class ReactionManagerAgent : public agent {
	public:
		static overwrite_buffer<map<string, Reaction>> cache;
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<DiscordCoreInternal::PutReactionData>* requestPutReactionBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* requestDeleteReactionBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetReactionData>* requestGetReactionBuffer;
		static unbounded_buffer<Reaction>* outReactionBuffer;
		static concurrent_queue<Reaction> reactionsToInsert;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			ReactionManagerAgent::requestDeleteReactionBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>;
			ReactionManagerAgent::requestGetReactionBuffer = new unbounded_buffer<DiscordCoreInternal::GetReactionData>;
			ReactionManagerAgent::requestPutReactionBuffer = new unbounded_buffer<DiscordCoreInternal::PutReactionData>;
			ReactionManagerAgent::outReactionBuffer = new unbounded_buffer<Reaction>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		task<Reaction> putObjectDataAsync(DiscordCoreInternal::PutReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManagerAgent::putObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::putObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ReactionManagerAgent::putObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ReactionData reactionData;
			DiscordCoreInternal::parseObject(returnData.data, &reactionData);
			Reaction reaction(reactionData, this->discordCoreClient);
			co_return reaction;
		}

		task<void> deleteObjectDataAsync(DiscordCoreInternal::DeleteReactionDataAll dataPackage) {
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
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManagerAgent::deleteObjectAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::deleteObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ReactionManagerAgent::deleteObjectAsync() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			co_return;
		}

		void run() {
			try {
				DiscordCoreInternal::PutReactionData dataPackage01;
				if (try_receive(ReactionManagerAgent::requestPutReactionBuffer, dataPackage01)) {
					Reaction reaction = this->putObjectDataAsync(dataPackage01).get();
					send(ReactionManagerAgent::outReactionBuffer, reaction);
				}
				DiscordCoreInternal::DeleteReactionDataAll dataPackage02;
				if (try_receive(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackage02)) {
					this->deleteObjectDataAsync(dataPackage02).get();
				}
				DiscordCoreInternal::GetReactionData dataPackage03;
				if (try_receive(ReactionManagerAgent::requestGetReactionBuffer, dataPackage03)) {
					map<string, Reaction> cacheTemp;
					if (try_receive(ReactionManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.messageId + dataPackage03.userId + dataPackage03.emojiName)) {
							Reaction reaction = cacheTemp.at(dataPackage03.messageId + dataPackage03.userId + dataPackage03.emojiName);
							send(ReactionManagerAgent::outReactionBuffer, reaction);
						}
					}
				}
				DiscordCoreInternal::ReactionData dataPackage04;
				Reaction reaction(dataPackage04, this->discordCoreClient);
				while (ReactionManagerAgent::reactionsToInsert.try_pop(reaction)) {
					map<string, Reaction> cacheTemp;
					if (try_receive(ReactionManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name)) {
							cacheTemp.erase(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name);
						}
					}
					cacheTemp.insert(make_pair(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name, reaction));
					asend(ReactionManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& error) {
				send(errorBuffer, error);
			}
			done();
		}
	};

	class ReactionManager {
	public:

		task<Reaction> createReactionAsync(CreateReactionData dataPackage){
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PutReactionData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(5);
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.emoji = output;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threads, this->discordCoreClient, this->threads->at(4).scheduler);
			send(ReactionManagerAgent::requestPutReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::createReactionAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->discordCoreClient);
			try_receive(ReactionManagerAgent::outReactionBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<Reaction> getReactionAsync(GetReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetReactionData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.emojiName = dataPackage.emojiName;
			dataPackageNew.userId = dataPackage.userId;
			ReactionManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			send(ReactionManagerAgent::requestGetReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::getReactionAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->discordCoreClient);
			try_receive(ReactionManagerAgent::outReactionBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::USER_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
				
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteUserReactionAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::SELF_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteOwnReactionAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteReactionsByEmojiAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.threadContext = this->threads->at(9);
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent requestAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteAllReactionsAsync() Error: " << error.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> insertReactionAsync(Reaction reaction) {
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->discordCoreClient, this->threads->at(2).scheduler);
			ReactionManagerAgent::reactionsToInsert.push(reaction);
			reactionManagerAgent.start();
			agent::wait(&reactionManagerAgent);
			exception error;
			while (reactionManagerAgent.getError(error)) {
				cout << "ReactionManager::insertReactionAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeReactionAsync(string messageId, string userId, string emojiName) {
			apartment_context mainThread;
			co_await resume_background();
			map<string, Reaction> cache;
			try_receive(ReactionManagerAgent::cache, cache);
			if (cache.contains(messageId + userId +emojiName)) {
				cache.erase(messageId + userId + emojiName);
			}
			asend(ReactionManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		friend class DiscordCoreClient;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->discordCoreClient = discordCoreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::PutReactionData>* ReactionManagerAgent::requestPutReactionBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* ReactionManagerAgent::requestDeleteReactionBuffer;
	unbounded_buffer<DiscordCoreInternal::GetReactionData>* ReactionManagerAgent::requestGetReactionBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outReactionBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInsert;
	overwrite_buffer<map<string, Reaction>> ReactionManagerAgent::cache;
}
#endif
