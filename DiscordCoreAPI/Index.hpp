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

    void onChannelCreation(OnChannelCreationData dataPackage) {
        dataPackage.channel.discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
        return;
    }

    void onChannelUpdate(OnChannelUpdateData dataPackage) {
        dataPackage.channelNew.discordCoreClient->channels->insertChannelAsync(dataPackage.channelNew).get();
        return;
    }

    void onChannelDeletion(OnChannelDeletionData dataPackage) {
        dataPackage.channel.discordCoreClient->channels->removeChannelAsync(dataPackage.channel.data.id).get();
        return;
    }

    void onGuildCreation(OnGuildCreationData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
        return;
    }

    void onGuildUpdate(OnGuildUpdateData dataPackage) {
        dataPackage.guildNew.discordCoreClient->guilds->insertGuildAsync(dataPackage.guildNew).get();
        return;
    }

    void onGuildDeletion(OnGuildDeletionData dataPackage) {
        dataPackage.guild.discordCoreClient->guilds->removeGuildAsync(dataPackage.guild.data.id).get();
        return;
    }

    void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
        dataPackage.guildMember.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMember).get();
        return;
    }

    void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
        dataPackage.user.discordCoreClient->guildMembers->removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.data.id).get();
        return;
    }

    void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
        dataPackage.guildMemberNew.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMemberNew).get();
        return;
    }

    void onRoleCreation(OnRoleCreationData dataPackage) {
        dataPackage.role.discordCoreClient->roles->insertRoleAsync(dataPackage.role).get();
        return;
    }

    void onRoleUpdate(OnRoleUpdateData dataPackage) {
        dataPackage.roleNew.discordCoreClient->roles->insertRoleAsync(dataPackage.roleNew).get();
        return;
    }

    void onRoleDeletion(OnRoleDeletionData dataPackage) {
        dataPackage.roleOld.discordCoreClient->roles->removeRoleAsync(dataPackage.roleOld.data.id).get();
        return;
    }

    void onMessageCreation(OnMessageCreationData dataPackage) {
        dataPackage.message.discordCoreClient->messages->insertMessageAsync(dataPackage.message).get();
        return;
    }

    void onMessageUpdate(OnMessageUpdateData dataPackage) {
        dataPackage.messageNew.discordCoreClient->messages->insertMessageAsync(dataPackage.messageNew).get();
        return;
    }

    void onMessageDeletion(OnMessageDeletionData dataPackage) {
        dataPackage.discordCoreClient->messages->removeMessageAsync(dataPackage.channelId, dataPackage.messageId).get();
        return;
    }

    task<void> onInputEventCreation(OnInputEventCreationData dataPackage) {
        apartment_context mainThread;
        co_await resume_background();
        try {
            if (dataPackage.eventData.eventType != InputEventType::BUTTON_INTERACTION) {
                DiscordCoreAPI::CommandData commandData(dataPackage.eventData);
                commandData.eventData = dataPackage.eventData;
                DiscordCoreAPI::CommandController::checkForAndRunCommands(commandData);
            }
            else {
                DiscordCoreAPI::ButtonInteractionData dataPackageNew;
                dataPackageNew.applicationId = dataPackage.eventData.getApplicationId();
                dataPackageNew.channelId = dataPackage.eventData.getChannelId();
                dataPackageNew.customId = dataPackage.eventData.interactionData.customId;
                dataPackageNew.guildId = dataPackage.eventData.getGuildId();
                dataPackageNew.id = dataPackage.eventData.getInteractionId();
                dataPackageNew.member = dataPackage.eventData.interactionData.member;
                dataPackageNew.message = dataPackage.eventData.interactionData.message;
                dataPackageNew.token = dataPackage.eventData.getInteractionToken();
                dataPackageNew.type = dataPackage.eventData.interactionData.type;
                dataPackageNew.user = dataPackage.eventData.interactionData.user;
                send(Button::buttonInteractionBuffer, dataPackageNew);
            }
            co_await mainThread;
            co_return;
        }
        catch (exception& e) {
            cout << "onMessageCreated() Error: " << e.what() << endl << endl;
            co_return;
        }
    }

    void onReactionAdd(OnReactionAddData dataPackage) {
        dataPackage.reaction.discordCoreClient->reactions->insertReactionAsync(dataPackage.reaction).get();
        return;
    }

    shared_ptr<DiscordCoreClient> DiscordCoreClient::finalSetup(string botToken) {
        shared_ptr<DiscordCoreClient> pDiscordCoreClient{ nullptr };
        try {
            pDiscordCoreClient = make_shared<DiscordCoreClient>(to_hstring(botToken));
            pDiscordCoreClient->login();
            pDiscordCoreClient->eventManager->onChannelCreation(onChannelCreation);
            pDiscordCoreClient->eventManager->onChannelUpdate(onChannelUpdate);
            pDiscordCoreClient->eventManager->onChannelDeletion(onChannelDeletion);
            pDiscordCoreClient->eventManager->onGuildCreation(onGuildCreation);
            pDiscordCoreClient->eventManager->onGuildUpdate(onGuildUpdate);
            pDiscordCoreClient->eventManager->onGuildDeletion(onGuildDeletion);
            pDiscordCoreClient->eventManager->onGuildMemberAdd(onGuildMemberAdd);
            pDiscordCoreClient->eventManager->onGuildMemberRemove(onGuildMemberRemove);
            pDiscordCoreClient->eventManager->onGuildMemberUpdate(onGuildMemberUpdate);
            pDiscordCoreClient->eventManager->onRoleCreation(onRoleCreation);
            pDiscordCoreClient->eventManager->onRoleUpdate(onRoleUpdate);
            pDiscordCoreClient->eventManager->onRoleDeletion(onRoleDeletion);
            pDiscordCoreClient->eventManager->onMessageCreation(onMessageCreation);
            pDiscordCoreClient->eventManager->onMessageUpdate(onMessageUpdate);
            pDiscordCoreClient->eventManager->onMessageDeletion(onMessageDeletion);
            pDiscordCoreClient->eventManager->onInputEventCreation(onInputEventCreation);
            pDiscordCoreClient->eventManager->onReactionAdd(onReactionAdd);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopRole, DiscordCoreAPI::addShopRole.commandName);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopItem, DiscordCoreAPI::addShopItem.commandName);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::balance, DiscordCoreAPI::balance.commandName);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::help, DiscordCoreAPI::help.commandName);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::shop, DiscordCoreAPI::shop.commandName);
            return pDiscordCoreClient;
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
            return pDiscordCoreClient;
        }
    };
}
#endif

