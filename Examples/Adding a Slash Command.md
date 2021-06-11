```C++
CreateApplicationCommandData createTestCommandData;
createTestCommandData.defaultPermission = true;
createTestCommandData.description = "A test command.";
createTestCommandData.name = "test";
ApplicationCommandOptionData testCommandOptionOne;
testCommandOptionOne.name = "optionone";
testCommandOptionOne.required = true;
testCommandOptionOne.type = ApplicationCommandOptionType::STRING;
testCommandOptionOne.description = "The first argument to be entered.";
createTestCommandData.options.push_back(testCommandOptionOne);
args->eventData->discordCoreClient->slashCommands->createGlobalApplicationCommandAsync(createTestCommandData).get();
```
