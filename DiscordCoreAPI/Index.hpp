// Index.hpp - An index file to round everything up.
// Jun 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INDEX_
#define _INDEX_

#include "DiscordCoreClient.hpp"
#include "Commands\CommandsList.hpp"

namespace DiscordCoreAPI {

    task<void> onInputEventCreated(DiscordCoreAPI::OnInputEventCreateData dataPackage) {
        apartment_context mainThread;
        co_await resume_background();
        try {
            if (dataPackage.eventData.eventType != DiscordCoreAPI::InputEventType::BUTTON_INTERACTION) {
                DiscordCoreAPI::CommandData commandData(dataPackage.eventData);
                commandData.eventData = dataPackage.eventData;
                DiscordCoreAPI::CommandController::checkForAndRunCommands(commandData);
            }
            else {
                DiscordCoreAPI::ButtonInteractionData dataPackageNew;
                dataPackageNew.applicationId = dataPackage.eventData.interactionData.applicationId;
                dataPackageNew.channelId = dataPackage.eventData.getChannelId();
                dataPackageNew.customId = dataPackage.eventData.interactionData.customId;
                dataPackageNew.guildId = dataPackage.eventData.getGuildId();
                dataPackageNew.id = dataPackage.eventData.interactionData.id;
                dataPackageNew.member = dataPackage.eventData.interactionData.member;
                dataPackageNew.message = dataPackage.eventData.interactionData.message;
                dataPackageNew.token = dataPackage.eventData.interactionData.token;
                dataPackageNew.type = dataPackage.eventData.interactionData.type;
                dataPackageNew.user = dataPackage.eventData.interactionData.user;
                send(DiscordCoreAPI::InteractionManager::buttonInteractionBuffer, dataPackageNew);
            }
            co_await mainThread;
            co_return;
        }
        catch (exception& e) {
            cout << "onMessageCreated() Error: " << e.what() << endl << endl;
            co_return;
        }
    }

    void onGuildCreated(DiscordCoreAPI::OnGuildCreationData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
        return;
    }

    void onReactionAdded(DiscordCoreAPI::OnReactionAddData dataPackage) {
        dataPackage.reaction.discordCoreClient->reactions->insertReactionAsync(dataPackage.reaction).get();
        return;
    }

    void onGuildMemberAdded(DiscordCoreAPI::OnGuildMemberAddData dataPackage) {
        dataPackage.guildMember.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMember).get();
        return;
    }

    shared_ptr<DiscordCoreAPI::DiscordCoreClient> DiscordCoreClient::finalSetup(string botToken) {
        shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = make_shared<DiscordCoreAPI::DiscordCoreClient>(to_hstring(botToken));
        pDiscordCoreClient->login();
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopRole, DiscordCoreAPI::addShopRole.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopItem, DiscordCoreAPI::addShopItem.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::balance, DiscordCoreAPI::balance.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::help, DiscordCoreAPI::help.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::shop, DiscordCoreAPI::shop.commandName);
        pDiscordCoreClient->eventManager->onInputEventCreate(onInputEventCreated);
        pDiscordCoreClient->eventManager->onGuildCreation(onGuildCreated);
        pDiscordCoreClient->eventManager->onReactionAdd(onReactionAdded);
        pDiscordCoreClient->eventManager->onGuildMemberAdd(onGuildMemberAdded);
        return pDiscordCoreClient;
	};
}
#endif