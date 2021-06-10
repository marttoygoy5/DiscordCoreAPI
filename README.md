# DiscordCoreAPI
My bot library/framework, written in C++ using WinRT, and a few other useful libraries!

## Features
### Concurrent Discord API and MongoDB Access
As a result of using the asynchronous agents library, this library has the ability to make fully asynchronous requests to the Discord API, as well as fully asynchronous reads/writes to/from the built-in MongoDB database.

### Slash Commands and Buttons
With relatively simple interfaces for utilization!
```C++
// For example - adding a "slash command" to the bot's application.
CreateApplicationCommandData createBuyCommandData;
createBuyCommandData.defaultPermission = true;
createBuyCommandData.description = "Purchase an item from the guildshop.";
createBuyCommandData.name = "buy";
ApplicationCommandOptionData buyCommandOptionOne;
buyCommandOptionOne.name = "objectname";
buyCommandOptionOne.required = true;
buyCommandOptionOne.type = ApplicationCommandOptionType::STRING;
buyCommandOptionOne.description = "The item or role which you would like to purchase.";
createBuyCommandData.options.push_back(buyCommandOptionOne);
args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createBuyCommandData).get();
```
![](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(53).png?raw=true)
### A Unified "Input Event" System
Both user-messages and user-interactions are accepted via the "Input-Event" event.
They can all be responded to using the InputEventHandler::respondToEvent() function.
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

## Examples/Documentation
### [Login]()

## Roadmap
Alright, so I'm currently working on getting this thing to be "useful", and here are some of the things I plan on implementing:

-Finish adding the base Discord-API data structures, along with classes to utilize/access/change their values.

-Add a "music-listening" layer, with a usable "voice channel" that includes audio encoding/decoding.

-Build a few bots with the library and document examples.

If you have any suggestions, feel free to let me know and I will consider taking you up on it! Thanks!

## Dependencies

[WinRT](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt)  

NOTE: I installed these using the [vcpkg](https://github.com/microsoft/vcpkg) installer!

[Nlohmann Json](https://github.com/nlohmann/json)

[MongoDB](https://docs.mongodb.com/manual/installation/)

[LibCurl](https://github.com/curl/curl)

