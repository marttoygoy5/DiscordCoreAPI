// Blackjack.hpp - Header for the blackjack command.
// May 31, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BLACKJACK_
#define _BLACKJACK_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

/**
	Refreshes a blackjack stack of Cards.
*/
vector<DiscordCoreAPI::Card> refreshBlackjackStack() {
	vector<DiscordCoreAPI::Card> newCardStack;
	if (newCardStack.size() == 0) {
		newCardStack.resize(312);
		for (unsigned int x = 0; x < 6; x += 1) {
			DiscordCoreAPI::Deck newDeck;
			for (unsigned int y = 0; y < 52; y += 1) {
				newCardStack[x * 52 + y] = newDeck.drawRandomcard();
			}
		}
	}
	return newCardStack;
}

/**
	Draws the next card from a stack of blackjack cards.
 */
DiscordCoreAPI::Card drawNextBlackjackCard(vector<DiscordCoreAPI::Card>* cardStack) {
	if (cardStack->size() == 0) {
		*cardStack = refreshBlackjackStack();
		DiscordCoreAPI::Card currentCard = cardStack->at(0);
		cardStack->erase(cardStack->begin());
		return currentCard;
	}

	DiscordCoreAPI::Card currentCard = cardStack->at(0);
	cardStack->erase(cardStack->begin());
	return currentCard;
}

void checkAndSetAceValues(vector<DiscordCoreAPI::Card>* playerHand, vector<unsigned int>* playerAceIndices) {
	auto newPlayerAceIndices = playerAceIndices;
	if (playerHand->at(playerHand->size() - 1).type ==  "Ace") {
		newPlayerAceIndices->resize(newPlayerAceIndices->size() + 1);
		newPlayerAceIndices->at((int)newPlayerAceIndices->size() - 1) = (int)playerHand->size() - 1;
	}
	auto playerNonAceValue = 0;
	for (auto x = 0; x < playerHand->size(); x += 1) {
		if (playerHand->at(x).type != "Ace") {
			playerNonAceValue += playerHand->at(x).value;
		}
	}
	auto newPlayerHand = playerHand;
	if (playerNonAceValue + 11 > 21 && newPlayerAceIndices->size()> 0) {
		newPlayerHand->at(newPlayerAceIndices->at(0)).value = 1;
	}
	else if (playerNonAceValue + 11 <= 21 && playerAceIndices->size()> 0) {
		newPlayerHand->at(playerAceIndices->at(0)).value = 11;
	}
	for (auto x = 0; x < playerAceIndices->size(); x += 1) {
		if (x > 0) {
			newPlayerHand->at(playerAceIndices->at(x)).value = 1;
		}
	}
}

