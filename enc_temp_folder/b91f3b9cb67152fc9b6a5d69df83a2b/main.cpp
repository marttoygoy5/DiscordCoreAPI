// Main.cpp - Main source file, for "the framework".
// May 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "pch.h"
#include "DiscordCoreClient.hpp"
#include "Commands/Commands.hpp"
#include "Commands/Help.hpp"

shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreAPI;

task<void> createdMessageEventTask(DiscordCoreAPI::MessageCreationData messageData) {
    try {
        if (messageData.message.data.content.c_str()[0] == '!') {
            DiscordCoreAPI::CommandData commandData(messageData.message);
            commandData.message = messageData.message;
            commandData.pClient = pDiscordCoreAPI;
            DiscordCoreAPI::CommandController::checkForAndRunCommand(commandData);

            DiscordCoreAPI::CreateMessageData createMessageData;
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
            createMessageData.embed.setDescription("TESTING THE DESCRIPTION");
            createMessageData.embed.setAuthor(messageData.message.data.author.username, messageData.message.data.author.getAvatarURL());
            createMessageData.embed.color[0] = 0;
            createMessageData.embed.color[1] = 255;
            createMessageData.embed.color[2] = 255;
            DiscordCoreAPI::Channel DmChannel = pDiscordCoreAPI->channels->getDMChannelAsync({ messageData.message.data.author.id }).get();
            DiscordCoreAPI::Channel channel = pDiscordCoreAPI->channels->getChannelAsync({ messageData.message.data.channelId }).get();
            DiscordCoreAPI::Message messageDM = messageData.message.messages->sendDMAsync({ .channelId = DmChannel.data.id, .userId = messageData.message.data.author.id, .messageData = createMessageData }).get();
            if (channel.data.type == DiscordCoreInternal::ChannelType::DM) {
                co_return;
            }
            DiscordCoreAPI::Message message = messageData.message.messages->createMessageAsync(createMessageData, messageData.message.data.channelId).get();
            messageData.message.messages->deleteMessageAsync({ messageData.message.data.channelId, messageData.message.data.id, 4500 }).get();
            DiscordCoreAPI::EditMessageData editData;
            editData.embed = message.data.embeds.at(0);
            editData.content = message.data.content;
            editData.embed.fields.at(0).name = "__**TESTING NAME**__";
            editData.embed.fields.at(1).value = "WE'RE TESTING THE VALUES";
            editData.embed.color[0] = 245;
            editData.embed.color[1] = 0;
            editData.embed.color[2] = 255;
            editData.embed.setImage("https://raw.githubusercontent.com/RealTimeChris/Unnamed-Renderer-DX12/main/Images/124%2C%203000SPP%2C%2030B%2C%20720p.png");
            editData.embed.setThumbnail("https://raw.githubusercontent.com/RealTimeChris/Unnamed-Renderer-DX12/main/Images/122%2C%203000SPP%2C%2030B%2C%20720p.png");
            message.messages->editMessageAsync(editData, message.data.channelId, message.data.id).get();
            
            message.messages->deleteMessageAsync({ message.data.channelId,message.data.id, 20000 }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "ZeedenAndDoom" ,"805151947131715654" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "🔫" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "🧪" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "❤" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "🌎" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "🚀" }).get();
            message.reactions->createReactionAsync({ message.data.channelId,message.data.id , "❗" }).get();


            co_return;
        }
    }
    catch (exception error) {
        cout << "Error Message: " << error.what() << endl;
    }
}

task<void> onGuildCreationDelegate(DiscordCoreAPI::GuildCreationData data) {
    pDiscordCoreAPI->guilds->insertGuild(data.guild).get();
    co_return;
}

task<void> onReactionAddDelegate(DiscordCoreAPI::ReactionAddData reactionAddData) {
    
    try {
        DiscordCoreAPI::Guild guild = pDiscordCoreAPI->guilds->getGuildAsync({ "782757641540730900" }).get();
        DiscordCoreAPI::GetChannelData channelData;
        DiscordCoreAPI::Channel channel = guild.channels->getChannelAsync({ "789162599035961364" }).get();
        DiscordCoreAPI::GetMessageData getMessageData;
        getMessageData.channelId = reactionAddData.reaction.data.channelId;
        getMessageData.id = reactionAddData.reaction.data.messageId;
        DiscordCoreAPI::Message message = pDiscordCoreAPI->messages->getMessageAsync(getMessageData).get();
        if (reactionAddData.reaction.data.userId == "802191276015222825") {
            message.reactions->deleteUserReactionAsync({ .channelId = getMessageData.channelId, .messageId = getMessageData.id, .userId = "802191276015222825", .emojiName = reactionAddData.reaction.data.emoji.name, .emojiId = reactionAddData.reaction.data.emoji.id }).get();
        }
        co_return;
    }
    catch (exception error) {
        cout << "Error Message: " << error.what() << endl;
    }
}

int main() {
    winrt::init_apartment();
    hstring botToken = L"ODI2ODI3MTM1NzA4NTYxNDc4.YGSIxg.zZkL6wfnsOF4BcA37rlIqDqw1gI";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pdiscordCoreAPI = make_shared<DiscordCoreAPI::DiscordCoreClient>(botToken);
    pDiscordCoreAPI.reset(pdiscordCoreAPI.get());
    // Do other work here
    pdiscordCoreAPI->login().get();
    DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::helpCommand, "help");
    pDiscordCoreAPI->EventMachine->onMessageCreation(createdMessageEventTask);
    pDiscordCoreAPI->EventMachine->onGuildCreation(onGuildCreationDelegate);
    pDiscordCoreAPI->EventMachine->onReactionAdd(onReactionAddDelegate);
    agent::wait(pdiscordCoreAPI.get());
    return 1;
}
