# Login/Instantiation of the Library
### 1. Create a string, with your bot's token.
### 2. Create a new pointer to the DiscordCoreAPI::DiscordCoreClient class.
### 3. Execute the DiscordCoreClient::finalSetup() function.
### 4. Execute agent::wait() on the DiscordCoreClient class.
### 5. Collect and display any possible errors that pop up during the library's execution.
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
