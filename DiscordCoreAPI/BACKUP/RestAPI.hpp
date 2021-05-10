// RestAPI.hpp - Header for the REST API and its associated functions.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REST_API_
#define _REST_API_

#include "pch.h"
#include "JSONifier.hpp"
#include "FoundationClasses.hpp"
#include "SystemThreads.hpp"

namespace CommanderNS {

	struct RestAPI : implements<RestAPI, winrt::Windows::Foundation::IInspectable> {

	public:
		RestAPI(hstring botToken, hstring baseURL, hstring* pSocketPath) {
			try {
				RestAPI::baseURL = baseURL;
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
				stream << JSONifier::parseSocketPath(httpResponseBody.c_str()).c_str();
				stream << L"/?v=8&encoding=json";
				*pSocketPath = stream.str();
			}
			catch (winrt::hresult_error ex) {
				std::wcout << "Error: " << ex.message().c_str() << std::endl;
			}
		}

		static task<HTTPData> httpGETObjectDataAsync(std::string relativeURL, RateLimitData* pRateLimitData) {
			HTTPData getData;
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

		static task<HTTPData> httpPUTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HTTPData putData;
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

		static task<HTTPData> httpPOSTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HTTPData postData;
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

		static task<HTTPData > httpDELETEObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HTTPData  deleteData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentHeaderCollection contentHeaderCollection;
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			contentHeaderCollection.ContentDisposition(headerValue);
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			contentHeaderCollection.ContentType(typeHeaderValue);
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

	protected:

		friend struct DiscordCoreAPI;
		friend struct WebSocket;
		Uri baseURI = Uri{ nullptr };
		hstring botToken;
		static hstring baseURL;
		hstring initialConnectionPath;
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		static HttpClient getHttpClient;
		static HttpClient putHttpClient;
		static HttpClient postHttpClient;
		static HttpClient deleteHttpClient;
	};
	hstring RestAPI::baseURL;
	HttpClient RestAPI::getHttpClient;
	HttpClient RestAPI::putHttpClient;
	HttpClient RestAPI::postHttpClient;
	HttpClient RestAPI::deleteHttpClient;
};
#endif
