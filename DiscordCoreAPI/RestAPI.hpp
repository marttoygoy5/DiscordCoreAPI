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

namespace CommanderNS {

	struct httpGETData {
		json data;
	};

	struct httpPOSTData {
		json data;
	};

	struct httpPUTData {
		json data;
		int msRemain = 0;
		int currentMsTime = 0;
		int getsRemaining = 1;
	};

	struct httpDELETEData {
		json data;
	};

	struct RestAPI : implements<RestAPI, IInspectable> {
	public:

		 static task<httpPUTData> theTask(RestAPI* pRestAPI, string relativeURL, string content, shared_ptr<FoundationClasses::RateLimitation> pRateLimitation) {
			 try {
				 if (pRestAPI != nullptr) {
					 httpPUTData putData;
					 string connectionPath = to_string(pRestAPI->baseURL) + relativeURL;
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
					 httpResponse = pRestAPI->httpClient.PutAsync(requestUri, contents).get();
					 putData.currentMsTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					 if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						 putData.getsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					 }
					 else {
						 putData.getsRemaining = 0;
					 }
					 if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						 pRateLimitation->msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					 }
					 else {
						 putData.msRemain = 250;
					 }
					 json jsonValue;
					 jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					 putData.data = jsonValue;
					 co_return putData;
				 }
				 else {
					 co_return httpPUTData();
				 }
			 }
			 catch (winrt::hresult_error error) {
				 wcout << L"Error: " << error.message().c_str() << std::endl;
			 }
		};

		RestAPI(hstring botToken, hstring baseURL, hstring* socketPath) {
			try {
				this->httpClient = HttpClient();
				this->headers = this->httpClient.DefaultRequestHeaders();
				this->baseURL = baseURL;
				this->botToken = botToken;
				this->initialConnectionPath = this->baseURL + L"/gateway/bot";
				hstring headerString = L"Bot ";
				hstring headerString2 = headerString + botToken;
				HttpCredentialsHeaderValue credentialValue(nullptr);
				credentialValue = credentialValue.Parse(headerString2.c_str());
				this->headers.Authorization(credentialValue);
				this->baseURI = Uri(this->initialConnectionPath.c_str());
				HttpResponseMessage httpResponse;
				httpResponse = httpClient.GetAsync(this->baseURI).get();
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

		httpGETData httpGETObjectData(std::string relativeURL, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData) {
			try {
				if (this != nullptr) {
					httpGETData getData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
					HttpResponseMessage httpResponse;
					httpResponse = httpClient.GetAsync(requestUri).get();
					pRateLimitData->currentMsTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						pRateLimitData->getsRemaining = std::stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						pRateLimitData->getsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						pRateLimitData->msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						pRateLimitData->msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					getData.data = jsonValue;
					return getData;
				}
				else {
					return httpGETData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
			}
		}

		httpPOSTData httpPOSTObjectData(string relativeURL, string content, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData) {
			try {
				if (this != nullptr) {
					httpPOSTData postData;
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
					httpResponse = httpClient.PostAsync(requestUri, contents).get();
					pRateLimitData->currentMsTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						pRateLimitData->getsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						pRateLimitData->getsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						pRateLimitData->msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						pRateLimitData->msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					postData.data = jsonValue;
					return postData;
				}
				else {
					return httpPOSTData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
			}
		}

		httpPUTData httpPUTObjectData(string relativeURL, string content, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData) {
			try {
				if (this != nullptr) {
					httpPUTData putData;
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
					httpResponse = httpClient.PutAsync(requestUri, contents).get();
					pRateLimitData->currentMsTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						pRateLimitData->getsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						pRateLimitData->getsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						pRateLimitData->msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						pRateLimitData->msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					putData.data = jsonValue;
					return putData;
				}
				else {
					return httpPUTData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
			}
		}

		httpDELETEData httpDELETEObjectData(string relativeURL, shared_ptr<FoundationClasses::RateLimitation> pRateLimitData) {
			try {
				if (this != nullptr) {
					httpDELETEData deleteData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
					HttpContentHeaderCollection contentHeaderCollection;
					HttpContentDispositionHeaderValue headerValue(L"payload_json");
					contentHeaderCollection.ContentDisposition(headerValue);
					HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
					contentHeaderCollection.ContentType(typeHeaderValue);
					HttpResponseMessage httpResponse;
					httpResponse = httpClient.DeleteAsync(requestUri).get();
					pRateLimitData->currentMsTime = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						pRateLimitData->getsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						pRateLimitData->getsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						pRateLimitData->msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						pRateLimitData->msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					deleteData.data = jsonValue;
					return deleteData;
				}
				else {
					return httpDELETEData();
				}
			}
			catch (winrt::hresult_error error) {
				wcout << L"Error: " << error.message().c_str() << std::endl;
			}
		}

	protected:

		friend struct DiscordCoreAPI;
		friend struct WebSocket;

		hstring botToken;
		hstring baseURL;
		hstring initialConnectionPath;
		Uri baseURI = Uri{ nullptr };
		HttpRequestHeaderCollection headers{ nullptr };
		HttpClient httpClient;
	};
};
#endif