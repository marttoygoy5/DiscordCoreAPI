// HttpStuff.hpp - Header for all of the HTTP related stuff.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_STUFF_
#define _HTTP_STUFF_

#include "pch.h"
#include "JSONifier.hpp"

bool executeByRateLimitData(DiscordCoreInternal::RateLimitData* rateLimitData) {
	if (rateLimitData->getsRemaining == 0) {
		float loopStartTime = rateLimitData->timeStartedAt;
		float targetTime = loopStartTime + rateLimitData->msRemain;
		float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
		float tryAgainElapsedTime = currentTime - rateLimitData->timeStartedAtTryAgain;
		if (rateLimitData->msRemainTryAgain <= 15000.0f && rateLimitData->msRemain == 0.0f) {
			targetTime = rateLimitData->timeStartedAtTryAgain + rateLimitData->msRemainTryAgain;
			rateLimitData->msRemain = rateLimitData->msRemainTryAgain;
		}
		else if (tryAgainElapsedTime < rateLimitData->msRemainTryAgain) {
			cout << "Waiting on rate-limit, Time Remainiing: " << rateLimitData->msRemainTryAgain - tryAgainElapsedTime << "ms." << endl;
			return true;
		}
		while (rateLimitData->msRemain > 0.0f) {
			currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			rateLimitData->msRemain = targetTime - currentTime;
		}
		return false;
	}
	return false;
}

namespace DiscordCoreInternal {

	class HttpRequestAgent :public agent {
	public:
		unbounded_buffer<HttpData> workReturnBuffer;
		unbounded_buffer<HttpWorkload> workSubmissionBuffer;
		HttpRequestAgent(HttpAgentResources agentResources, Scheduler* pScheduler = nullptr) 
			: agent(*pScheduler)
		{
			try {
				this->baseURL = agentResources.baseURL;
				this->botToken = agentResources.botToken;
				this->initialConnectionPath = this->baseURL + L"/gateway/bot";
				this->getHttpClient = HttpClient();
				this->getHeaders = this->getHttpClient.DefaultRequestHeaders();
				this->putHttpClient = HttpClient();
				this->putHeaders = this->putHttpClient.DefaultRequestHeaders();
				this->postHttpClient = HttpClient();
				this->postHeaders = this->postHttpClient.DefaultRequestHeaders();
				this->patchHttpClient = HttpClient();
				this->patchHeaders = this->patchHttpClient.DefaultRequestHeaders();
				this->deleteHttpClient = HttpClient();
				this->deleteHeaders = this->deleteHttpClient.DefaultRequestHeaders();
				hstring headerString = L"Bot ";
				hstring headerString2 = headerString + this->botToken;
				HttpCredentialsHeaderValue credentialValue(nullptr);
				credentialValue = credentialValue.Parse(headerString2.c_str());
				this->getHeaders.Authorization(credentialValue);
				this->putHeaders.Authorization(credentialValue);
				this->postHeaders.Authorization(credentialValue);
				this->patchHeaders.Authorization(credentialValue);
				this->deleteHeaders.Authorization(credentialValue);
				this->baseURI = Uri(this->initialConnectionPath.c_str());
				HttpResponseMessage httpResponse;
				httpResponse = getHttpClient.GetAsync(this->baseURI).get();
				hstring httpResponseBody = httpResponse.Content().ReadAsStringAsync().get().c_str();
				std::wstringstream stream;
				stream << DiscordCoreInternal::parseSocketPath(httpResponseBody.c_str()).c_str();
				stream << L"/?v=9&encoding=json";
				*agentResources.pSocketPath = stream.str();
			}
			catch (winrt::hresult_error ex) {
				std::wcout << "Error: " << ex.message().c_str() << std::endl;
			}
		}

		bool get_error(exception& e){
			return try_receive(errorBuffer, e);
		}

	protected:
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		single_assignment<exception> errorBuffer;

