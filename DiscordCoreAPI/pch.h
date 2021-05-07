// pch.h - The precompiled header file.
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _THE_CORE_API_
#define _THE_CORE_API_
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _AMD64_
#define _AMD64_
#endif
#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif
#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif

#include <windows.h>
#include <agents.h>
#include <winrt/base.h>
#include <winrt/windows.system.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.System.Threading.Core.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <DispatcherQueue.h>
#include <winrt/impl/Windows.Media.Import.0.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/windows.networking.sockets.h>
#include <winrt/windows.storage.streams.h>
#include <winrt/windows.ui.xaml.h>
#include <pplawait.h>
#include <ppl.h>
#include <concrt.h>
#include <processenv.h>
#include <consoleapi.h>
#include <iostream>
#include <sstream>
#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <agents.h>
#include <concrt.h>
#include <memory>
#include <concrtrm.h>
#include </json-develop/include/nlohmann/json.hpp>
#include <curl.h>
#include <winrt/impl/Windows.Foundation.0.h>

using namespace winrt;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Web::Http::Headers;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Storage;
using namespace concurrency;
using namespace nlohmann;
using namespace std;
