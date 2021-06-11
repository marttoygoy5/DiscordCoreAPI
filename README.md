# DiscordCoreAPI
- A Discord bot library, written in C++ using WinRT, and a few other useful libraries!

## [Documentation-Examples](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md)

## Features
### Concurrent Discord API Access
- As a result of using the asynchronous agents library, this library has the ability to make fully asynchronous requests to the Discord API.

### Slash Commands and Buttons
![](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(53).png?raw=true)
### A Unified "Input-Event" System
- Both user-messages and user-interactions are accepted via the "Input-Event" event.
- They can all be responded to using the InputEventHandler::respondToEvent() function.
```C++
if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
	InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
	responseData.channelId = args->eventData.messageData.channelId;
	responseData.messageId = args->eventData.messageData.id;
	responseData.embeds.push_back(messageEmbed);
	event01 = InputEventHandler::respondToEvent(responseData).get();
	InputEventHandler::deleteInputEventResponse(event01, 20000).get();
}
else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
	InputEventResponseData responseData(InputEventResponseType::INTERACTION_RESPONSE);
	responseData.applicationId = args->eventData.interactionData.applicationId;
	responseData.embeds.push_back(messageEmbed);
	responseData.interactionId = args->eventData.interactionData.id;
	responseData.interactionToken = args->eventData.interactionData.token;
	responseData.type = InteractionCallbackType::ChannelMessage;
	event01 = InputEventHandler::respondToEvent(responseData).get();
	event01.interactionData.applicationId = args->eventData.interactionData.applicationId;
	event01.interactionData.token = args->eventData.interactionData.token;
	InputEventHandler::deleteInputEventResponse(event01, 20000).get();
}
```
## Dependencies

- [WinRT](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt)
(NOTE: I've got the "C++ Language Standard" setting set to "Preview - Features from the Latest C++ Working Draft (/std:c++latest)" in Visual Studio for this library!)

- NOTE: I installed these using the [vcpkg](https://github.com/microsoft/vcpkg) installer!
 
- [Nlohmann Json](https://github.com/nlohmann/json)

- [MongoDB](https://docs.mongodb.com/manual/installation/)

- [LibCurl](https://github.com/curl/curl)

## Roadmap
Alright, so I'm currently working on getting this thing to be "useful", and here are some of the things I plan on implementing:

- Finish adding the base Discord-API data structures, along with classes to utilize/access/change their values.

- Add a "music-listening" layer, with a usable "voice channel" that includes audio encoding/decoding.

- Build a few bots with the library and document examples.

If you have any suggestions, feel free to let me know and I will consider taking you up on it! Thanks!
