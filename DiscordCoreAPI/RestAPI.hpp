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
		nlohmann::json data;
		int getsRemaining = 1;
		int msRemain = 0;
	};

	struct RestAPI: implements<RestAPI, IInspectable> {
	public:

		RestAPI(hstring botToken, hstring baseURL, hstring* socketPath) {
			try {
				this->httpClient = winrt::Windows::Web::Http::HttpClient();
				this->headers = this->httpClient.DefaultRequestHeaders();
				this->baseURL = baseURL;
				this->botToken = botToken;
				this->initialConnectionPath = this->baseURL + L"/gateway/bot";
				hstring headerString = L"Bot ";
				hstring headerString2 = headerString + botToken;
				winrt::Windows::Web::Http::Headers::HttpCredentialsHeaderValue credentialValue(nullptr);
				credentialValue = credentialValue.Parse(headerString2.c_str());
				this->headers.Authorization(credentialValue);
				this->baseURI = Uri(this->initialConnectionPath.c_str());
				Windows::Web::Http::HttpResponseMessage httpResponse;
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
					std::string connectionPath = winrt::to_string(this->baseURL) + relativeURL;
					Uri requestUri = Uri(winrt::to_hstring(connectionPath.c_str()));
					Windows::Web::Http::HttpResponseMessage httpResponse;
					httpResponse = httpClient.GetAsync(requestUri).get();

					if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
						getData.getsRemaining = std::stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
					}
					else {
						getData.getsRemaining = 1;
					}
					if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
						getData.msRemain = static_cast<int>(std::stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000);
					}
					else {
						getData.msRemain = 0;
					}
					nlohmann::json jsonValue;
					jsonValue = jsonValue.parse(winrt::to_string(httpResponse.Content().ReadAsStringAsync().get().c_str()));
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

	protected:

		friend struct DiscordCoreAPI;
		friend struct WebSocket;

		hstring botToken;
		hstring baseURL;
		hstring initialConnectionPath;
		Uri baseURI = Uri{ nullptr };
		winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection headers{ nullptr };
		winrt::Windows::Web::Http::HttpClient httpClient;
	};
};