task<void> executeCrossResponse(DiscordCoreAPI::Button button, DiscordCoreAPI::DiscordGuildMember discordGuildMember, unsigned int betAmount, DiscordCoreAPI::GuildMember guildMember, DiscordCoreAPI::DiscordGuild discordGuild, DiscordCoreAPI::Message newMessage, DiscordCoreAPI::BaseFunctionArguments* args,
	unsigned int newCardCount, vector<DiscordCoreAPI::Card> userHand, vector<unsigned int>* userAceIndices, vector<unsigned int>* dealerAceIndices, string userID, vector<DiscordCoreAPI::Card> dealerHand, DiscordCoreAPI::ActionRowData actionRowData2) {
	discordGuildMember.getDataFromDB().get();
	unsigned int fineAmount = 0;
	fineAmount = 1 * betAmount;
	if (fineAmount > discordGuildMember.data.currency.wallet){
		string inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";
		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField(newMessage.data.embeds[0].fields[0].name, newMessage.data.embeds[0].fields[0].value, newMessage.data.embeds[0].fields[0].Inline);
		msgEmbed.addField(newMessage.data.embeds[0].fields[1].name, newMessage.data.embeds[0].fields[1].value, newMessage.data.embeds[0].fields[1].Inline);
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand.size(); x += 1) {
		newUserHandScore += userHand.at(x).value;
	}
	unsigned int newDealerHandScore = 0;
	while (newDealerHandScore < 17) {
		for (auto x = 0; x < dealerHand.size(); x += 1) {
			newDealerHandScore += dealerHand.at(x).value;
		}
		if (newDealerHandScore >= 17) {
			break;
		}
		discordGuild.getDataFromDB().get();
		dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
		discordGuild.writeDataToDB().get();

		checkAndSetAceValues(&dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand.size(); x += 1) {
		newDealerHandScore += dealerHand.at(x).value;
	}

	string dealerHandString;
	for (auto x = 0; x < dealerHand.size(); x += 1) {
		dealerHandString += dealerHand.at(x).suit + dealerHand.at(x).type;
	}

	string userHandString;
	for (auto x = 0; x < userHand.size(); x += 1) {
		userHandString += userHand.at(x).suit + userHand.at(x).type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21
		&& newUserHandScore > newDealerHandScore) || (newUserHandScore < 21
			&& newDealerHandScore > 21)) {
		unsigned int payAmount = betAmount;
		discordGuild.getDataFromDB().get();
		if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
			discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
		}
		discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild.data.casinoStats.totalPayout += payAmount;
		discordGuild.writeDataToDB().get();

		discordGuildMember.getDataFromDB().get();
		discordGuildMember.data.currency.wallet += payAmount;
		discordGuildMember.writeDataToDB().get();

		string winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " +
			args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		if (newMessage.data.embeds.at(0).description[0] != (char)0x40) {
			msgEmbed.setDescription(newMessage.data.embeds[0].description);
		}		
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.embed = msgEmbed;
		editData.originalMessageData = newMessage.data;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
	else if (newUserHandScore == newDealerHandScore) {

		string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
	else {
		unsigned int payAmount = -1 * betAmount;
		discordGuildMember.getDataFromDB().get();
		discordGuildMember.data.currency.wallet += payAmount;
		discordGuildMember.writeDataToDB().get();
		discordGuild.getDataFromDB().get();
		if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
			discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
		}
		discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild.data.casinoStats.totalPayout += payAmount;
		discordGuild.writeDataToDB().get();
		string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.embed = msgEmbed;
		editData.originalMessageData = newMessage.data;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;

	}
}

