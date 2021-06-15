// RegisterSlashCommands.hpp - Registers the slash commands of this bot.
// May 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REGISTER_SLASH_COMMANDS_
#define _REGISTER_SLASH_COMMANDS_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {
	
	class RegisterSlashCommands :public BaseFunction {
	public:
		RegisterSlashCommands() {
			this->commandName = "registerslashcommands";
			this->helpDescription = "__**Register Slash Commands Usage:**__ !registerslashcommands or /registerslashcommands!";
		}
		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try{
				/*
			CreateApplicationCommandData createSellDrugsCommandData;
			createSellDrugsCommandData.defaultPermission = true;
			createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createSellDrugsCommandData).get();
			CreateApplicationCommandData registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.defaultPermission = true;
			registerSlashCommandsCommandData.description = "Register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(registerSlashCommandsCommandData).get();

			CreateApplicationCommandData createBalanceCommandData;
			createBalanceCommandData.defaultPermission = true;
			createBalanceCommandData.description = "Check your or another person's currency balances.";
			createBalanceCommandData.name = "balance";
			ApplicationCommandOptionData applicationCommandOptionOne;
			applicationCommandOptionOne.name = "person";
			applicationCommandOptionOne.required = false;
			applicationCommandOptionOne.type = ApplicationCommandOptionType::USER;
			applicationCommandOptionOne.description = "The person who's balances you would like to check.";
			createBalanceCommandData.options.push_back(applicationCommandOptionOne);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createBalanceCommandData).get();

			CreateApplicationCommandData createAddShopItemCommandData;
			createAddShopItemCommandData.defaultPermission = true;
			createAddShopItemCommandData.description = "Add an item to the shop's inventory.";
			createAddShopItemCommandData.name = "addshopitem";
			ApplicationCommandOptionData addShopItemCommandOptionOne;
			addShopItemCommandOptionOne.name = "itemname";
			addShopItemCommandOptionOne.required = true;
			addShopItemCommandOptionOne.type = ApplicationCommandOptionType::STRING;
			addShopItemCommandOptionOne.description = "The name of the item.";
			createAddShopItemCommandData.options.push_back(addShopItemCommandOptionOne);
			ApplicationCommandOptionData addShopItemCommandOptionTwo;
			addShopItemCommandOptionTwo.name = "selfmod";
			addShopItemCommandOptionTwo.required = true;
			addShopItemCommandOptionTwo.type = ApplicationCommandOptionType::INTEGER;
			addShopItemCommandOptionTwo.description = "The self-mod value of the item.";
			createAddShopItemCommandData.options.push_back(addShopItemCommandOptionTwo);
			ApplicationCommandOptionData addShopItemCommandOptionThree;
			addShopItemCommandOptionThree.name = "oppmod";
			addShopItemCommandOptionThree.required = true;
			addShopItemCommandOptionThree.type = ApplicationCommandOptionType::INTEGER;
			addShopItemCommandOptionThree.description = "The opp-mod value of the item.";
			createAddShopItemCommandData.options.push_back(addShopItemCommandOptionThree);
			ApplicationCommandOptionData addShopItemCommandOptionFour;
			addShopItemCommandOptionFour.name = "itemcost";
			addShopItemCommandOptionFour.required = true;
			addShopItemCommandOptionFour.type = ApplicationCommandOptionType::INTEGER;
			addShopItemCommandOptionFour.description = "The value/cost of the item.";
			createAddShopItemCommandData.options.push_back(addShopItemCommandOptionFour);
			ApplicationCommandOptionData addShopItemCommandOptionFive;
			addShopItemCommandOptionFive.name = "emoji";
			addShopItemCommandOptionFive.required = true;
			addShopItemCommandOptionFive.type = ApplicationCommandOptionType::STRING;
			addShopItemCommandOptionFive.description = "The emoji/icon to use for the item.";
			createAddShopItemCommandData.options.push_back(addShopItemCommandOptionFive);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createAddShopItemCommandData).get();

			CreateApplicationCommandData createAddShopRoleCommandData;
			createAddShopRoleCommandData.defaultPermission = true;
			createAddShopRoleCommandData.description = "Add a role to the shop's inventory.";
			createAddShopRoleCommandData.name = "addshoprole";
			ApplicationCommandOptionData addShopRoleCommandOptionOne;
			addShopRoleCommandOptionOne.name = "rolename";
			addShopRoleCommandOptionOne.required = true;
			addShopRoleCommandOptionOne.type = ApplicationCommandOptionType::STRING;
			addShopRoleCommandOptionOne.description = "The name of the role.";
			createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionOne);
			ApplicationCommandOptionData addShopRoleCommandOptionTwo;
			addShopRoleCommandOptionTwo.name = "hexcolorvalue";
			addShopRoleCommandOptionTwo.required = true;
			addShopRoleCommandOptionTwo.type = ApplicationCommandOptionType::STRING;
			addShopRoleCommandOptionTwo.description = "The hex-color value of the role.";
			createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionTwo);
			ApplicationCommandOptionData addShopRoleCommandOptionThree;
			addShopRoleCommandOptionThree.name = "rolecost";
			addShopRoleCommandOptionThree.required = true;
			addShopRoleCommandOptionThree.type = ApplicationCommandOptionType::INTEGER;
			addShopRoleCommandOptionThree.description = "The value/cost of the role.";
			createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionThree);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createAddShopRoleCommandData).get();

			CreateApplicationCommandData createCoinflipRoleCommandData;
			createCoinflipRoleCommandData.defaultPermission = true;
			createCoinflipRoleCommandData.description = "Play heads or tails.";
			createCoinflipRoleCommandData.name = "coinflip";
			ApplicationCommandOptionData coinflipCommandOptionOne;
			coinflipCommandOptionOne.name = "betamount";
			coinflipCommandOptionOne.required = true;
			coinflipCommandOptionOne.type = ApplicationCommandOptionType::INTEGER;
			coinflipCommandOptionOne.description = "The wager you would like to place.";
			createCoinflipRoleCommandData.options.push_back(coinflipCommandOptionOne);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createCoinflipRoleCommandData).get();

			CreateApplicationCommandData createBotInfoCommandData;
			createBotInfoCommandData.defaultPermission = true;
			createBotInfoCommandData.description = "Display info about the current bot.";
			createBotInfoCommandData.name = "botinfo";
			ApplicationCommandOptionData botInfoCommandOptionOne;
			botInfoCommandOptionOne.name = "botname";
			botInfoCommandOptionOne.required = true;
			botInfoCommandOptionOne.type = ApplicationCommandOptionType::STRING;
			botInfoCommandOptionOne.description = "The bot which you would like to display the info of.";
			ApplicationCommandOptionChoiceData botInfoChoiceOne;
			botInfoChoiceOne.name = "name";
			botInfoChoiceOne.valueString = "gamehouse";
			botInfoCommandOptionOne.choices.push_back(botInfoChoiceOne);
			createBotInfoCommandData.options.push_back(botInfoCommandOptionOne);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createBotInfoCommandData).get();

			CreateApplicationCommandData createBlackjackCommandData;
			createBlackjackCommandData.defaultPermission = true;
			createBlackjackCommandData.description = "Play a round of blackjack.";
			createBlackjackCommandData.name = "blackjack";
			ApplicationCommandOptionData blackJackCommandOptionOne;
			blackJackCommandOptionOne.name = "betamount";
			blackJackCommandOptionOne.required = true;
			blackJackCommandOptionOne.type = ApplicationCommandOptionType::INTEGER;
			blackJackCommandOptionOne.description = "The amount which you would like to wager.";
			createBlackjackCommandData.options.push_back(blackJackCommandOptionOne);
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createBlackjackCommandData).get();

			CreateApplicationCommandData createButtonsCommandData;
			createButtonsCommandData.defaultPermission = true;
			createButtonsCommandData.description = "Test the buttons.";
			createButtonsCommandData.name = "buttons";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createButtonsCommandData).get();

			CreateApplicationCommandData createShopCommandData;
			createShopCommandData.defaultPermission = true;
			createShopCommandData.description = "Check out the server's shop!";
			createShopCommandData.name = "shop";
			args->coreClient->slashCommands->createGlobalApplicationCommandAsync(createShopCommandData).get();

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
			
			DiscordCoreAPI::CreateApplicationCommandData createTestCommandData;
			createTestCommandData.defaultPermission = true;
			createTestCommandData.description = "A test command.";
			createTestCommandData.name = "test";
			DiscordCoreAPI::ApplicationCommandOptionData testCommandOptionOne;
			testCommandOptionOne.name = "optionone";
			testCommandOptionOne.required = true;
			testCommandOptionOne.type = ApplicationCommandOptionType::STRING;
			testCommandOptionOne.description = "The first argument to be entered.";
			createTestCommandData.options.push_back(testCommandOptionOne);
			args->eventData.discordCoreClient->slashCommands->createGlobalApplicationCommandAsync(createTestCommandData).get();
			*/
			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.messageData.guildId }).get();
			DiscordGuild discordGuild(guild.data);
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("Nicely done, you've registered some commands!");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Register Slash Commands Complete:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData responseData(args->eventData);
				responseData.embed = msgEmbed;
				InputEventData  event01 = InputEventHandler::respondToEvent(responseData).get();
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateInteractionResponseData responseData(args->eventData);
				responseData.data.embeds.push_back(msgEmbed);
				InputEventData event;
				event = InputEventHandler::respondToEvent(responseData).get();
			}
			co_return;
			}
			catch (exception& e) {
				cout << "RegisterSlashCommands::execute() Error: " << e.what() << endl << endl;
			}
		}
	};
	RegisterSlashCommands registerSlashCommands{};
}
#endif
