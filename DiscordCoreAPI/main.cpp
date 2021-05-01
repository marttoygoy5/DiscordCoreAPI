// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreAPI.hpp"

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.xRMrt8Mte-SIF_luQYTA9YH6BoI";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    // Do other work here.
    pdiscordCoreAPI->eventMachine->onMessageCreation([](CommanderNS::EventDataTypes::MessageCreationData message) {
        CommanderNS::ClientDataTypes::CreateMessageData createMessageData;
            if (message.message.Data.content.c_str()[0] == '!') {
                createMessageData.content = "TESTING TESTING!";
                createMessageData.tts = true;
                CommanderNS::ClientDataTypes::EmbedFieldData embedField;
                embedField.Inline = true;
                embedField.name = "__**TEST FIELD!**__";
                embedField.value = "**TEST VALUE!**";
                createMessageData.embed.fields.push_back(embedField);                
                embedField.Inline = true;
                embedField.name = "ANOTHER FIELD!";
                embedField.value = "WE ARE TESTING THE VALUES!";
                createMessageData.embed.fields.push_back(embedField);
                createMessageData.embed.color[0] = 0;
                createMessageData.embed.color[1] = 255;
                createMessageData.embed.color[2] = 255;
                CommanderNS::ClientClasses::MessageManager* messageManager = (CommanderNS::ClientClasses::MessageManager*)(message.message.messageManager);
                CommanderNS::ClientClasses::Message message = messageManager->CreateMessage(createMessageData).get();
                message.Reactions.AddReaction("⚔️").get();
                message.Reactions.AddReaction("🔫").get();
                message.Reactions.AddReaction("🧪").get();
            };
        });
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd([](CommanderNS::EventDataTypes::GuildMemberAddData guildMember) {std::cout << guildMember.guildMember.Data.user.username << std::endl; });
    pdiscordCoreAPI->login();
}
