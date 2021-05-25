// DatabaseStuff.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATABASE_STUFF_
#define _DATABASE_STUFF_

#include "pch.h"

namespace DiscordCoreAPI {
    
    class DatabaseManager;

    struct DiscordUserData {
        vector<string> botCommanders;
        string currencyName = "MBux";
        int32_t guildCount;
        int32_t hoursOfDrugSaleCooldown = 3;
        int32_t hoursOfDepositCooldown = 24;
        float hoursOfRobberyCooldown = 0.1f;
        string prefix = "!";
        string userId;
        string userName;
    };
    
    struct DiscordGuildData {

    };

    struct DiscordGuildMemberData {

    };
    
    enum class DatabaseWorkloadType {
        DISCORD_USER = 0,
        DISCORD_GUILD = 1,
        DISCORD_GUILD_MEMBER = 2
    };

    struct DatabaseWorkload {
        string guildId = "";
        string guildMemberId = "";
        DatabaseWorkloadType workloadType;
    };
    
    class DatabaseManager : public mongocxx::client, public agent {        
    protected:
        friend class DiscordCoreClient;
        friend class DiscordUser;
        friend class DiscordGuild;
        friend class DiscordGuildMember;
        static string botUserId;
        static Scheduler* pScheduler;
        mongocxx::instance instance{};
        mongocxx::database dataBase;
        mongocxx::collection collection;
        unbounded_buffer<DatabaseWorkload> requestBuffer;
        unbounded_buffer<DiscordUser>discordUserSubmissionBuffer;
        unbounded_buffer<DiscordGuild>discordGuildSubmissionBuffer;
        unbounded_buffer<DiscordGuildMember>discordGuildMemberSubmissionBuffer;
        unbounded_buffer<DiscordUser>discordUserOutputBuffer;
        unbounded_buffer<DiscordGuild>discordGuildOutputBuffer;
        unbounded_buffer<DiscordGuildMember>discordGuildMemberOutputBuffer;


        DatabaseManager()
            :client(mongocxx::uri{}),
            agent(*DatabaseManager::pScheduler)
        {
            this->botUserId = DatabaseManager::botUserId;
            this->dataBase = (*this)[this->botUserId];
            this->collection = this->dataBase[this->botUserId];
        }

        static void initialize(string botUserIdNew, Scheduler* pSchedulerNew) {
            DatabaseManager::botUserId = botUserIdNew;
            DatabaseManager::pScheduler = pSchedulerNew;
        }

        static bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
            try {
                bsoncxx::builder::basic::document buildDoc;
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordUserData.userId));
                buildDoc.append(kvp("currencyName", discordUserData.currencyName));
                buildDoc.append(kvp("guildCount", bsoncxx::types::b_int32(discordUserData.guildCount)));
                buildDoc.append(kvp("hoursOfDepositCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDepositCooldown)));
                buildDoc.append(kvp("hoursOfDrugSaleCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDrugSaleCooldown)));
                buildDoc.append(kvp("hoursOfRobberyCooldown", bsoncxx::types::b_double(discordUserData.hoursOfRobberyCooldown)));
                buildDoc.append(kvp("prefix", discordUserData.prefix));
                bsoncxx::builder::basic::array newArray;
                for (unsigned int x = 0; x < discordUserData.botCommanders.size(); x += 1) {
                    newArray.append(discordUserData.botCommanders.at(x));
                }
                buildDoc.append(kvp("botCommanders", newArray));
                return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::convertUserDataToDBDoc() Error: " << e.what() << endl;
            }

        }

        static DiscordUserData parseUserData(bsoncxx::document::value docValue) {
            DiscordUserData userData;
            userData.currencyName = (string)docValue.view()["currencyName"].get_utf8().value;
            userData.guildCount = docValue.view()["guildCount"].get_int32();
            userData.hoursOfDepositCooldown = docValue.view()["hoursOfDepositCooldown"].get_int32();
            userData.hoursOfDrugSaleCooldown = docValue.view()["hoursOfDrugSaleCooldown "].get_int32();
            userData.hoursOfRobberyCooldown = docValue.view()["hoursOfRobberyCooldown"].get_double();
            userData.prefix = (string)docValue.view()["prefix"].get_utf8().value;
            userData.userId = (string)docValue.view()["userId"].get_utf8().value;
            userData.userName = (string)docValue.view()["userName"].get_utf8().value;
            auto botCommandersArray = docValue.view()["botCommanders"].get_array();
            for (const auto& value : botCommandersArray.value) {
                userData.botCommanders.push_back((string)value.get_utf8().value);
            }
            return userData;
        }

        DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {

        }

        DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {

        }


        void run() {
            DatabaseWorkload workload;
            if (try_receive(this->requestBuffer, workload)) {

            }
        }

    };

    class DiscordUser {
    public:
        DiscordUserData data;

        DiscordUser(string userNameNew, string userIdNew, unsigned int guildCountNew) {
            this->data.userId = userIdNew;
            this->data.userName = userNameNew;
            this->data.guildCount = guildCountNew;
            this->writeDataToDB().get();
            this->getDataFromDB().get();
        }

        task<void> writeDataToDB() {
            DatabaseManager databaseManager();
            bsoncxx::builder::basic::document doc = DatabaseManager::convertUserDataToDBDoc(this->data);
            cout << "WriteDataToDB: " << bsoncxx::to_json(doc.view()) << endl;
            boost::optional<bsoncxx::document::value> result;
            try {
                result = this->pCollection->find_one_and_update(bsoncxx::builder::stream::document{} << "userId" << this->data.userId << finalize, doc.view());
            }
            catch (const mongocxx::write_exception& e) {
                cout << "DiscordUser::writeDataToDB() Error: " << e.what() << endl << endl;
                this->pCollection->insert_one(doc.view());
            }
            co_return;
        }

        task<void> getDataFromDB() {
            try {
                auto result = this->pCollection->find_one(bsoncxx::builder::stream::document{} << "_id" << this->data.userId << finalize).get();
                cout << "GetDataFromDB: " << bsoncxx::to_json(result.view()) << endl;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DiscordUser::getDataFromDB() Error: " << e.what() << endl << endl;
            }
            co_return;
        }

    protected:
        mongocxx::collection* pCollection{ nullptr };
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
    Scheduler* DatabaseManager::pScheduler;
    string DatabaseManager::botUserId;
};
#endif

