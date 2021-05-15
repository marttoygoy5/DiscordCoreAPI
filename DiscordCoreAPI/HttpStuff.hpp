// HttpStuff.hpp - Header for all of the HTTP related stuff.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_STUFF_
#define _HTTP_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"
#include "JSONifier.hpp"

namespace DiscordCoreInternal {

	struct HttpRequestAgent;

	enum class HttpWorkloadClass {
		GET = 0,
		PUT = 1,
		POST = 2,
		PATCH = 3,
		DELETED = 4
	};

	enum class HttpWorkloadType {
		UNSET = 0,
		GET_MESSAGE = 1,
		POST_MESSAGE = 2,
		DELETE_MESSAGE = 3,
		GET_USER = 4,
		GET_USER_SELF = 5,
		GET_GUILD = 6,
		GET_CHANNEL = 7,
		GET_REACTION = 8,
		PUT_REACTION = 9,
		DELETE_REACTION = 10,
		PATCH_MESSAGE = 11,
		DELETE_ALL_REACTION = 12,
		GET_GUILD_MEMBER = 13,
		GET_GUILD_MEMBERS = 14,
		GET_ROLES = 15,
		GET_USER_GUILDS = 16
	};

	struct HttpAgentPointers {
		com_ptr<HttpRequestAgent> pGETAgent;
		com_ptr<HttpRequestAgent> pPUTAgent;
		com_ptr<HttpRequestAgent> pPOSTAgent;
		com_ptr<HttpRequestAgent> pPATCHAgent;
		com_ptr<HttpRequestAgent> pDELETEAgent;
	};

	struct HttpData {
		json data;
	};

	struct HttpWorkload {
		HttpWorkloadClass workloadClass;
		HttpWorkloadType workloadType;
		string relativePath;
		string content;
	};

	struct RateLimitData {
		HttpWorkloadType workloadType;
		float timeStartedAt = 0;
		int getsRemaining = 1;
		float msRemain = 0;
		string bucket;
	};

	struct HttpRequestAgent :agent, implements<HttpRequestAgent, winrt::Windows::Foundation::IInspectable> {
	public:
		unbounded_buffer<HttpWorkload> workSubmissionBuffer;
		unbounded_buffer<json> workReturnBuffer;
		HttpRequestAgent(hstring botToken, hstring baseURL, hstring* pSocketPath, Scheduler* pScheduler = nullptr) 
			:
			agent(*pScheduler)
		{
			try {
				this->baseURL = baseURL;
				this->botToken = botToken;
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
				hstring headerString2 = headerString + botToken;
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
				stream << parseSocketPath(httpResponseBody.c_str()).c_str();
				stream << L"/?v=9&encoding=json";
				*pSocketPath = stream.str();
			}
			catch (winrt::hresult_error ex) {
				std::wcout << "Error: " << ex.message().c_str() << std::endl;
			}
		}

		void terminate() {
			this->doWeQuit = true;
		}

	protected:
		bool doWeQuit = false;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;

		void run() {
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
				send(this->workReturnBuffer, returnData.data);
				});
			completeHttpRequest.link_target(&this->workReturnBuffer);
			while (this->doWeQuit == false) {
				HttpWorkload workload;
				if (try_receive(&this->workSubmissionBuffer, workload)) {
					send(&completeHttpRequest, workload);
				}
			}
			done();
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
			string connectionPath = to_string(baseURL) + relativeURL;
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
			wcout << httpResponse.Content().ReadAsStringAsync().get().c_str() << "WHAT IS UP?" << endl;
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