		void run() {
			try {
				transformer<HttpWorkload, HttpData> completeHttpRequest([this](HttpWorkload workload) -> HttpData{
					RateLimitData rateLimitData;
					rateLimitData.workloadType = workload.workloadType;
					auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
					if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
						string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
						rateLimitData = HttpRequestAgent::rateLimitData.at(bucket);
					}
					if (executeByRateLimitData(&rateLimitData)) {
						HttpData returnData;
						return returnData;
					}
					HttpData returnData;
					if (workload.workloadClass == HttpWorkloadClass::GET) {
						returnData = httpGETObjectDataAsync(workload.relativePath, &rateLimitData).get();
					}
					else if (workload.workloadClass == HttpWorkloadClass::POST) {
						returnData = httpPOSTObjectDataAsync(workload.relativePath, workload.content, &rateLimitData).get();
					}
					else if (workload.workloadClass == HttpWorkloadClass::PUT) {
						returnData = httpPUTObjectDataAsync(workload.relativePath, workload.content, &rateLimitData).get();
					}
					else if (workload.workloadClass == HttpWorkloadClass::PATCH) {
						returnData = httpPATCHObjectDataAsync(workload.relativePath, workload.content, &rateLimitData).get();
					}
					else if (workload.workloadClass == HttpWorkloadClass::DELETED) {
						returnData = httpDELETEObjectDataAsync(workload.relativePath, &rateLimitData).get();
					}
					auto bucketValueIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
					if (bucketValueIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
						HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
					}
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitData.bucket));
					auto rateLimitIterator = HttpRequestAgent::rateLimitData.find(rateLimitData.bucket);
					if (rateLimitIterator != end(HttpRequestAgent::rateLimitData)) {
						HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitData.bucket);
					}
					HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitData.bucket, rateLimitData));
					return returnData;
					});
				completeHttpRequest.link_target(&this->workReturnBuffer);
				HttpWorkload workload = receive(&this->workSubmissionBuffer);
				send(&completeHttpRequest, workload);
				done();
			}
			catch (exception error) {
				send(errorBuffer, error);
			}
		}

		task<HttpData> httpGETObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData getData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = getHttpClient.GetAsync(requestUri).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			float currentMSTimeTryAgainLocal;
			float msRemainTryAgainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			currentMSTimeTryAgainLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			getData.returnCode = (unsigned int)httpResponse.StatusCode();
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainTryAgainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
				msRemainLocal = 0.0f;
			}
			else {
				msRemainTryAgainLocal = 0;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			pRateLimitData->msRemainTryAgain = msRemainTryAgainLocal;
			pRateLimitData->timeStartedAtTryAgain = currentMSTimeTryAgainLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				executeByRateLimitData(pRateLimitData);
				getData = httpGETObjectDataAsync(relativeURL, pRateLimitData).get();
			}
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			getData.data = jsonValue;
			co_return getData;
		}

		task<HttpData> httpPUTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData putData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentHeaderCollection contentHeaderCollection;
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			contentHeaderCollection.ContentDisposition(headerValue);
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			contentHeaderCollection.ContentType(typeHeaderValue);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpResponseMessage httpResponse;
			httpResponse = putHttpClient.PutAsync(requestUri, contents).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			float currentMSTimeTryAgainLocal;
			float msRemainTryAgainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			currentMSTimeTryAgainLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			putData.returnCode = (unsigned int)httpResponse.StatusCode();
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainTryAgainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
				msRemainLocal = 0.0f;
			}
			else {
				msRemainTryAgainLocal = 0;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			pRateLimitData->msRemainTryAgain = msRemainTryAgainLocal;
			pRateLimitData->timeStartedAtTryAgain = currentMSTimeTryAgainLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				executeByRateLimitData(pRateLimitData);
				putData = httpPUTObjectDataAsync(relativeURL, content, pRateLimitData).get();
			}
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			putData.data = jsonValue;
			co_return putData;
		}

		task<HttpData> httpPOSTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData postData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentHeaderCollection contentHeaderCollection;
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			contentHeaderCollection.ContentDisposition(headerValue);
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			contentHeaderCollection.ContentType(typeHeaderValue);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpResponseMessage httpResponse;
			httpResponse = postHttpClient.PostAsync(requestUri, contents).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			float currentMSTimeTryAgainLocal;
			float msRemainTryAgainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			currentMSTimeTryAgainLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			postData.returnCode = (unsigned int)httpResponse.StatusCode();
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainTryAgainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
				msRemainLocal = 0.0f;
			}
			else {
				msRemainTryAgainLocal = 0;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			pRateLimitData->msRemainTryAgain = msRemainTryAgainLocal;
			pRateLimitData->timeStartedAtTryAgain = currentMSTimeTryAgainLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				executeByRateLimitData(pRateLimitData);
				postData = httpPOSTObjectDataAsync(relativeURL, content, pRateLimitData).get();
			}
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			postData.data = jsonValue;
			co_return postData;
		}

		task<HttpData> httpPATCHObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData patchData;
			string connectionPath = to_string(this->baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			auto contentHeaderCollection = HttpRequestHeaderCollection(nullptr);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpRequestMessage httpRequest;
			httpRequest.Method(HttpMethod::Patch());
			httpRequest.Content(contents);
			httpRequest.RequestUri(requestUri);
			HttpResponseMessage httpResponse;
			HttpCompletionOption completionOption;
			httpResponse = patchHttpClient.SendRequestAsync(httpRequest, completionOption).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			float currentMSTimeTryAgainLocal;
			float msRemainTryAgainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			currentMSTimeTryAgainLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			patchData.returnCode = (unsigned int)httpResponse.StatusCode();
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainTryAgainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
				msRemainLocal = 0.0f;
			}
			else {
				msRemainTryAgainLocal = 0;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			pRateLimitData->msRemainTryAgain = msRemainTryAgainLocal;
			pRateLimitData->timeStartedAtTryAgain = currentMSTimeTryAgainLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				executeByRateLimitData(pRateLimitData);
				patchData = httpPATCHObjectDataAsync(relativeURL, content, pRateLimitData).get();
			}
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			patchData.data = jsonValue;
			co_return patchData;
		}

		task<HttpData> httpDELETEObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData deleteData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = deleteHttpClient.DeleteAsync(requestUri).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			float currentMSTimeTryAgainLocal;
			float msRemainTryAgainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			currentMSTimeTryAgainLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			deleteData.returnCode = (unsigned int)httpResponse.StatusCode();
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainTryAgainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
				msRemainLocal = 0.0f;
			}
			else {
				msRemainTryAgainLocal = 0;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			pRateLimitData->msRemainTryAgain = msRemainTryAgainLocal;
			pRateLimitData->timeStartedAtTryAgain = currentMSTimeTryAgainLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				executeByRateLimitData(pRateLimitData);
				deleteData = httpDELETEObjectDataAsync(relativeURL, pRateLimitData).get();
			}
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			deleteData.data = jsonValue;
			co_return deleteData;
		}

		Uri baseURI{ nullptr };
		hstring baseURL;
		hstring botToken;
		hstring initialConnectionPath;
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection patchHeaders{ nullptr };
		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		HttpClient getHttpClient;
		HttpClient putHttpClient;
		HttpClient postHttpClient;
		HttpClient patchHttpClient;
		HttpClient deleteHttpClient;
	};
	concurrent_unordered_map<HttpWorkloadType, string> HttpRequestAgent::rateLimitDataBucketValues;
	concurrent_unordered_map<string, RateLimitData> HttpRequestAgent::rateLimitData;
}
#endif
