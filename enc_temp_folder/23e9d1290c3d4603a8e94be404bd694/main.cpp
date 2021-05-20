// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreClient.hpp"

shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreAPI;

task<void> createdMessageEventTask(DiscordCoreAPI::MessageCreationData messageData) {
    try {
        if (messageData.message.data.content.c_str()[0] == '!') {
            DiscordCoreInternal::CreateMessageData createMessageData;
            createMessageData.tts = false;
            createMessageData.content = "TESTING TESTING TESTING!";
            DiscordCoreInternal::EmbedFieldData embedField;
            embedField.Inline = true;
            embedField.value = "**TEST VALUE!**";
            embedField.name = "__**TEST FIELD!**__";
            createMessageData.embed.fields.push_back(embedField);
            embedField.Inline = true;
            embedField.name = "ANOTHER FIELD!";
            embedField.value = "WE ARE TESTING;HE VALUES!";
            createMessageData.embed.fields.push_back(embedField);
            createMessageData.embed.setAuthor(messageData.message.data.author.username, messageData.message.data.author.getAvatarURL());
            createMessageData.embed.color[0] = 0;
            createMessageData.embed.color[1] = 255;
            createMessageData.embed.color[2] = 255;
            DiscordCoreAPI::Channel channel = pDiscordCoreAPI->guilds->getGuildAsync({ messageData.message.data.guildId }).get().channels->getChannelAsync({ messageData.message.data.channelId }).get();
            cout << channel.data.name << endl;
            DiscordCoreAPI::Message message = messageData.message.messages->createMessageAsync(createMessageData, messageData.message.data.channelId).get();
            messageData.message.messages->deleteMessageAsync({ .timeDelay = 2500 , .channelId = messageData.message.data.channelId,.messageId = messageData.message.data.id, }).get();
            message.messages->deleteMessageAsync({ 3000 , message.data.channelId,message.data.id, }).get();
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
        cout << "Error Message: " << error.what() << endl;
    }
};

task<void> onGuildCreationDelegate(DiscordCoreAPI::GuildCreationData data) {
    pDiscordCoreAPI->guilds->insertGuild(data.guild).get();
    co_return;
}

task<void> onReactionAddDelegate(DiscordCoreAPI::ReactionAddData reactionAddData) {
    
    try {
        DiscordCoreAPI::Guild guild = pDiscordCoreAPI->guilds->getGuildAsync({ "782757641540730900" }).get();// reactionAddData.reaction.guild; //pDiscordCoreAPI->guilds->fetchAsync({ reactionAddData.reaction.guild->data.id }).get;
        DiscordCoreAPI::GetChannelData channelData;
        DiscordCoreAPI::Channel channel = guild.channels->getChannelAsync({ "789162599035961364" }).get(); //*reactionAddData.reaction.channel;
        DiscordCoreAPI::GetMessageData getMessageData;
        getMessageData.channelId = reactionAddData.reaction.data.channelId;
        getMessageData.id = reactionAddData.reaction.data.messageId;
        DiscordCoreAPI::Message message = channel.messages->fetchAsync(getMessageData).get();
        if (reactionAddData.reaction.data.userId == "802191276015222825") {
            message.reactions->deleteUserReactionAsync({ .emojiName = reactionAddData.reaction.data.emoji.name, .emojiId = reactionAddData.reaction.data.emoji.id,  .userId = "802191276015222825" }).get();
        }
        co_return;
    }
    catch (exception error) {
        cout << "Error Message: " << error.what() << endl;
    }
}

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.vP10SKtqeF4cuZnFbs1B_dxYpQQ";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pdiscordCoreAPI = make_shared<DiscordCoreAPI::DiscordCoreClient>(botToken);
    pDiscordCoreAPI.reset(pdiscordCoreAPI.get());
    // Do other work here
    pdiscordCoreAPI->login().get();
    pDiscordCoreAPI->EventMachine->onMessageCreation(createdMessageEventTask);
    pDiscordCoreAPI->EventMachine->onGuildCreation(onGuildCreationDelegate);
    pDiscordCoreAPI->EventMachine->onReactionAdd(onReactionAddDelegate);
    agent::wait(pdiscordCoreAPI.get());
    return 1;
}
