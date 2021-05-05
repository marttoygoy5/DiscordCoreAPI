// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreAPI.hpp"

com_ptr<CommanderNS::DiscordCoreAPI> pDiscordCoreAPI;

task<void> createdMessageEventTask(CommanderNS::EventDataTypes::MessageCreationData messageData) {
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
            CommanderNS::ClientClasses::MessageManager* messageManager = (CommanderNS::ClientClasses::MessageManager*)(messageData.message.messageManager);
            CommanderNS::ClientClasses::Message message = messageManager->CreateMessageAsync(createMessageData).get();
            messageData.message.DeleteMessageAsync().get();
            message.Reactions.AddReactionAsync({ "ZeedenAndDoom" ,"805151947131715654" })
                .then([message]() ->CommanderNS::ClientClasses::Message {return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "🔫" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "🧪" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "❤" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "🌎" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "🚀" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions.AddReactionAsync({ "❗" }).get(); return message; }).get();
            co_return;
        }
    }
    catch (exception error) {
        cout << "ERROR INFO: " << error.what() << endl;

    }

};

task<void> scheduleMessageTask(CommanderNS::EventDataTypes::MessageCreationData messageData) {
    task_handle taskHandle = make_task([messageData] { createdMessageEventTask(messageData).get(); });
    DispatcherQueue threadQueue = *messageData.threadContext.threadQueue.get();
    co_await resume_foreground(threadQueue);
    messageData.threadContext.taskGroup->run_and_wait(taskHandle);
    co_return;
}

task<void> scheduleGuildMemberAddTask(CommanderNS::EventDataTypes::GuildMemberAddData guildMemberAddData) {
    DispatcherQueue threadQueue = *guildMemberAddData.threadContext.threadQueue.get();
    co_await resume_foreground(threadQueue);
    task_handle taskHandle = make_task([guildMemberAddData] {std::cout << "THEIR NAME THEIR NAME THEIR NAME: " << guildMemberAddData.guildMember.Data.user.username << std::endl; });
    guildMemberAddData.threadContext.taskGroup->run_and_wait(taskHandle);
    co_return;    
}

task<void> onReactionAddFunction(CommanderNS::EventDataTypes::ReactionAddData reactionAddData) {
    try {
        DispatcherQueue threadQueue = *reactionAddData.threadContext.threadQueue.get();
        co_await resume_foreground(threadQueue);
        task_handle taskHandle = make_task([reactionAddData] {
            CommanderNS::ClientDataTypes::DeleteReactionData deleteReactionData;
            deleteReactionData.channelId = reactionAddData.reaction.Data.channelId;
            deleteReactionData.emojiName = reactionAddData.reaction.Data.emoji.name;
            deleteReactionData.emojiId = reactionAddData.reaction.Data.emoji.id;
            deleteReactionData.messageId = reactionAddData.reaction.Data.messageId;
            deleteReactionData.userId = reactionAddData.reaction.Data.userId;
            pDiscordCoreAPI->Client->Guilds.Fetch(reactionAddData.reaction.Data.guildId).get().Channels.Fetch(reactionAddData.reaction.Data.channelId).get()
                .messageManager->Fetch(reactionAddData.reaction.Data.channelId, reactionAddData.reaction.Data.messageId).get()
                .Reactions.DeleteUserReactionAsync(deleteReactionData).get();
            });
        reactionAddData.threadContext.taskGroup->run_and_wait(taskHandle);
        co_return;
    }
    catch (exception  error) {
        cout << "Error: " << error.what() << endl;
    }
}

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.I0oFrKHHo0TegRiPBZ609MeEQAI";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    pDiscordCoreAPI.attach(pdiscordCoreAPI.get());
    pDiscordCoreAPI->eventMachine->onMessageCreation(scheduleMessageTask);
    // Do other work here
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd(scheduleGuildMemberAddTask);
    pdiscordCoreAPI->eventMachine->onReactionAdd(onReactionAddFunction);
    pdiscordCoreAPI->login().get();
    return 1;
}