### **Login/Instantiation of the Library**
##### 0. Install the dependencies, and #include Index.hpp.
##### 1. Create a string, with your bot's token.
##### 2. Create a new pointer to an object of the DiscordCoreAPI::DiscordCoreClient class, by executing the DiscordCoreClient::finalSetup() function.
##### 3. Execute agent::wait() on the DiscordCoreClient object.
##### 4. Collect and display any possible errors that pop up during the library's execution.
```C++
// Main.cpp - Main source file.
// https://github.com/RealTimeChris

#include "pch.h"
#include "Index.hpp"

int main() {
    winrt::init_apartment();
    string botToken = "BOT_TOKEN_HERE";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    agent::wait((agent*)pDiscordCoreClient.get());
    exception error;
    while (pDiscordCoreClient.get()->getError(error)) {
        cout << "DiscordCoreClient() Error: " << error.what() << endl << endl;
    }
    return 1;
}
```
