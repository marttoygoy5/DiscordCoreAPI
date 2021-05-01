// pch.h - The precompiled header.
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _AMD64_

#pragma comment(lib, "Shlwapi.lib")

#include <winrt/base.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/windows.networking.sockets.h>
#include <winrt/windows.storage.streams.h>
#include <winrt/windows.system.h>
#include <winrt/windows.ui.xaml.h>
#include <pplawait.h>
#include <ppltasks.h>
#include <processenv.h>
#include <consoleapi.h>
#include <iostream>
#include <sstream>
#include <shlwapi.h>
#include "/json-develop/include/nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Headers;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;
