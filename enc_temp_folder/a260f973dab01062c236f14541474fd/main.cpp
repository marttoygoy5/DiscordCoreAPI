// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreAPI.hpp"

com_ptr<CommanderNS::DiscordCoreAPI> pDiscordCoreAPI;

task<void> scheduleMessageTask(CommanderNS::EventDataTypes::MessageCreationData messageData) {
    try {
        if (messageData.message.Data.content.c_str()[0] == '!') {
            CommanderNS::ClientDataTypes::CreateMessageData createMessageData;
            createMessageData.tts = true;
            createMessageData.content = "TESTING TESTING!";
            CommanderNS::ClientDataTypes::EmbedFieldData embedField;
            embedField.Inline = true;
            embedField.value = "**TEST VALUE!**";
            embedField.name = "__**TEST FIELD!**__";
            createMessageData.embed.fields.push_back(embedField);
            embedField.Inline = true;
            embedField.name = "ANOTHER FIELD!";
            embedField.value = "WE ARE TESTING THE VALUES!";
            createMessageData.embed.fields.push_back(embedField);
            createMessageData.embed.color[0] = 127;
            createMessageData.embed.color[1] = 0;
            createMessageData.embed.color[2] = 255;
            createMessageData.embed.thumbnail.url = "https://raw.githubusercontent.com/RealTimeChris/Unnamed-Renderer-DX12/main/Images/103%2C%203000SPP%2C%2030B%2C%20720p.png";
            createMessageData.embed.image.url = "https://raw.githubusercontent.com/RealTimeChris/Unnamed-Renderer-DX12/main/Images/124%2C%203000SPP%2C%2030B%2C%20720p.png";
            createMessageData.embed.title = "TEST TITLE!";
            CommanderNS::ClientClasses::MessageManager* messageManager = (CommanderNS::ClientClasses::MessageManager*)(messageData.message.messageManager);
            CommanderNS::ClientClasses::Message message = messageManager->createMessageAsync(createMessageData).get();
            messageData.message.deleteMessageAsync().get();
            message.Reactions.addReactionAsync({ "ZeedenAndDoom" ,"805151947131715654" })
                .then([message]() ->CommanderNS::ClientClasses::Message {return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "🔫" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "🧪" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "❤" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "🌎" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "🚀" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.addReactionAsync({ "❗" }).get(); return message; }).get();
            co_return;
        }
    }
    catch (exception error) {
        cout << "ERROR INFO: " << error.what() << endl;
    }
};

task<void> scheduleGuildMemberAddTask(CommanderNS::EventDataTypes::GuildMemberAddData guildMemberAddData) {
    std::cout << "THEIR NAME THEIR NAME THEIR NAME: " << guildMemberAddData.guildMember.Data.user.username << std::endl;
    co_return;
};

task<void> onReactionAddFunction(CommanderNS::EventDataTypes::ReactionAddData reactionAddData) {
    try {
        CommanderNS::ClientDataTypes::DeleteReactionData deleteReactionData;
        deleteReactionData.channelId = reactionAddData.reaction.Data.channelId;
        deleteReactionData.emojiName = reactionAddData.reaction.Data.emoji.name;
        deleteReactionData.emojiId = reactionAddData.reaction.Data.emoji.id;
        deleteReactionData.messageId = reactionAddData.reaction.Data.messageId;
        deleteReactionData.userId = reactionAddData.reaction.Data.userId;
        pDiscordCoreAPI->Client->Guilds.fetchAsync(reactionAddData.reaction.Data.guildId).get().Channels.fetchAsync(reactionAddData.reaction.Data.channelId).get()
            .messageManager->fetchAsync(reactionAddData.reaction.Data.channelId, reactionAddData.reaction.Data.messageId).get()
            .Reactions.deleteUserReactionAsync(deleteReactionData).get();
        co_return;
    }
    catch (exception  error) {
        cout << "Error: " << error.what() << endl;
    }
};

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.Rn7jfduCmDILniGAHMPBkF-kVVI";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    pDiscordCoreAPI.attach(pdiscordCoreAPI.get());
    // Do other work here
    pDiscordCoreAPI->eventMachine->onMessageCreation(scheduleMessageTask);
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd(scheduleGuildMemberAddTask);
    pdiscordCoreAPI->eventMachine->onReactionAdd(onReactionAddFunction);
    return pdiscordCoreAPI->login().get();
}