task<void> executeCheckResponse(DiscordCoreAPI::Button button, DiscordCoreAPI::DiscordGuildMember discordGuildMember, unsigned int betAmount, DiscordCoreAPI::GuildMember guildMember, DiscordCoreAPI::DiscordGuild discordGuild, DiscordCoreInternal::InputEventData newEvent, DiscordCoreAPI::BaseFunctionArguments* args,
	unsigned int newCardCount, vector<DiscordCoreAPI::Card> userHand, vector<unsigned int>* userAceIndices, vector<unsigned int>* dealerAceIndices, string userID, vector<DiscordCoreAPI::Card> dealerHand, DiscordCoreInternal::ActionRowData actionRowData2) {
	
	discordGuildMember.getDataFromDB().get();
	unsigned int fineAmount = 0;
	fineAmount = 1 * betAmount;
	if (fineAmount > discordGuildMember.data.currency.wallet) {
		string inPlayFooterString;
		inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		if (newEvent.eventType == DiscordCoreInternal::InputEventType::REGULAR_MESSAGE) {
			msgEmbed.setDescription(newEvent.messageData.embeds[0].description);
			msgEmbed.addField(newEvent.messageData.embeds.at(0).fields.at(0).name, newEvent.messageData.embeds.at(0).fields.at(0).value, newEvent.messageData.embeds.at(0).fields.at(0).Inline);
			msgEmbed.addField(newEvent.messageData.embeds.at(0).fields.at(1).name, newEvent.messageData.embeds.at(0).fields.at(1).value, newEvent.messageData.embeds.at(0).fields.at(1).Inline);
		}
		
		
		DiscordCoreInternal::InputEventResponseData responseData(DiscordCoreInternal::InputEventResponseType::REGULAR_MESSAGE_EDIT);
		responseData.embeds.push_back(msgEmbed);
		responseData.editMessageData.originalMessageData = newMessage.data;
		args->eventData.discordCoreClient->messages->editMessageAsync(editMessageData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}

	newCardCount += 1;
	discordGuild.getDataFromDB().get();
	userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
	discordGuild.writeDataToDB().get();

	checkAndSetAceValues(&userHand, userAceIndices);

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand.size(); x += 1) {
		newUserHandScore += userHand[x].value;
	}

	if (newUserHandScore > 21) {
		unsigned int payAmount = (unsigned int)((float)betAmount * -1.0);
		discordGuildMember.getDataFromDB().get();
		discordGuildMember.data.currency.wallet += payAmount;
		discordGuildMember.writeDataToDB().get();
		discordGuild.getDataFromDB().get();
		if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
			discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
		}
		discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild.data.casinoStats.totalPayout += payAmount;
		discordGuild.writeDataToDB().get();

		unsigned int newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand.size(); x += 1) {
			newDealerHandScore += dealerHand[x].value;
		}

		string dealerHandString;
		for (auto x = 0; x < dealerHand.size(); x += 1) {
			dealerHandString += dealerHand[x].suit + dealerHand[x].type;
		}

		string userHandString;
		for (auto x = 0; x < userHand.size(); x += 1) {
			userHandString += userHand[x].suit + userHand[x].type;
		}

		string bustFooterString;
		bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		DiscordCoreAPI::EditMessageData editMessageData;
		editMessageData.embed = msgEmbed;
		editMessageData.originalMessageData = newMessage.data;
		args->eventData.discordCoreClient->messages->editMessageAsync(editMessageData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
	else if (newUserHandScore == 21) {
		unsigned int newDealerHandScore = 0;
		while (newDealerHandScore < 17) {
			for (auto x = 0; x < dealerHand.size(); x += 1) {
				newDealerHandScore += dealerHand[x].value;
			}
			if (newDealerHandScore >= 17) {
				break;
			}
			discordGuild.getDataFromDB().get();
			dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
			discordGuild.writeDataToDB().get();

			checkAndSetAceValues(&dealerHand, dealerAceIndices);
		}

		newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand.size(); x += 1) {
			newDealerHandScore += dealerHand[x].value;
		}

		if (newDealerHandScore == 21) {
			string dealerHandString;
			for (auto x = 0; x < dealerHand.size(); x += 1) {
				dealerHandString += dealerHand[x].suit + dealerHand[x].type;
			}

			string userHandString;
			for (auto x = 0; x < userHand.size(); x += 1) {
				userHandString += userHand[x].suit + userHand[x].type;
			}

			string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

			DiscordCoreInternal::EmbedData msgEmbed;
			msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Tie:**__");
			msgEmbed.setColor("0000FF");
			msgEmbed.setFooter("Cards Remaining: " + discordGuild.data.blackjackStack.size());
			msgEmbed.setDescription(newMessage.data.embeds[0].description);
			msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
			DiscordCoreAPI::EditMessageData editMessageData;
			editMessageData.embed = msgEmbed;
			editMessageData.originalMessageData = newMessage.data;
			args->eventData.discordCoreClient->messages->editMessageAsync(editMessageData, newMessage.data.channelId, newMessage.data.id).get();
			co_return;
		}
		else {
			unsigned int payAmount = betAmount;
			discordGuildMember.getDataFromDB().get();
			discordGuildMember.data.currency.wallet += payAmount;
			discordGuildMember.writeDataToDB().get();
			discordGuild.getDataFromDB().get();
			if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
				discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
				discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
				discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
			}
			discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild.data.casinoStats.totalPayout = payAmount;
			discordGuild.writeDataToDB();

			string dealerHandString;
			for (auto x = 0; x < dealerHand.size(); x += 1) {
				dealerHandString += dealerHand[x].suit + dealerHand[x].type;
			}

			string userHandString;
			for (auto x = 0; x < userHand.size(); x += 1) {
				userHandString += userHand[x].suit + userHand[x].type;
			}

			string winFooterString;
			winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " +
				args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

			DiscordCoreInternal::EmbedData msgEmbed;
			msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Win:**__");
			msgEmbed.setColor("00FF00");
			msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
			msgEmbed.setDescription(newMessage.data.embeds[0].description);
			msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
			DiscordCoreAPI::EditMessageData editData;
			editData.embed = msgEmbed;
			editData.originalMessageData = newMessage.data;
			args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
			co_return;
		}
	}
	else if (newUserHandScore < 21) {
		unsigned int newDealerHandScore = dealerHand[0].value;

		string dealerHandString;
		dealerHandString += dealerHand[0].suit + dealerHand[0].type;

		string userHandString;
		for (auto x = 0; x < userHand.size(); x += 1) {
			userHandString += userHand[x].suit + userHand[x].type;
		}

		string inPlayFooterString;
		inPlayFooterString = "------\n------";
		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		if(newMessage.data.embeds.at(0).description[0] != (char)0x40){
			msgEmbed.setDescription(newMessage.data.embeds[0].description);
		}			
		cout << (char)0x40 << endl;
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: In Play**__", inPlayFooterString, false);
		DiscordCoreInternal::InputEventResponseData responseData(DiscordCoreInternal::InputEventResponseType::REGULAR_MESSAGE_EDIT);
		DiscordCoreAPI::EditMessageData editData;
		editData.embed = msgEmbed;
		editData.originalMessageData = newMessage.data;
		editData.components.push_back(actionRowData2);
		DiscordCoreAPI::Message newerMessage = args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		DiscordCoreAPI::Button button2();
		DiscordCoreAPI::ButtonInteractionData buttonInteractionData = button2.getOurButtonData(60000);
		if (buttonInteractionData.customId == "") {
			string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
			DiscordCoreInternal::EmbedData msgEmbed2;
			msgEmbed2.setColor("FF0000");
			msgEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed2.setTitle("__**Blackjack Game:**__");
			msgEmbed2.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
			msgEmbed2.setDescription(timeOutString);
			vector<DiscordCoreInternal::EmbedData> embeds;
			embeds.push_back(msgEmbed2);
			newMessage.data.author = args->message.data.author;
			DiscordCoreAPI::Message newestMessage = args->eventData.discordCoreClient->messages->editMessageAsync({ .embed = msgEmbed2,.originalMessageData = newMessage.data, .components = vector<DiscordCoreAPI::ActionRowData>() }, args->message.data.channelId, newMessage.data.id).get();
			args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = newestMessage.data.channelId, .messageId = newestMessage.data.id, .timeDelay = 20000 }).get();
			co_return;
		}
		if (button2.getButtonId()== "check") {
			executeCheckResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, userAceIndices, dealerAceIndices, userID, dealerHand, actionRowData2);
		}
		else if (button2.getButtonId() == "cross") {
			executeCrossResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, userAceIndices, dealerAceIndices, userID, dealerHand, actionRowData2);
		}
	}
}

