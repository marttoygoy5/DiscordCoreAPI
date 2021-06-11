### **Adding a Slash Command**
#### 1. Create a "CreateApplicationCommandData" data structure.
#### 2. For each argument, fill out an ApplicationCommandOptionData structure, and push it back into the options member of the CreateApplicationCommandData structure. (IMPORTANT #1: Notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/slash-commands#subcommands-and-subcommand-groups) (IMPORTANT #2: Be sure to set the order of the arguments up to be equivalent to what they are in the normal version of the command, so that they can be parsed properly.)
#### 3. Run, from the DiscordCoreClient instance, the "slashCommands::createGlobalApplicationCommandAsync() function. (Note: This is done from within a command called "registerslashcommands", by default.)

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
