#include "pch.h"
#include "DiscordCoreAPI.hpp"

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.xRMrt8Mte-SIF_luQYTA9YH6BoI";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    // Do other work here.
    pdiscordCoreAPI->eventMachine->onMessageCreation([](CommanderNS::EventDataTypes::MessageCreationData message) {
            cout << "TESTING TESTING" << endl;
            CommanderNS::ClientDataTypes::CreateMessageData createMessageData;
            if (message.message.Data.content.c_str()[0] == '!') {
                createMessageData.content = "TESTING TESTING!";
                createMessageData.tts = true;
                CommanderNS::ClientClasses::MessageManager* messageManager = (CommanderNS::ClientClasses::MessageManager*)(message.message.messageManager);
                CommanderNS::ClientClasses::Message message = messageManager->CreateMessage(createMessageData).get();
            };
        });
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd([](CommanderNS::EventDataTypes::GuildMemberAddData guildMember) {std::cout << guildMember.guildMember.Data.user.username << std::endl; });
    pdiscordCoreAPI->login();
}