task<void> executeDoubleResponse(DiscordCoreAPI::Button button, DiscordCoreAPI::DiscordGuildMember discordGuildMember, unsigned int betAmount, DiscordCoreAPI::GuildMember guildMember, DiscordCoreAPI::DiscordGuild discordGuild, DiscordCoreAPI::Message newMessage, DiscordCoreAPI::BaseFunctionArguments* args,
	unsigned int newCardCount, vector<DiscordCoreAPI::Card> userHand, vector<unsigned int>* userAceIndices, vector<unsigned int>* dealerAceIndices, string userID, vector<DiscordCoreAPI::Card> dealerHand, DiscordCoreAPI::ActionRowData actionRowData2) {
	unsigned int fineAmount = 2 * betAmount;
	if (fineAmount > discordGuildMember.data.currency.wallet || newCardCount > 2) {

		string failedFooterString;
		discordGuildMember.getDataFromDB().get();
		if ((newMessage.data.embeds[0].fields[2].value.find("⏬") == string::npos) || newCardCount > 2) {
			failedFooterString = "__***Sorry, but you do not have the option to double down!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		}
		else if (fineAmount > discordGuildMember.data.currency.wallet) {
			failedFooterString = "__***Sorry, but you have insufficient funds for placing that wager now!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		}

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("0000FF");
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField(newMessage.data.embeds[0].fields[0].name, newMessage.data.embeds[0].fields[0].value, newMessage.data.embeds[0].fields[0].Inline);
		msgEmbed.addField(newMessage.data.embeds[0].fields[1].name, newMessage.data.embeds[0].fields[1].value, newMessage.data.embeds[0].fields[1].Inline);
		msgEmbed.addField("__**Game Status: In Play**__", failedFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		executeCheckResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, userAceIndices, dealerAceIndices, userID, dealerHand, actionRowData2);
	};

	newCardCount += 1;
	discordGuild.getDataFromDB().get();
	userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
	discordGuild.writeDataToDB().get();

	checkAndSetAceValues(&userHand, userAceIndices);

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand.size(); x += 1) {
		newUserHandScore += userHand[x].value;
	}
	unsigned int newDealerHandScore = 0;
	while (newDealerHandScore < 17) {
		for (auto x = 0; x < dealerHand.size(); x += 1) {
			newDealerHandScore += dealerHand[x].value;
		}
		if (newDealerHandScore >= 17) {
			break;
		}
		discordGuild.getDataFromDB().get();
		dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
		discordGuild.writeDataToDB().get();

		checkAndSetAceValues(&dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand.size(); x += 1) {
		newDealerHandScore += dealerHand[x].value;
	}

	string dealerHandString;
	for (auto x = 0; x < dealerHand.size(); x += 1) {
		dealerHandString += dealerHand[x].suit + dealerHand[x].type;
	}

	string userHandString;
	for (auto x = 0; x < userHand.size(); x += 1) {
		userHandString += userHand[x].suit + userHand[x].type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21)
		|| (newUserHandScore < 21 && newUserHandScore > newDealerHandScore)
		|| (newUserHandScore < 21 && newDealerHandScore > 21)) {
		unsigned int payAmount = 2 * betAmount;

		discordGuildMember.data.currency.wallet += payAmount;
		discordGuildMember.writeDataToDB().get();
		discordGuild.getDataFromDB().get();
		if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
			discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
		}
		discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild.data.casinoStats.totalPayout += payAmount;
		discordGuild.writeDataToDB().get();

		string winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " "
			+ args->eventData.discordCoreClient->currentUser->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
	else if (newUserHandScore == newDealerHandScore) {
		string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
	else {
		unsigned int payAmount = -2 * betAmount;

		discordGuildMember.data.currency.wallet += payAmount;
		discordGuildMember.writeDataToDB().get();
		discordGuild.getDataFromDB().get();
		if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
			discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
		}
		discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild.data.casinoStats.totalPayout += payAmount;
		discordGuild.writeDataToDB().get();

		string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreInternal::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.getAvatarURL());
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
		msgEmbed.setDescription(newMessage.data.embeds[0].description);
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		DiscordCoreAPI::EditMessageData editData;
		editData.originalMessageData = newMessage.data;
		editData.embed = msgEmbed;
		args->eventData.discordCoreClient->messages->editMessageAsync(editData, newMessage.data.channelId, newMessage.data.id).get();
		co_return;
	}
}

