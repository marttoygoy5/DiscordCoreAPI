// RestAPI.hpp - Header for the REST API and its associated functions.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "JSONifier.hpp"

namespace CommanderNS {

	struct httpGETData {
		json data;
		int getsRemaining = 1;
		int msRemain = 0;
	};

	struct httpPOSTData {
		json data;
		int postsRemaining = 1;
		int msRemain = 0;
	};

	struct httpPUTData {
		json data;
		int putsRemaining = 1;
		int msRemain = 0;
	};

	struct httpDELETEData {
		json data;
		int deletesRemaining = 1;
		int msRemain = 0;
	};

	struct RestAPI: implements<RestAPI, IInspectable> {
	public:

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

		httpGETData httpGETObjectData(std::string relativeURL) {
			try {
				if (this != nullptr) {
					httpGETData getData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
					HttpResponseMessage httpResponse;
					httpResponse = httpClient.GetAsync(requestUri).get();

					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						getData.getsRemaining = std::stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						getData.getsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						getData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						getData.msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					getData.data = jsonValue;
					return getData;
				}
				else
				{
					return httpGETData();
				}
			}
			catch (winrt::hresult_error error) {
				cout << "Error: " << error.message().c_str() << std::endl;
			}
		}

		httpPOSTData httpPOSTObjectData(string relativeURL, string content) {
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
					HttpStringContent content(to_hstring(content), UnicodeEncoding::Utf8);
					content.Headers().ContentDisposition(headerValue);
					content.Headers().ContentType(typeHeaderValue);
					HttpResponseMessage httpResponse;
					httpResponse = httpClient.PostAsync(requestUri, content).get();

					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						postData.postsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						postData.postsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						postData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						postData.msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					postData.data = jsonValue;
					return postData;
				}
				else
				{
					return httpPOSTData();
				}
			}
			catch (winrt::hresult_error error) {
				cout << "Error: " << error.message().c_str() << std::endl;
			}
		}

		httpPUTData httpPUTObjectData(string relativeURL, string content) {
			try {
				if (this != nullptr) {
					cout << "HTTP OUT CALLED WITH RELATIVE URL: " << relativeURL << endl;
					httpPUTData putData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
					HttpContentHeaderCollection contentHeaderCollection;
					HttpContentDispositionHeaderValue headerValue(L"payload_json");
					contentHeaderCollection.ContentDisposition(headerValue);
					HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
					contentHeaderCollection.ContentType(typeHeaderValue);
					HttpStringContent content(to_hstring(content), UnicodeEncoding::Utf8);
					content.Headers().ContentDisposition(headerValue);
					content.Headers().ContentType(typeHeaderValue);
					HttpResponseMessage httpResponse;
					httpResponse = httpClient.PutAsync(requestUri, content).get();

					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						putData.putsRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						putData.putsRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						putData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						putData.msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					putData.data = jsonValue;
					return putData;
				}
				else
				{
					return httpPUTData();
				}
			}
			catch (winrt::hresult_error error) {
				cout << "Error: " << error.message().c_str() << std::endl;
			}
		}

		httpDELETEData httpDELETEObjectData(string relativeURL) {
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

					if (httpResponse.Headers().HasKey(L"X-RateLimit-Remaining")) {
						deleteData.deletesRemaining = stoi(httpResponse.Headers().TryLookup(L"X-RateLimit-Remaining").value().c_str());
					}
					else {
						deleteData.deletesRemaining = 0;
					}
					if (httpResponse.Headers().HasKey(L"X-RateLimit-Reset-After")) {
						deleteData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"X-RateLimit-Reset-After").value().c_str()) * 1000);
					}
					else {
						deleteData.msRemain = 250;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					deleteData.data = jsonValue;
					return deleteData;
				}
				else
				{
					return httpDELETEData();
				}
			}
			catch (winrt::hresult_error error) {
				cout << "Error: " << error.message().c_str() << std::endl;
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