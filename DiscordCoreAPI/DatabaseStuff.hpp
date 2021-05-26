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
        vector<string> botCommanders = { "", "", "" };
        string currencyName = "MBux";
        int32_t guildCount = 0;
        int32_t hoursOfDrugSaleCooldown = 3;
        int32_t hoursOfDepositCooldown = 24;
        float hoursOfRobberyCooldown = 0.100f;
        string prefix = "!";
        string userId = "";
        string userName = "";
    };
    
    struct Card {
        string suit;
        string type;
        unsigned int value;
    };

    /**
     * Class representing a standard 52-card Deck of cards.
     */
    class Deck {
        vector<Card>cards;

        Deck() {
            this->cards.resize(52);

            for (unsigned int x = 0; x < 52; x += 1) {
                this->cards[x].suit = "";
                this->cards[x].type = "";
                this->cards[x].value = 0;

                if (trunc(x / 13) == 0) {
                    this->cards[x].suit = ":hearts:";
                }
                else if (trunc(x / 13) == 1) {
                    this->cards[x].suit = ":diamonds:";
                }
                else if (trunc(x / 13) == 2) {
                    this->cards[x].suit = ":clubs:";
                }
                else if (trunc(x / 13) == 3) {
                    this->cards[x].suit = ":spades:";
                }

                if (x % 13 == 0) {
                    this->cards[x].type = "Ace";
                    this->cards[x].value = 0;
                }
                else if (x % 13 == 1) {
                    this->cards[x].type = "2";
                    this->cards[x].value = 2;
                }
                else if (x % 13 == 2) {
                    this->cards[x].type = "3";
                    this->cards[x].value = 3;
                }
                else if (x % 13 == 3) {
                    this->cards[x].type = "4";
                    this->cards[x].value = 4;
                }
                else if (x % 13 == 4) {
                    this->cards[x].type = "5";
                    this->cards[x].value = 5;
                }
                else if (x % 13 == 5) {
                    this->cards[x].type = "6";
                    this->cards[x].value = 6;
                }
                else if (x % 13 == 6) {
                    this->cards[x].type = "7";
                    this->cards[x].value = 7;
                }
                else if (x % 13 == 7) {
                    this->cards[x].type = "8";
                    this->cards[x].value = 8;
                }
                else if (x % 13 == 8) {
                    this->cards[x].type = "9";
                    this->cards[x].value = 9;
                }
                else if (x % 13 == 9) {
                    this->cards[x].type = "10";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 10) {
                    this->cards[x].type = "Jack";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 11) {
                    this->cards[x].type = "Queen";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 12) {
                    this->cards[x].type = "King";
                    this->cards[x].value = 10;
                }
            }
        }
        /**
         * Draws a random card from the Deck.
         */
        Card drawRandomcard() {
            if (this->cards.size() == 0) {
                Card voidCard{ .suit = "",.type = "",.value = 0 };
                voidCard.suit = ":black_large_square:";
                voidCard.type = "null";
                voidCard.value = 0;
                return voidCard;
            }

            unsigned int cardIndex = (unsigned int)trunc(rand() * this->cards.size());
            Card currentCard = this->cards.at(cardIndex);
            this->cards.erase(this->cards.begin() + cardIndex);
            return currentCard;
        }
    };

    struct RouletteBet {
        unsigned int betAmount = 0;
        string betOptions = "";
        string betType = "";
        unsigned int payoutAmount = 0;
        string userID = "";
        vector<string> winningNumbers = { 0 };
    };

    /**
     * Class representnig a roulette game.
     */
    struct Roulette {
        map<string, RouletteBet> rouletteBets{};
        bool currentlySpinning = false;
    };

    /**
     * Class representing the "largest paid out" user.
     */
    struct LargestPayout {
        unsigned int amount = 0;
        string date = "";
        string userID = "";
        string username = "";
    };

    /**
     * Class representing casino statistics.
     */
    struct CasinoStats {
        LargestPayout largestBlackjackPayout{};
        unsigned int totalBlackjackPayout = 0;
        LargestPayout largestCoinFlipPayout{};
        unsigned int totalCoinFlipPayout = 0;
        LargestPayout largestRoulettePayout{};
        unsigned int totalRoulettePayout = 0;
        LargestPayout largestSlotsPayout{};
        unsigned int totalSlotsPayout = 0;
        unsigned int totalPayout = 0;
    };

    /**
     * Class representing an inventory item, for the dueling/robbing system.
     */
    struct InventoryItem {
        string emoji = "";
        unsigned int itemCost = 0;
        string itemName = "";
        unsigned int oppMod = 0;
        unsigned int selfMod = 0;
    };

    /**
     * Class representing an inventory role for the dueling/robbing sytstem
     */
    struct InventoryRole {
        unsigned int roleCost = 0;;
        string roleID = "";
        string roleName = "";
    };

    /**
     * Class representing the "shop" that is hosted by the bot.
     */
    struct Shop {
        map<string, InventoryItem> items{};
        map<string, InventoryRole> roles{};
    };

    /**
     * Class representing an individual"s currency.
     */
    struct Currency {
        unsigned int bank = 10000;
        unsigned int wallet = 10000;
        unsigned int timeOfLastDeposit = 0;
    };

    struct DiscordGuildData {
        string guildId = "";
        string guildName;
        unsigned int memberCount;
        vector<Card> blackjackStack;
        unsigned int borderColor[3] = { 254, 254, 254 };
        CasinoStats casinoStats;
        vector<string> gameChannelIDs;
        Shop guildShop;
        Roulette rouletteGame;
    };

    struct DiscordGuildMemberData {
        string guildMemberId = "";
        string displayName = "";
        string guildId = "";
        string userName = "";
        Currency currency{};
        vector<InventoryItem> items;
        vector<InventoryRole> roles;
        unsigned int lastTimeRobbed;
        unsigned int lastTimeWorked;
    };

    enum class DatabaseWorkloadType {
        DISCORD_USER_WRITE = 0,
        DISCORD_USER_READ = 1,
        DISCORD_GUILD_WRITE = 2,
        DISCORD_GUILD_READ = 3,
        DISCORD_GUILD_MEMBER_WRITE = 4,
        DISCORD_GUILD_MEMBER_READ = 5
    };

    struct DatabaseWorkload {
        DiscordGuildData guildData;
        DiscordUserData userData;
        DiscordGuildMemberData guildMemberData;
        string guildId = "";
        string guildMemberId = "";
        DatabaseWorkloadType workloadType;
    };    

    class DatabaseManager : public agent {        
    protected:
        friend class DiscordCoreClient;
        friend class DiscordUser;
        friend class DiscordGuild;
        friend class DiscordGuildMember;
        static string botUserId;
        static Scheduler* pScheduler;
        static mongocxx::instance* instance;
        mongocxx::database dataBase;
        mongocxx::collection collection;
        mongocxx::client client;
        unbounded_buffer<DatabaseWorkload> requestBuffer;
        unbounded_buffer<DiscordUserData>discordUserOutputBuffer;
        unbounded_buffer<DiscordGuildData>discordGuildOutputBuffer;
        unbounded_buffer<DiscordGuildMemberData>discordGuildMemberOutputBuffer;
        unbounded_buffer<exception>errorBuffer;

        DatabaseManager()
            : agent(*DatabaseManager::pScheduler)
        {
            this->botUserId = DatabaseManager::botUserId;
            cout << "BOT USER ID: " << this->botUserId << endl;
            this->client = mongocxx::client{ mongocxx::uri{} };
            this->dataBase = this->client[this->botUserId];
            this->collection = this->dataBase[this->botUserId];
        }

        static void initialize(string botUserIdNew, Scheduler* pSchedulerNew) {
            DatabaseManager::botUserId = botUserIdNew;
            DatabaseManager::pScheduler = pSchedulerNew;
            DatabaseManager::instance = new mongocxx::instance();
        }

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
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
                buildDoc.append(kvp("userId", discordUserData.userId));
                buildDoc.append(kvp("userName", discordUserData.userName));
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
            userData.hoursOfDrugSaleCooldown = docValue.view()["hoursOfDrugSaleCooldown"].get_int32();
            userData.hoursOfRobberyCooldown = (float)docValue.view()["hoursOfRobberyCooldown"].get_double();
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
            try {
                DatabaseWorkload workload;
                if (try_receive(this->requestBuffer, workload)) {
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManager::convertUserDataToDBDoc(workload.userData);
                        cout << "WriteDataToDB: " << bsoncxx::to_json(doc.view()) << endl;
                        boost::optional<bsoncxx::document::value> result;
                        try {
                            result = this->collection.find_one_and_update(bsoncxx::builder::stream::document{} << "userId" << workload.userData.userId << finalize, doc.view());
                            if (!result) {
                                this->collection.insert_one(doc.view());
                            }
                        }
                        catch (const mongocxx::write_exception& e) {
                            cout << "DatabaseManager::run() Error 01: " << e.what() << endl << endl;
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_READ) {
                        try {
                            auto result = this->collection.find_one(bsoncxx::builder::stream::document{} << "userId" << workload.userData.userId << finalize);
                            if (result) {
                                DiscordUserData userData = DatabaseManager::parseUserData(result.get());
                                send(this->discordUserOutputBuffer, userData);
                            }
                            else {
                                DiscordUserData userData;
                                send(this->discordUserOutputBuffer, userData);
                            }
                            cout << "GetDataFromDB: " << bsoncxx::to_json(result.get().view()) << endl;
                        }
                        catch (bsoncxx::v_noabi::exception& e) {
                            cout << "DatabaseManager::run() Error 02: " << e.what() << endl << endl;
                        }
                    }
                }
            }
            catch (const exception& e) {
                send(errorBuffer, e);
            }
            done();
        }

    };

    class DiscordUser {
    public:
        DiscordUserData data;
        DiscordUser() = delete;
        DiscordUser(string userNameNew, string userIdNew) {
            this->data.userId = userIdNew;
            this->data.userName = userNameNew;
            this->getDataFromDB().get();
        }

        task<void> writeDataToDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_USER_WRITE;
                workload.userData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordUser::writeDataToDB() Error: " << error.what() << endl << endl;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordUser::writeDataToDB() Error: " << e.what() << endl;
            }
        }

        task<void> getDataFromDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_USER_READ;
                workload.userData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordUser::getDataFromDB() Error 01: " << error.what() << endl << endl;
                }
                DiscordUserData userData;
                try_receive(databaseManager.discordUserOutputBuffer, userData);
                if (userData.userId != "") {
                    this->data = userData;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordUser::getDataFromDB() Error 02: " << e.what() << endl;
            }
            
        }

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
    mongocxx::instance* DatabaseManager::instance;
};
#endif

