// RestAPI.hpp - Header for the REST API and its associated functions.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "pch.h"
#include "HelperFunctions.hpp"
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

					if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
						getData.getsRemaining = std::stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
					}
					else {
						getData.getsRemaining = 1;
					}
					if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
						getData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000);
					}
					else {
						getData.msRemain = 0;
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
				std::cout << "Error: " << error.message().c_str() << std::endl;
			}
		}

		httpPOSTData httpPOSTObjectData(string relativeURL, string content) {
			try {
				if (this != nullptr) {
					httpPOSTData getData;
					string connectionPath = to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
					HttpResponseMessage httpResponse;
					HttpStringContent content(to_hstring(content), UnicodeEncoding::Utf8);
					httpResponse = httpClient.PostAsync(requestUri, content).get();

					if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
						getData.postsRemaining = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
					}
					else {
						getData.postsRemaining = 1;
					}
					if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
						getData.msRemain = static_cast<int>(stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000);
					}
					else {
						getData.msRemain = 0;
					}
					json jsonValue;
					jsonValue = jsonValue.parse(to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
					getData.data = jsonValue;
					return getData
				}
				else
				{
					return httpPOSTData();
				}
			}
			catch (winrt::hresult_error error) {
				std::cout << "Error: " << error.message().c_str() << std::endl;
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