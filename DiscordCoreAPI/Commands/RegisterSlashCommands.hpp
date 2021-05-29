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
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments* args) {
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
			*/
		}
	};
	RegisterSlashCommands registerSlashCommands{};
}
#endif
