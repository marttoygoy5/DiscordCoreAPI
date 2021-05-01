#include "pch.h"
#include "DiscordCoreAPI.hpp"

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.xRMrt8Mte-SIF_luQYTA9YH6BoI";
    winrt::com_ptr<CommanderNS::DiscordCoreAPI> pdiscordCoreAPI = winrt::make_self<CommanderNS::DiscordCoreAPI>(botToken);
    // Do other work here.
    pdiscordCoreAPI->eventMachine->onMessageCreation([](CommanderNS::EventDataTypes::MessageCreationData messageData) {std::cout << messageData.messageData.content << std::endl; });
    pdiscordCoreAPI->eventMachine->onGuildMemberAdd([](CommanderNS::EventDataTypes::GuildMemberAddData guildMemberAddData) {std::cout << guildMemberAddData.guildMemberData.user.username << std::endl; });
    pdiscordCoreAPI->login();
    CommanderNS::ClientDataTypes::MessageData messageData;
}
