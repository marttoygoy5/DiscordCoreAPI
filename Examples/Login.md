# Login/Instantiation of the Library
## Make sure you've got the dependencies installed, then create a pointer to the DiscordCoreAPI::DiscordCoreClient class, as follows! (Note: Don't forget to execute the agent::wait() function on it, after executing the finalSetup() function.)
```C++
// Main.cpp - Main source file.
// https://github.com/RealTimeChris

#include "pch.h"
#include "Index.hpp"

int main() {
    winrt::init_apartment();
    Windows::System::AppDiagnosticInfo appDiagnosticInfo{ nullptr };
    string botToken = "ODQ3NzAzNTg3MzExNzE0MzM1.YLB7fA.OD332Lmt9wRMaTqr8IhezbUc4rM";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    agent::wait((agent*)pDiscordCoreClient.get());
    exception error;
    while (pDiscordCoreClient.get()->getError(error)) {
        cout << "DiscordCoreClient() Error: " << error.what() << endl << endl;
    }
    return 1;
}
```
