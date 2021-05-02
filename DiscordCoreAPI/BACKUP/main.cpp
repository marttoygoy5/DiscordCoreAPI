// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreAPI.hpp"

task<void> createdMessageEvent(CommanderNS::EventDataTypes::MessageCreationData messageData) {
    co_await resume_background();
    concurrency::critical_section criticalSection;
    criticalSection.lock();
    if (messageData.message.Data.content.c_str()[0] == '!') {
        CommanderNS::ClientDataTypes::CreateMessageData createMessageData;
        createMessageData.content = "TESTING TESTING!";
        createMessageData.tts = true;
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
        shared_ptr<CommanderNS::ClientClasses::MessageManager> messageManager = messageData.message.pMessageManager;
        CommanderNS::ClientClasses::Message message = messageManager->CreateMessage(createMessageData).get();
        messageData.message.DeleteMessage().get();
        message.Reactions.AddReaction({ "ZeedenAndDoom" ,"805151947131715654" }).get();
        message.Reactions.AddReaction({ "🧪" }).get();
        message.Reactions.AddReaction({ "🔫" }).get();
        co_return;
    }
};
/*
void createdMessageEvent(CommanderNS::EventDataTypes::MessageCreationData messageData) {
    cout << "THREAD ID 01: " << this_thread::get_id() << endl;
    if (messageData.message.Data.content.c_str()[0] == '!') {
        CommanderNS::ClientDataTypes::CreateMessageData createMessageData;
        createMessageData.content = "TESTING TESTING!";
        createMessageData.tts = true;
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
        CommanderNS::ClientClasses::Message message = messageManager->CreateMessage(createMessageData).get();
        messageData.message.DeleteMessage().get();
        message.Reactions.AddReaction({ "ZeedenAndDoom" ,"805151947131715654" }).get();
        message.Reactions.AddReaction({ "🧪" }).get();
        message.Reactions.AddReaction({ "🔫" }).get();
        message.Reactions.AddReaction({ "🚀" }).get();
        message.Reactions.AddReaction({ "❤" }).get();
        message.Reactions.AddReaction({ "❗" }).get();
        message.Reactions.AddReaction({ "🌎" }).get();
        return;
    }
};
*/
int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.bkkqDCVr3eRb8jTNsgkQuDUaDCY";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    // Do other work here.    


    //pdiscordCoreAPI->eventMachine->onMessageCreation(createdMessageEvent);
    //pdiscordCoreAPI->eventMachine->onGuildMemberAdd([](CommanderNS::EventDataTypes::GuildMemberAddData guildMember) {std::cout << guildMember.guildMember.Data.user.username << std::endl; });
    pdiscordCoreAPI->login();
    return 1;
}
