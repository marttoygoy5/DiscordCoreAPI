// HttpStuff.hpp - Header for all of the HTTP related stuff.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_STUFF_
#define _HTTP_STUFF_

#include "pch.h"
#include "JSONifier.hpp"

namespace DiscordCoreInternal {

	class HttpRequestAgent;

	class HttpRequestAgent :public agent {
	public:
		unbounded_buffer<json> workReturnBuffer;
		unbounded_buffer<HttpWorkload> workSubmissionBuffer;
		HttpRequestAgent(HttpAgentResources agentResources, Scheduler* pScheduler = nullptr) 
			:
			agent(*pScheduler)
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
				this->deleteHttpClient = HttpClient();
				this->deleteHeaders = this->deleteHttpClient.DefaultRequestHeaders();
				hstring headerString = L"Bot ";
				hstring headerString2 = headerString + this->botToken;
				HttpCredentialsHeaderValue credentialValue(nullptr);
				credentialValue = credentialValue.Parse(headerString2.c_str());
				this->getHeaders.Authorization(credentialValue);
				this->putHeaders.Authorization(credentialValue);
				this->postHeaders.Authorization(credentialValue);
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

		bool get_error(exception& e)
		{
			return try_receive(_error, e);
		}

	protected:

		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;
		single_assignment<exception> _error;

		void run() {
			try {
				transformer<HttpWorkload, json> completeHttpRequest([this](HttpWorkload workload) -> json {
					RateLimitData rateLimitData;
					try {
						string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
						rateLimitData = HttpRequestAgent::rateLimitData.at(bucket);
					}
					catch (exception error) {
						cout << "HttpRequestAgent Error: " << error.what() << endl;
					}
					if (rateLimitData.getsRemaining == 0) {
						float loopStartTime = rateLimitData.timeStartedAt;
						float targetTime = loopStartTime + static_cast<float> (rateLimitData.msRemain);
						float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						while (rateLimitData.msRemain > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							rateLimitData.msRemain = targetTime - currentTime;
						}
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
					try {
						HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
						HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
						HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitData.bucket));
					}
					catch (exception error) {
						cout << "HttpRequestAgent Error: " << error.what() << endl;
						HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitData.bucket));
					}
					try {
						HttpRequestAgent::rateLimitData.at(rateLimitData.bucket);
						HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitData.bucket);
						HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitData.bucket, rateLimitData));
					}
					catch (exception error) {
						cout << "HttpRequestAgent Error: " << error.what() << endl;
						HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitData.bucket, rateLimitData));
					}
					return returnData.data;
					});
				completeHttpRequest.link_target(&this->workReturnBuffer);
				HttpWorkload workload = receive(&this->workSubmissionBuffer);
				send(&completeHttpRequest, workload);
				done();
			}
			catch (exception error) {
				send(_error, error);
			}
		}

		task<HttpData> httpGETObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData getData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = getHttpClient.GetAsync(requestUri).get();
			int currentMSTimeLocal;
			unsigned int getsRemainingLocal;
			int msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
				msRemainLocal = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"X-RateLimit-Bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = (float)msRemainLocal;
			pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
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
			int currentMSTimeLocal;
			unsigned int getsRemainingLocal;
			int msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
				msRemainLocal = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"X-RateLimit-Bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = (float)msRemainLocal;
			pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
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
			int currentMSTimeLocal;
			unsigned int getsRemainingLocal;
			int msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
				msRemainLocal = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"X-RateLimit-Bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = (float)msRemainLocal;
			pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
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
			httpResponse = postHttpClient.SendRequestAsync(httpRequest, completionOption).get();
			int currentMSTimeLocal;
			unsigned int getsRemainingLocal;
			int msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
				msRemainLocal = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"X-RateLimit-Bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = (float)msRemainLocal;
			pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
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
			int currentMSTimeLocal;
			unsigned int getsRemainingLocal;
			int msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
				msRemainLocal = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"X-RateLimit-Bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"X-RateLimit-Bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = (float)msRemainLocal;
			pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			json jsonValue;
			if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
				jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
			}
			deleteData.data = jsonValue;
			co_return deleteData;
		}

		Uri baseURI = Uri{ nullptr };
		hstring baseURL;
		hstring botToken;
		hstring initialConnectionPath;
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		HttpClient postHttpClient;
		HttpClient getHttpClient;
		HttpClient putHttpClient;
		HttpClient deleteHttpClient;
	};
	concurrent_unordered_map<HttpWorkloadType, string> HttpRequestAgent::rateLimitDataBucketValues;
	concurrent_unordered_map<string, RateLimitData> HttpRequestAgent::rateLimitData;
}
#endif
