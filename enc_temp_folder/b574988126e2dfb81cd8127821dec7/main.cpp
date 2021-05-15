// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreClient.hpp"

com_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreAPI;

task<void> createdMessageEventTask(DiscordCoreAPI::MessageCreationData messageData) {
    
    try {
        if (messageData.message.data.content.c_str()[0] == '!') {
            DiscordCoreInternal::CreateMessageData createMessageData;
            createMessageData.tts = true;
            createMessageData.content = "TESTING TESTING!";
            DiscordCoreInternal::EmbedFieldData embedField;
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
            DiscordCoreAPI::MessageManager* messageManager = (DiscordCoreAPI::MessageManager*)(&messageData.message.messages);
            DiscordCoreAPI::Message message = messageManager->createMessageAsync(createMessageData, messageData.message.data.channelId).get();
            messageData.message.deleteMessageAsync().get();
            message.reactions->createReactionAsync({ "ZeedenAndDoom" ,"805151947131715654" })
                .then([message]() ->DiscordCoreAPI::Message {return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "🔫" }).get(); return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "🧪" }).get(); return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "❤" }).get(); return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "🌎" }).get(); return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "🚀" }).get(); return message; })
                .then([](DiscordCoreAPI::Message message) {message.reactions->createReactionAsync({ "❗" }).get(); return message; }).get();
            co_return;
        }
    }
    catch (exception error) {
        cout << "ERROR INFO: " << error.what() << endl;

    }
    
    co_return;
};

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.KoplQLq-kp5ZBSgzjEZvdYXzaAs";
    winrt::com_ptr<DiscordCoreAPI::DiscordCoreClient> pdiscordCoreAPI = winrt::make_self<DiscordCoreAPI::DiscordCoreClient>(botToken);
    pDiscordCoreAPI.attach(pdiscordCoreAPI.get());
    // Do other work here
    pdiscordCoreAPI->login().get();
    pdiscordCoreAPI->EventMachine->onMessageCreation(createdMessageEventTask);
    agent::wait(pdiscordCoreAPI.get());
    return 1;
}