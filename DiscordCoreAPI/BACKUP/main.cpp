// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreClient.hpp"

task<void> onMessageCreatedTask(DiscordCoreAPI::MessageCreationData messageData) {
    try {
        if (messageData.message.content.c_str()[0] == '!') {
            cout << this_thread::get_id() << endl;
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
            /*
            DiscordCoreAPI::Message = messageData.message.Guilds->getGuildAsync(messageData.message->Data.guildId).get()->Channels->getChannelAsync(messageData.message->Data.channelId).get()->Messages->createMessageAsync(createMessageData).get();
            messageData.message->deleteMessageAsync().get();
            message.Reactions->addReactionAsync({ "ZeedenAndDoom" ,"805151947131715654" })
                .then([message]() ->CommanderNS::ClientClasses::Message {return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "🔫" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "🧪" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "❤" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "🌎" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "🚀" }).get(); return message; })
                .then([](CommanderNS::ClientClasses::Message message) {message.Reactions->addReactionAsync({ "❗" }).get(); return message; }).get();
                */
            co_return;
        }
    }
    catch (exception error) {
        cout << "ERROR INFO: " << error.what() << endl;
    }
};

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.gG0DBR5qZHcd_glrXgSz_8nOCVc";
    winrt::com_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = winrt::make_self<DiscordCoreAPI::DiscordCoreClient>(botToken);
    // Do other work here.
    pDiscordCoreClient->login();
    agent::wait(pDiscordCoreClient.get());
    pDiscordCoreClient->pEventMachine->onMessageCreation(onMessageCreatedTask);
    return 0;
};
