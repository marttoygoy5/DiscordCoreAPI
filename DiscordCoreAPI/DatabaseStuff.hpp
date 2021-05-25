// DatabaseStuff.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATABASE_STUFF_
#define _DATABASE_STUFF_

#include "pch.h"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

    struct DiscordUserData {
        vector<string> botCommanders;
        string currencyName;
        unsigned int guildCount;
        unsigned int hoursOfDepositCooldown;
        unsigned int hoursOfDrugSaleCooldown;
        unsigned int hoursOfRobberyCooldown;
        string prefix;
        string userId;
        string userName;
    };

    struct DiscordGuildData {

    };

    struct DiscordGuildMemberData {

    };

	class DatabaseManager : public mongocxx::client {
	public:
		DatabaseManager(string botUserId)
			:client(mongocxx::uri{})
		{
            this->dataBase = (*this)[botUserId];
            this->collection = this->dataBase[botUserId];
		}
    
        void convertUserDataToDBDoc(DiscordUserData discordUserData) {
            auto docBuilder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value docValue = docBuilder
                << "currencyName" << discordUserData.currencyName
                << "guildCount" << discordUserData.guildCount
                << "hoursOfDepositCooldown" << discordUserData.hoursOfDepositCooldown
                << "houseOfDrugSaleCooldown" << discordUserData.hoursOfDrugSaleCooldown
                << "houseOfRobberyCooldown" << discordUserData.hoursOfRobberyCooldown
                << "prefix" << discordUserData.prefix
                << "userId" << discordUserData.userId
                << "userName" << discordUserData.userName
                << "_id"<< discordUserData.userId
                << bsoncxx::builder::stream::finalize;

            docBuilder << "botCommanders" << bsoncxx::builder::stream::open_array;
            for (unsigned int x = 0; x < discordUserData.botCommanders.size(); x += 1) {
                if (x == discordUserData.botCommanders.size()- 1) {
                    docBuilder << discordUserData.botCommanders.at(x) << bsoncxx::builder::stream::close_array;
                    break;
                }
                docBuilder << discordUserData.botCommanders.at(x);
            }
        }

        DiscordUserData parseUserData() {

        }

        DiscordGuildData parseGuildData() {

        }

        DiscordGuildMemberData parseGuildMemberData() {

        }

	protected:
        mongocxx::instance instance{};
        mongocxx::database dataBase;
        mongocxx::collection collection;
	};

    class DiscordUser {
    public:
        DiscordUserData data;

        DiscordUser(DiscordUserData dataNew, DatabaseManager* pDatabaseNew) {
            this->data = dataNew;
            this->pDatabase = pDatabaseNew;
        }

        task<void> writeDataToDB() {
            
        }

        task<void> getDataFromDB() {

        }

    protected:
        DatabaseManager* pDatabase{ nullptr };
    };

    class DiscordGuild {
    public:
        DiscordGuildData data;

        DiscordGuild(DiscordGuildData dataNew, DatabaseManager* pDatabaseNew) {
            this->data = dataNew;
            this->pDatabase = pDatabaseNew;
        }

        task<void> writeDataToDB() {

        }

        task<void> getDataFromDB() {

        }

    protected:
        DatabaseManager* pDatabase{ nullptr };
    };

    class DiscordGuildMember {
    public:
        DiscordGuildMemberData data;

        DiscordGuildMember(DiscordGuildMemberData dataNew, DatabaseManager* pDatabaseNew) {
            this->data = dataNew;
            this->pDatabase = pDatabaseNew;
        }

        task<void> writeDataToDB() {

        }

        task<void>getDataFromDB() {

        }
        
    protected:
        DatabaseManager* pDatabase{ nullptr };
    };
};
#endif

