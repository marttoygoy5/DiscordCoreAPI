﻿// RestAPI.hpp - Header for the REST API and its associated functions.
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

	struct httpData {
		json data;
	};

	struct RestAPI : implements<RestAPI, winrt::Windows::Foundation::IInspectable> {

		com_ptr<SystemThreads> pSystemThreads;

	public:
		RestAPI(hstring botToken, hstring baseURL, hstring* socketPath, com_ptr<SystemThreads> pSystemThreads) {
			try {
				this->pSystemThreads = pSystemThreads;
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
				stream << JSONifier::parseSocketPath(httpResponseBody.c_str()).c_str();
				stream << L"/?v=8&encoding=json";
				*socketPath = stream.str();
			}
			catch (winrt::hresult_error ex) {
				std::wcout << "Error: " << ex.message().c_str() << std::endl;
			}
		}

		task<httpData> httpGETObjectDataAsync(std::string relativeURL, FoundationClasses::RateLimitData* pRateLimitData) {
			try {
				if (this != nullptr) {
					httpData getData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
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
					pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
					pRateLimitData->getsRemaining = getsRemainingLocal;
					pRateLimitData->msRemain = (float)msRemainLocal;
					json jsonValue;
					if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
						jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					}
					getData.data = jsonValue;
					co_return getData;
				}
				else {
					co_return httpData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
				co_return httpData();
			}
		}

		task<httpData> httpPOSTObjectDataAsync(string relativeURL, string content, FoundationClasses::RateLimitData* pRateLimitData) {
			try {
				if (this != nullptr) {
					httpData postData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
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
					pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
					pRateLimitData->getsRemaining = getsRemainingLocal;
					pRateLimitData->msRemain = (float)msRemainLocal;
					json jsonValue;
					if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
						jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					}
					postData.data = jsonValue;
					co_return postData;
				}
				else {
					co_return httpData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
				co_return httpData();
			}
		}

		task<httpData> httpPUTObjectDataAsync(string relativeURL, string content, FoundationClasses::RateLimitData* pRateLimitData) {
			try {
				if (this != nullptr) {
					httpData putData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
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
					pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
					pRateLimitData->getsRemaining = getsRemainingLocal;
					pRateLimitData->msRemain = (float)msRemainLocal;
					json jsonValue;
					if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
						jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					}
					putData.data = jsonValue;
					co_return putData;
				}
				else {
					co_return httpData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
				co_return httpData();
			}
		}

		task<httpData> httpDELETEObjectDataAsync(string relativeURL, FoundationClasses::RateLimitData* pRateLimitData) {
			try{
				if (this != nullptr) {
					httpData deleteData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
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
					pRateLimitData->timeStartedAt = (float)currentMSTimeLocal;
					pRateLimitData->getsRemaining = getsRemainingLocal;
					pRateLimitData->msRemain = (float)msRemainLocal;
					json jsonValue;
					if (httpResponse.Content().ReadAsStringAsync().get() != L"") {
						jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					}
					deleteData.data = jsonValue;
					co_return deleteData;
				}
				else {
					co_return httpData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
				co_return httpData();
			}
		}

	protected:

		friend struct DiscordCoreAPI;
		friend struct WebSocket;
		hstring botToken;
		hstring baseURL;
		hstring initialConnectionPath;
		Uri baseURI = Uri{ nullptr };
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		HttpClient getHttpClient;
		HttpClient putHttpClient;
		HttpClient postHttpClient;
		HttpClient deleteHttpClient;
	};
};
#endif