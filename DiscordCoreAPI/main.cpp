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
            cout << this_thread::get_id() << endl;
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
            embedField.value = "WE ARE TESTING;HE VALUES!";
            createMessageData.embed.fields.push_back(embedField);
            createMessageData.embed.color[0] = 0;
            createMessageData.embed.color[1] = 255;
            createMessageData.embed.color[2] = 255;
            CommanderNS::ClientClasses::MessageManager* messageManager = (CommanderNS::ClientClasses::MessageManager*)(messageData.message.messageManager);
            CommanderNS::ClientClasses::Message message = messageManager->CreateMessageAsync(createMessageData).get();
            CommanderNS::ClientClasses::Channel channel = pDiscordCoreAPI->Client.Guilds.Fetch(messageData.message.Data.guildId).get().Channels.Fetch(messageData.message.Data.channelId).get();
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
    DispatcherQueue threadQueue = *messageData.threadContext.threadQueue.get();
    cout << this_thread::get_id() << endl;
    co_await resume_foreground(threadQueue);
    cout << this_thread::get_id() << endl;
    task_handle taskHandle = make_task([messageData] { createdMessageEventTask(messageData).get(); });
    messageData.threadContext.taskGroup->run_and_wait(taskHandle);
    co_return;
}

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.rXnNRXWX3UOtOaP6YOiTMdoDXfk";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    pDiscordCoreAPI.attach(pdiscordCoreAPI.get());
    // Do other work here
    pDiscordCoreAPI->eventMachine->onMessageCreation(scheduleMessageTask);
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd([](CommanderNS::EventDataTypes::GuildMemberAddData guildMember) {std::cout << guildMember.guildMember.Data.user.username << std::endl; });
    pdiscordCoreAPI->login().get();
    return 1;
}