namespace DiscordCoreAPI {
	class Blackjack :public BaseFunction {
	public:
		Blackjack() {
			this->commandName = "blackjack";
			this->helpDescription = "__**Blackjack:**__ Enter !blackjack = BETAMOUNT, or /blackjack BETAMOUNT.";
		}
		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(args->eventData.messageData, args->eventData.discordCoreClient, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->message.data.messageType != DiscordCoreInternal::MessageTypeReal::SLASH_COMMAND_INTERACTION) {
					args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = args->message.data.channelId, .messageId = args->message.data.id, .timeDelay = 0 });
				}

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->message.data.guildId }).get();
				DiscordGuild discordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->message, discordGuild.data);

				if (areWeAllowed == false) {
					co_return;
				}

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->message.data.author.id }).get();
				GuildMember botMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->message.data.guildId, .guildMemberId = args->eventData.discordCoreClient->discordUser->data.userId }).get();
				if (!DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, DiscordCoreInternal::Permissions::MANAGE_MESSAGES)) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
					Message msg = args->eventData.discordCoreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 200000 }).get();
					co_return;
				}

				regex betRegExp("\\d{1,18}");
				if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), betRegExp) || stoll(args->argumentsArray.at(0)) < 1) {
					string msgString = "------\n**Please enter a valid bet amount!(!blackjack = BETAMOUNT)**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					Message msg = args->eventData.discordCoreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 200000 }).get();
					co_return;
				}

				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, betRegExp);
				unsigned int betAmount = (unsigned int)stoll(matchResults.str());

				srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

				string userID = guildMember.data.user.id;
				DiscordCoreAPI::DiscordGuildMember discordGuildMember(guildMember.data);

				if (betAmount > discordGuildMember.data.currency.wallet) {
					string msgString = "------\n**Sorry, but you have insufficient funds for placing that wager!**\n------";
					DiscordCoreInternal::EmbedData msgEmbed;
					msgEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Funds:**__");
					Message msg = args->eventData.discordCoreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = msgEmbed }).get();
					args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = msg.data.channelId, .messageId = msg.data.id, .timeDelay = 200000 }).get();
					co_return;
				}

				string finalMsgString;
				finalMsgString = "__**Your Bet Amount:**__ " + to_string(betAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n";

				string footerMsgStringOld = "------\n------";

				discordGuild.getDataFromDB().get();
				vector<Card> userHand;
				vector<unsigned int> userAceIndices;
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				unsigned int userHandScore = userHand.at(0).value + userHand.at(1).value;
				discordGuild.writeDataToDB().get();

				vector<Card> dealerHand;
				vector<unsigned int> dealerAceIndices;
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				unsigned int newDealerHandScore = dealerHand[0].value;
				discordGuild.writeDataToDB().get();
				string footerMsgString = footerMsgStringOld;

				Message newMessage;
				if (userHandScore == 21) {
					if (dealerHand[0].value == 10 && dealerHand[1].type == "Ace") {
						dealerHand[1].value = 11;
					}
					else if (dealerHand[1].value == 10 && dealerHand[0].type == "Ace") {
						dealerHand[0].value = 11;
					}
					newDealerHandScore = dealerHand[0].value + dealerHand[1].value;
					string footerMsgString2;
					if (newDealerHandScore == 21) {
						footerMsgString2 = "\n------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

						DiscordCoreInternal::EmbedData finalMessageEmbed;
						finalMessageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
						finalMessageEmbed.setTimeStamp(getTimeAndDate());
						finalMessageEmbed.setColor("0000FF");
						finalMessageEmbed.setDescription(finalMsgString);
						finalMessageEmbed.setTitle("__**Blackjack Tie:**__");
						finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
						DiscordCoreAPI::EmbedFieldData field01;
						field01.name = "Dealer's Hand: " + to_string(newDealerHandScore);
						field01.value = dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type;
						field01.Inline = true;
						finalMessageEmbed.fields.push_back(field01);
						DiscordCoreAPI::EmbedFieldData field02;
						field02.name = "Player's Hand: " + to_string(userHandScore);
						field02.value = userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type;
						field02.Inline = true;
						finalMessageEmbed.fields.push_back(field02);
						DiscordCoreAPI::EmbedFieldData field03;
						field03.name = "__**Game Status: Tie**__";
						field03.value = footerMsgString2;
						field03.Inline = false;
						finalMessageEmbed.fields.push_back(field03);

						newMessage = args->eventData.discordCoreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = finalMessageEmbed }).get();
						co_return;
					}

					discordGuildMember.getDataFromDB().get();
					unsigned int payAmount = (unsigned int)trunc(1.5 * betAmount);
					discordGuildMember.data.currency.wallet += payAmount;
					discordGuildMember.writeDataToDB().get();
					discordGuild.getDataFromDB().get();
					if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
						discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
						discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
						discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
					}
					discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
					discordGuild.data.casinoStats.totalPayout += payAmount;
					discordGuild.writeDataToDB().get();

					footerMsgString2 = "\n------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " +
						to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

					DiscordCoreInternal::EmbedData finalMessageEmbed;
					finalMessageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					finalMessageEmbed.setTimeStamp(getTimeAndDate());
					finalMessageEmbed.setColor("00FF00");
					finalMessageEmbed.setDescription(finalMsgString);
					finalMessageEmbed.setTitle("__**Blackjack Win:**__");
					finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
					finalMessageEmbed.addField("Dealer's Hand: ", dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
					finalMessageEmbed.addField("Player's Hand: ", userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
					finalMessageEmbed.addField("__**Game Status: Player Wins**__", footerMsgString2, false);
					newMessage = args->eventData.discordCoreClient->messages->replyAsync({ .replyingToMessageData = args->message.data, .embed = finalMessageEmbed }).get();
					co_return;
				}
				bool canWeDoubleDown = false;
				if ((userHandScore == 9) || (userHandScore == 10) || (userHandScore == 11)) {
					canWeDoubleDown = true;
				}
				DiscordCoreInternal::EmbedData finalMessageEmbed;
				finalMessageEmbed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
				finalMessageEmbed.setTimeStamp(getTimeAndDate());
				finalMessageEmbed.setColor("00FF00");
				finalMessageEmbed.setDescription(finalMsgString);
				finalMessageEmbed.setTitle("__**Blackjack:**__");
				finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
				DiscordCoreAPI::EmbedFieldData field01;
				field01.name = "Dealer's Hand: " + to_string(newDealerHandScore);
				field01.value = dealerHand[0].suit + dealerHand[0].type;
				field01.Inline = true;
				finalMessageEmbed.fields.push_back(field01);
				DiscordCoreAPI::EmbedFieldData field02;
				field02.name = "Player's Hand: " + to_string(userHandScore);
				field02.value = userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type;
				field02.Inline = true;
				finalMessageEmbed.fields.push_back(field02);
				DiscordCoreAPI::EmbedFieldData field03;
				field03.name = "__**Game Status: In Play**__";
				field03.value = footerMsgString;
				field03.Inline = false;
				finalMessageEmbed.fields.push_back(field03);
				DiscordCoreAPI::ActionRowData actionRowDataOld;
				DiscordCoreAPI::ActionRowData actionRowData;
				DiscordCoreAPI::ComponentData componentData01;
				componentData01.disabled = false;
				componentData01.customId = "check";
				componentData01.emoji.name = "✅";
				componentData01.label = "Hit";
				componentData01.style = DiscordCoreAPI::ButtonStyle::Success;
				componentData01.type = DiscordCoreAPI::ComponentType::Button;
				actionRowDataOld.components.push_back(componentData01);
				actionRowData.components.push_back(componentData01);
				DiscordCoreAPI::ComponentData componentData02;
				componentData02.disabled = false;
				componentData02.customId = "cross";
				componentData02.emoji.name = "❎";
				componentData02.label = "Stand";
				componentData02.style = DiscordCoreAPI::ButtonStyle::Success;
				componentData02.type = DiscordCoreAPI::ComponentType::Button;
				actionRowData.components.push_back(componentData02);
				actionRowDataOld.components.push_back(componentData02);
				ReplyMessageData replyMessageData;
				replyMessageData.embed = finalMessageEmbed;
				replyMessageData.replyingToMessageData = args->message.data;
				DiscordCoreAPI::ComponentData componentData03;
				vector<DiscordCoreAPI::ActionRowData> actionRows;
				unsigned int newCardCount = 2;
				if (canWeDoubleDown) {
					componentData03.disabled = false;
					componentData03.customId = "double";
					componentData03.emoji.name = "⏬";
					componentData03.label = "Double-Down";
					componentData03.style = DiscordCoreAPI::ButtonStyle::Primary;
					componentData03.type = DiscordCoreAPI::ComponentType::Button;
					actionRowData.components.push_back(componentData03);
				}
				actionRows.push_back(actionRowData);
				replyMessageData.components = actionRows;
				newMessage = args->eventData.discordCoreClient->messages->replyAsync(replyMessageData).get();
				Button button(newMessage.data);
				DiscordCoreAPI::ButtonInteractionData buttonInteractionData = button.getOurButtonData(60000);
				if (buttonInteractionData.customId == "") {
					string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					DiscordCoreInternal::EmbedData msgEmbed2;
					msgEmbed2.setColor("FF0000");
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setTitle("__**Blackjack Game:**__");
					msgEmbed2.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
					msgEmbed2.setDescription(timeOutString);
					vector<DiscordCoreInternal::EmbedData> embeds;
					embeds.push_back(msgEmbed2);
					newMessage.data.author = args->message.data.author;
					DiscordCoreAPI::Message newerMessage = args->eventData.discordCoreClient->messages->editMessageAsync({ .embed = msgEmbed2,.originalMessageData = newMessage.data, .components = vector<DiscordCoreAPI::ActionRowData>() }, args->message.data.channelId, newMessage.data.id).get();
					args->eventData.discordCoreClient->messages->deleteMessageAsync({ .channelId = newerMessage.data.channelId, .messageId = newerMessage.data.id, .timeDelay = 20000 }).get();
					co_return;
				}

				EditMessageData editData;
				editData.embed = finalMessageEmbed;
				editData.originalMessageData = newMessage.data;
				editData.components.push_back(actionRowDataOld);
				finalMessageEmbed.fields.at(2).value = footerMsgStringOld;
				newMessage = args->eventData.discordCoreClient->messages->editMessageAsync(editData, args->message.data.channelId, newMessage.data.id).get();
				if (button.getButtonId()== "check") {
					cout << "CHECK CHECK CHECK:" << endl;
					executeCheckResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, &userAceIndices, &dealerAceIndices, userID, dealerHand, actionRowDataOld).get();
				}
				else if (button.getButtonId() == "cross") {
					cout << "CROSS CROSS CROSS:" << endl;
					executeCrossResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, &userAceIndices, &dealerAceIndices, userID, dealerHand, actionRowDataOld).get();
				}
				else if (button.getButtonId()== "double") {
					cout << "DOUBLE DOUBLE DOUBLE:" << endl;
					executeDoubleResponse(button, discordGuildMember, betAmount, guildMember, discordGuild, newMessage, args, newCardCount, userHand, &userAceIndices, &dealerAceIndices, userID, dealerHand, actionRowDataOld).get();
				};
				co_return;
			}
				catch (exception& e) {
					cout << "Blackjack::execute() Error: " << e.what() << endl << endl;
				}
			};
			
		};
		Blackjack blackJack{};
	};
#endif
