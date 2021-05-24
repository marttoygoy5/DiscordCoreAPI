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
	
	class ReactionManager;

	class DiscordCoreClient;

	class Reaction {
	public:

		DiscordCoreInternal::ReactionData data;
		DiscordCoreClient* coreClient{ nullptr };

		Reaction() {};

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;

		Reaction(DiscordCoreInternal::ReactionData reactionData, DiscordCoreClient* coreClientNew) {
			this->data = reactionData;
			this->coreClient = coreClientNew;
		}
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
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<DiscordCoreInternal::PutReactionData>* requestPutBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* requestDeleteBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetReactionData>* requestGetBuffer;
		static unbounded_buffer<Reaction>* outBuffer;
		static concurrent_queue<Reaction> reactionsToInsert;
		static overwrite_buffer<map<string, Reaction>> cache;

		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew, Scheduler* pScheduler)
			:agent(*pScheduler) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}

		static void initialize() {
			ReactionManagerAgent::requestDeleteBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>;
			ReactionManagerAgent::requestGetBuffer = new unbounded_buffer<DiscordCoreInternal::GetReactionData>;
			ReactionManagerAgent::requestPutBuffer = new unbounded_buffer<DiscordCoreInternal::PutReactionData>;
			ReactionManagerAgent::outBuffer = new unbounded_buffer<Reaction>;
			return;
		}

		task<Reaction> putObjectAsync(DiscordCoreInternal::PutReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources, dataPackage.threadContext.scheduler);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			try {
				agent::wait(&requestAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManagerAgent::putObjectAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::putObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
			}
			exception error;
			if (requestAgent.get_error(error)) {
				cout << "Error Message: " << error.what() << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			DiscordCoreInternal::parseObject(returnData.data, &reactionData);
			Reaction reaction(reactionData, this->coreClient);
			co_return reaction;
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
			try {
				agent::wait(&requestAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManagerAgent::deleteObjectAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::deleteObjectAsync() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl;
			}
			exception error;
			if (requestAgent.get_error(error)) {
				cout << "Error Message: " << error.what() << endl;
			}
			co_return;
		}

		void run() {
			DiscordCoreInternal::PutReactionData putData;
			if (try_receive(ReactionManagerAgent::requestPutBuffer, putData)) {
				Reaction reaction = this->putObjectAsync(putData).get();
				send(ReactionManagerAgent::outBuffer, reaction);
			}
			DiscordCoreInternal::DeleteReactionDataAll deleteData;
			if (try_receive(ReactionManagerAgent::requestDeleteBuffer, deleteData)) {
				this->deleteObjectAsync(deleteData).get();
			}
			DiscordCoreInternal::GetReactionData getData;
			if (try_receive(ReactionManagerAgent::requestGetBuffer, getData)) {
				map<string, Reaction> cacheTemp;
				if (try_receive(ReactionManagerAgent::cache, cacheTemp)) {
					if (cacheTemp.contains(getData.messageId + getData.userId + getData.emojiName)) {
						Reaction reaction = cacheTemp.at(getData.messageId + getData.userId + getData.emojiName);
						send(ReactionManagerAgent::outBuffer, reaction);
					}
				}
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->coreClient);
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
			done();
		}
	};

	class ReactionManager {
	public:

		task<Reaction> createReactionAsync(CreateReactionData createReactionData){
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::PutReactionData putReactionData;
			putReactionData.channelId = createReactionData.channelId;
			putReactionData.messageId = createReactionData.messageId;
			putReactionData.agentResources = this->agentResources;
			putReactionData.threadContext = this->threads->at(5);
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
			ReactionManagerAgent reactionManagerAgent(putReactionData.agentResources, this->threads, this->coreClient, this->threads->at(4).scheduler);
			send(ReactionManagerAgent::requestPutBuffer, putReactionData);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::createReactionAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->coreClient);
			try_receive(ReactionManagerAgent::outBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<Reaction> getReactionAsync(GetReactionData getReactionData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::GetReactionData getReactionDataNew;
			getReactionDataNew.channelId = getReactionData.channelId;
			getReactionDataNew.messageId = getReactionData.messageId;
			getReactionDataNew.emojiName = getReactionData.emojiName;
			getReactionDataNew.userId = getReactionData.userId;
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			send(ReactionManagerAgent::requestGetBuffer, getReactionDataNew);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::getReactionAsync() Error: " << e.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->coreClient);
			try_receive(ReactionManagerAgent::outBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DeleteUserReactionData deleteUserReactionData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = deleteUserReactionData.channelId;
			deleteReactionData.messageId = deleteUserReactionData.messageId;
			deleteReactionData.userId = deleteUserReactionData.userId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::USER_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(9);
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
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::deleteUserReactionAsync() Error: " << e.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteOwnReactionAsync(DeleteOwnReactionData deleteOwnReactionData) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = deleteOwnReactionData.channelId;
			deleteReactionData.messageId = deleteOwnReactionData.messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::SELF_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(9);
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
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::deleteOwnReactionAsync() Error: " << e.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData deleteReactionDataOld) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.channelId = deleteReactionDataOld.channelId;
			deleteReactionData.messageId = deleteReactionDataOld.messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(9);
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
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::deleteReactionsByEmojiAsync() Error: " << e.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> deleteAllReactionsAsync(DeleteAllReactionsData deleteReactionDataOld) {
			apartment_context mainThread;
			co_await resume_background();
			DiscordCoreInternal::DeleteReactionDataAll deleteReactionData;
			deleteReactionData.agentResources = this->agentResources;
			deleteReactionData.threadContext = this->threads->at(9);
			deleteReactionData.channelId = deleteReactionDataOld.channelId;
			deleteReactionData.messageId = deleteReactionDataOld.messageId;
			deleteReactionData.deletionType = DiscordCoreInternal::ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(8).scheduler);
			send(ReactionManagerAgent::requestDeleteBuffer, deleteReactionData);
			reactionManagerAgent.start();
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::deleteAllReactionsAsync() Error: " << e.what() << endl << endl;
			}
			co_await mainThread;
			co_return;
		}

		task<void> insertReactionAsync(Reaction reaction) {
			ReactionManagerAgent reactionManagerAgent(this->agentResources, this->threads, this->coreClient, this->threads->at(2).scheduler);
			reactionManagerAgent.start();
			ReactionManagerAgent::reactionsToInsert.push(reaction);
			try {
				agent::wait(&reactionManagerAgent);
			}
			catch (const exception& e) {
				cout << "ReactionManager::insertReactionAsync() Error: " << e.what() << endl << endl;
			}
			co_return;
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		friend class DiscordCoreClient;
		DiscordCoreInternal::HttpAgentResources agentResources;
		concurrent_vector<DiscordCoreInternal::ThreadContext>* threads{ nullptr };
		DiscordCoreClient* coreClient{ nullptr };

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, concurrent_vector<DiscordCoreInternal::ThreadContext>* threadsNew, DiscordCoreClient* coreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threads = threadsNew;
			this->coreClient = coreClientNew;
		}
	};
	unbounded_buffer<DiscordCoreInternal::PutReactionData>* ReactionManagerAgent::requestPutBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* ReactionManagerAgent::requestDeleteBuffer;
	unbounded_buffer<DiscordCoreInternal::GetReactionData>* ReactionManagerAgent::requestGetBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInsert;
	overwrite_buffer<map<string, Reaction>> ReactionManagerAgent::cache;
}
#endif
