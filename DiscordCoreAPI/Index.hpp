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

    void onChannelCreation(DiscordCoreAPI::OnChannelCreationData dataPackage) {
        dataPackage.channel.discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
        return;
    }

    void onChannelUpdate(DiscordCoreAPI::OnChannelUpdateData dataPackage) {
        dataPackage.channel.discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
        return;
    }

    void onChannelDeletion(DiscordCoreAPI::OnChannelDeletionData dataPackage) {
        dataPackage.channel.discordCoreClient->channels->removeChannelAsync(dataPackage.channel.data.id).get();
        return;
    }

    void onGuildCreated(DiscordCoreAPI::OnGuildCreationData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
        return;
    }

    void onGuildUpdate(DiscordCoreAPI::OnGuildUpdateData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
        return;
    }

    void onGuildDeletion(DiscordCoreAPI::OnGuildDeletionData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->removeGuildAsync(dataPackage.guild.data.id).get();
        return;
    }

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
                cout << "BUTTON ID: " << dataPackageNew.customId << endl;
                send(DiscordCoreAPI::Button::buttonInteractionBuffer, dataPackageNew);
            }
            co_await mainThread;
            co_return;
        }
        catch (exception& e) {
            cout << "onMessageCreated() Error: " << e.what() << endl << endl;
            co_return;
        }
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
        pDiscordCoreClient->eventManager->onChannelCreation(onChannelCreation);
        pDiscordCoreClient->eventManager->onChannelUpdate(onChannelUpdate);
        pDiscordCoreClient->eventManager->onChannelDeletion(onChannelDeletion);
        pDiscordCoreClient->eventManager->onGuildCreation(onGuildCreated);
        pDiscordCoreClient->eventManager->onGuildUpdate(onGuildUpdate);
        pDiscordCoreClient->eventManager->onGuildDeletion(onGuildDeletion);
        pDiscordCoreClient->eventManager->onInputEventCreate(onInputEventCreated);
        pDiscordCoreClient->eventManager->onReactionAdd(onReactionAdded);
        pDiscordCoreClient->eventManager->onGuildMemberAdd(onGuildMemberAdded);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopRole, DiscordCoreAPI::addShopRole.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopItem, DiscordCoreAPI::addShopItem.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::balance, DiscordCoreAPI::balance.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::help, DiscordCoreAPI::help.commandName);
        DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::shop, DiscordCoreAPI::shop.commandName);        
        return pDiscordCoreClient;
	};
}
#endif