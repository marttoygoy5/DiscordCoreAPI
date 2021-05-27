// DatabaseStuff.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATABASE_STUFF_
#define _DATABASE_STUFF_

#include "pch.h"
#include "GuildStuff.hpp"
#include "GuildMemberStuff.hpp"

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
        string suit = "";
        string type = "";
        unsigned int value = 0;
    };

    class Deck {
        vector<Card> cards;

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

        // Draws a random card from the Deck.
        Card drawRandomcard() {
            if (this->cards.size() == 0) {
                Card voidCard{ .suit = "",.type = "",.value = 0 };
                voidCard.suit = ":black_large_square:";
                voidCard.type = "null";
                voidCard.value = 0;
                return voidCard;
            }

            unsigned int cardIndex = (unsigned int)trunc((rand() / RAND_MAX) * this->cards.size());
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
        string userId = "";
        vector<string> winningNumbers = { 0 };
    };

    struct Roulette {
        map<string, RouletteBet> rouletteBets{};
        bool currentlySpinning = false;
    };

    struct LargestPayout {
        unsigned int amount = 0;
        string date = "";
        string userId = "";
        string username = "";
    };

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

    struct InventoryItem {
        string emoji = "";
        unsigned int itemCost = 0;
        string itemName = "";
        unsigned int oppMod = 0;
        unsigned int selfMod = 0;
    };

    struct InventoryRole {
        unsigned int roleCost = 0;
        string roleId = "";
        string roleName = "";
    };

    struct Shop {
        vector<InventoryItem> items{};
        vector<InventoryRole> roles{};
    };

    struct Currency {
        unsigned int bank = 10000;
        unsigned int wallet = 10000;
        unsigned int timeOfLastDeposit = 0;
    };

    struct DiscordGuildData {
        string guildId = "";
        string guildName = "";
        unsigned int memberCount = 0;
        vector<Card> blackjackStack{};
        unsigned int borderColor[3] = { 254, 254, 254 };
        CasinoStats casinoStats{};
        vector<string> gameChannelIds{};
        Shop guildShop{};
        Roulette rouletteGame{};
    };

    struct DiscordGuildMemberData {
        string guildMemberId = "";
        string displayName = "";
        string guildId = "";
        string globalId = "";
        string userName = "";
        Currency currency{};
        map<string, InventoryItem> items{};
        map<string, InventoryRole> roles{};
        unsigned int lastTimeRobbed = 0;
        unsigned int lastTimeWorked = 0;
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
        string globalId = "";
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
        static mongocxx::collection collection;
        static mongocxx::database dataBase;
        static mongocxx::client client;
        unbounded_buffer<DatabaseWorkload> requestBuffer;
        unbounded_buffer<DiscordUserData>discordUserOutputBuffer;
        unbounded_buffer<DiscordGuildData>discordGuildOutputBuffer;
        unbounded_buffer<DiscordGuildMemberData>discordGuildMemberOutputBuffer;
        unbounded_buffer<exception>errorBuffer;

        DatabaseManager()
            : agent(*DatabaseManager::pScheduler)
        {
            this->botUserId = DatabaseManager::botUserId;
        }

        static void initialize(string botUserIdNew, Scheduler* pSchedulerNew) {
            DatabaseManager::botUserId = botUserIdNew;
            DatabaseManager::pScheduler = pSchedulerNew;
            DatabaseManager::instance = new mongocxx::instance();
            DatabaseManager::client = mongocxx::client{ mongocxx::uri{} };
            DatabaseManager::dataBase = DatabaseManager::client[DatabaseManager::botUserId];
            DatabaseManager::collection = DatabaseManager::dataBase[DatabaseManager::botUserId];
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
            try {
                DiscordUserData userData;
                userData.currencyName = docValue.view()["currencyName"].get_utf8().value.to_string();
                userData.guildCount = docValue.view()["guildCount"].get_int32();
                userData.hoursOfDepositCooldown = docValue.view()["hoursOfDepositCooldown"].get_int32();
                userData.hoursOfDrugSaleCooldown = docValue.view()["hoursOfDrugSaleCooldown"].get_int32();
                userData.hoursOfRobberyCooldown = (float)docValue.view()["hoursOfRobberyCooldown"].get_double();
                userData.prefix = docValue.view()["prefix"].get_utf8().value.to_string();
                userData.userId = docValue.view()["userId"].get_utf8().value.to_string();
                userData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                auto botCommandersArray = docValue.view()["botCommanders"].get_array();
                vector<string> newVector;
                for (const auto& value : botCommandersArray.value) {
                    newVector.push_back(value.get_utf8().value.to_string());
                }
                userData.botCommanders = newVector;
                return userData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::parseUserData() Error: " << e.what() << endl;
            }
        }

        static bsoncxx::builder::basic::document convertGuildDataToDBDoc(DiscordGuildData discordGuildData) {
            try {
                bsoncxx::builder::basic::document buildDoc;
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("guildId", discordGuildData.guildId));
                buildDoc.append(kvp("_id", discordGuildData.guildId));
                buildDoc.append(kvp("guildName", discordGuildData.guildName));
                buildDoc.append(kvp("blackjackStack",
                    [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                        for (auto& value : discordGuildData.blackjackStack) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
                                subDocument.append(kvp("suit", value.suit),
                                    kvp("type", value.type),
                                    kvp("value", bsoncxx::types::b_int32(value.value)));
                                });
                        }
                    }));
                buildDoc.append(kvp("memberCount", bsoncxx::types::b_int32(discordGuildData.memberCount)));
                buildDoc.append(kvp("borderColor", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordGuildData.borderColor) {
                        subArray.append(bsoncxx::types::b_int32(value));
                    }
                    }));
                buildDoc.append(kvp("gameChannelIds", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordGuildData.gameChannelIds) {
                        subArray.append(value);
                    }
                    }));
                buildDoc.append(kvp("guildShop", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("items", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                        for (const auto& value : discordGuildData.guildShop.items) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
                                subDocument2.append(kvp("itemCost", bsoncxx::types::b_int32(value.itemCost)));
                                subDocument2.append(kvp("itemName", value.itemName));
                                subDocument2.append(kvp("emoji", value.emoji));
                                subDocument2.append(kvp("selfMod", bsoncxx::types::b_int32(value.selfMod)));
                                subDocument2.append(kvp("oppMod", bsoncxx::types::b_int32(value.oppMod)));
                                });
                        }
                        })),
                        subDocument.append(kvp("roles", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                            for (const auto& value : discordGuildData.guildShop.roles) {
                                subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
                                    subDocument2.append(kvp("roleId", value.roleId));
                                    subDocument2.append(kvp("roleName", value.roleName));
                                    subDocument2.append(kvp("roleCost", bsoncxx::types::b_int32(value.roleCost)));
                                    });
                            }
                            }));
                    }));
                return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::convertGuildDataToDBDoc() Error: " << e.what() << endl;
            }
        };

        DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {
            try {
                DiscordGuildData guildData;
                guildData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
                guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
                for (auto& value : docValue.view()["blackjackStack"].get_array().value) {
                    Card blackjackCard;
                    if (value.get_document().view().find("suit") != value.get_document().view().end()){
                        blackjackCard.suit = value.get_document().view()["suit"].get_utf8().value.to_string();
                    }
                    if (value.get_document().view().find("type") != value.get_document().view().end()){
                        blackjackCard.type = value.get_document().view()["type"].get_utf8().value.to_string();
                    }
                    if (value.get_document().view().find("value") != value.get_document().view().end()) {
                        blackjackCard.value = value.get_document().view()["value"].get_int32().value;
                    }
                    guildData.blackjackStack.push_back(blackjackCard);
                }
                for (unsigned int x = 0; x < 3; x += 1) {
                    guildData.borderColor[x] = docValue.view()["borderColor"].get_array().value[x].get_int32().value;
                }
                
                for (auto& value : docValue.view()["gameChannelIds"].get_array().value) {
                    guildData.gameChannelIds.push_back(value.get_utf8().value.to_string());
                }
                if  (docValue.view()["guildShop"].get_document().value["items"].get_array().value.find(0) != docValue.view()["guildShop"].get_document().value["items"].get_array().value.end()){
                    for (auto& value : docValue.view()["guildShop"].get_document().value["items"].get_array().value) {
                        InventoryItem item;
                        item.emoji = value["emoji"].get_utf8().value.to_string();
                        item.itemCost = value["itemCost"].get_int32().value;
                        item.itemName = value["itemName"].get_utf8().value.to_string();
                        item.oppMod = value["oppMod"].get_int32().value;
                        item.selfMod = value["selfMod"].get_int32().value;
                        guildData.guildShop.items.push_back(item);
                    }
                }
                if (docValue.view()["guildShop"].get_document().value["roles"].get_array().value.find(0) != docValue.view()["guildShop"].get_document().value["items"].get_array().value.end()) {
                    for (auto& value : docValue.view()["guildShop"].get_document().value["roles"].get_array().value) {
                        InventoryRole role;
                        role.roleCost = value["roleCost"].get_int32().value;
                        role.roleId = value["roleId"].get_utf8().value.to_string();
                        role.roleName = value["roleName"].get_utf8().value.to_string();
                        guildData.guildShop.roles.push_back(role);
                    }
                }
                
                
                return guildData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::parseGuildData() Error: " << e.what() << endl;
            }
        };

        static bsoncxx::builder::basic::document convertGuildMemberDataToDBDoc(DiscordGuildMemberData discordGuildMemberData) {
            try {
                bsoncxx::builder::basic::document buildDoc;
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("guildId", discordGuildMemberData.guildId));
                buildDoc.append(kvp("_id", discordGuildMemberData.globalId));
                buildDoc.append(kvp("globalId", discordGuildMemberData.globalId));
                buildDoc.append(kvp("displayName", discordGuildMemberData.displayName));
                buildDoc.append(kvp("guildMemberId", discordGuildMemberData.guildMemberId));
                buildDoc.append(kvp("userName", discordGuildMemberData.userName));
                buildDoc.append(kvp("lastTimeRobbed", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeRobbed)));
                buildDoc.append(kvp("lastTimeWorked", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeWorked)));
                buildDoc.append(kvp("currency", [discordGuildMemberData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("bank", bsoncxx::types::b_int32(discordGuildMemberData.currency.bank)),
                        kvp("wallet", bsoncxx::types::b_int32(discordGuildMemberData.currency.wallet)),
                        kvp("timeOfLastDeposit", bsoncxx::types::b_int32(discordGuildMemberData.currency.timeOfLastDeposit)));
                    }));
                buildDoc.append(kvp("items", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& [key, value] : discordGuildMemberData.items) {
                        subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
                            subDocument.append(kvp("itemCost", bsoncxx::types::b_int32(value.itemCost)),
                                kvp("itemName", value.itemName),
                                kvp("selfMod", bsoncxx::types::b_int32(value.selfMod)),
                                kvp("oppMod", bsoncxx::types::b_int32(value.oppMod)),
                                kvp("emoji", value.emoji));

                            });
                    }

                    })),
                    buildDoc.append(kvp("roles", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
                        for (auto& [key, value] : discordGuildMemberData.roles) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
                                subDocument.append(kvp("roleCost", bsoncxx::types::b_int32(value.roleCost)),
                                    kvp("roleName", value.roleName),
                                    kvp("roleId", value.roleId));

                                });
                        }

                        }));

                return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::convertGuildDataToDBDoc() Error: " << e.what() << endl;
            }
        };

        DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {
            try {
                DiscordGuildMemberData guildMemberData;
                guildMemberData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildMemberData.displayName = docValue.view()["displayName"].get_utf8().value.to_string();
                guildMemberData.globalId = docValue.view()["globalId"].get_utf8().value.to_string();
                guildMemberData.guildMemberId = docValue.view()["guildMemberId"].get_utf8().value.to_string();
                guildMemberData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                guildMemberData.lastTimeWorked = docValue.view()["lastTimeWorked"].get_int32().value;
                guildMemberData.lastTimeRobbed = docValue.view()["lastTimeRobbed"].get_int32().value;
                guildMemberData.currency.bank = docValue.view()["currency"].get_document().value["bank"].get_int32().value;
                guildMemberData.currency.wallet = docValue.view()["currency"].get_document().value["wallet"].get_int32().value;
                guildMemberData.currency.timeOfLastDeposit = docValue.view()["currency"].get_document().value["timeOfLastDeposit"].get_int32().value;
                if(docValue.view()["items"].get_array().value.find(0)!=docValue.view()["items"].get_array().value.end()){
                    for (auto& value : docValue.view()["items"].get_array().value) {
                        InventoryItem item;
                        item.emoji = value["emoji"].get_utf8().value.to_string();
                        item.itemName = value["itemName"].get_utf8().value.to_string();
                        item.itemCost = value["itemCost"].get_int32().value;
                        item.oppMod = value["oppMod"].get_int32().value;
                        item.selfMod = value["selfMod"].get_int32().value;
                        guildMemberData.items.insert(make_pair(item.itemName, item));
                    }
                }
                if (docValue.view()["roles"].get_array().value.find(0) != docValue.view()["roles"].get_array().value.end()) {
                    for (auto& value : docValue.view()["roles"].get_array().value) {
                        InventoryRole role;
                        role.roleCost = value["roleCost"].get_int32().value;
                        role.roleId = value["roleId"].get_utf8().value.to_string();
                        role.roleName = value["roleName"].get_utf8().value.to_string();
                        guildMemberData.roles.insert(make_pair(role.roleName, role));
                    }
                }
                return guildMemberData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManager::parseGuildData() Error: " << e.what() << endl;
            }
        };

        void run() {
            try {
                DatabaseWorkload workload;
                if (try_receive(this->requestBuffer, workload)) {
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManager::convertUserDataToDBDoc(workload.userData);
                        auto result = DatabaseManager::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "userId" << workload.userData.userId << finalize, doc.view());
                        cout << "USER WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                        if (result.get_ptr() == NULL) {
                            cout << "USER WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                            DatabaseManager::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_READ) {
                        auto result = DatabaseManager::collection.find_one(bsoncxx::builder::stream::document{} << "userId" << workload.userData.userId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordUserData userData = DatabaseManager::parseUserData(result.get());
                            send(this->discordUserOutputBuffer, userData);
                            cout << "USER READ: " << bsoncxx::to_json(result.get().view()) << endl;
                        }
                        else {
                            DiscordUserData userData;
                            send(this->discordUserOutputBuffer, userData);
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManager::convertGuildDataToDBDoc(workload.guildData);
                        auto result = DatabaseManager::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "guildId" << workload.guildData.guildId << finalize, doc.view());
                        cout << "GUILD WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                        if (result.get_ptr() == NULL) {
                            cout << "GUILD WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                            DatabaseManager::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_READ) {
                        auto result = DatabaseManager::collection.find_one(bsoncxx::builder::stream::document{} << "guildId" << workload.guildData.guildId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildData guildData = DatabaseManager::parseGuildData(result.get());
                            send(this->discordGuildOutputBuffer, guildData);
                            cout << "GUILD READ: " << bsoncxx::to_json(result.get().view()) << endl;
                        }
                        else {
                            DiscordGuildData guildData;
                            cout << "GUILD READ DEFAULT: " << guildData.guildId << endl;
                            send(this->discordGuildOutputBuffer, guildData);
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManager::convertGuildMemberDataToDBDoc(workload.guildMemberData);
                        auto result = DatabaseManager::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "globalId" << workload.guildMemberData.globalId << finalize, doc.view());
                        cout << "GUILDMEMBER WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                        if (result.get_ptr() == NULL) {
                            cout << "GUILDMEMBER WRITE: " << bsoncxx::to_json(doc.view()) << endl;
                            DatabaseManager::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ) {
                        auto result = DatabaseManager::collection.find_one(bsoncxx::builder::stream::document{} << "globalId" << workload.guildMemberData.globalId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildMemberData guildMemberData = DatabaseManager::parseGuildMemberData(result.get());
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
                            cout << "GUILDMEMBER READ: " << bsoncxx::to_json(result.get().view()) << endl;
                        }
                        else {
                            DiscordGuildMemberData guildMemberData;
                            cout << "GUILDMEMBER DEFAULT READ: " << workload.guildMemberData.globalId << endl;
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
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
            this->getDataFromDB().get();
            this->data.guildCount = 0;
            this->data.userName = userNameNew;
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
                cout << "DiscordUser::writeDataToDB() Error: " << e.what() << endl << endl;
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
                cout << "DiscordUser::getDataFromDB() Error 02: " << e.what() << endl << endl;
            }
            
        }

    };

    class DiscordGuild {
    public:
        DiscordGuildData data;
        DiscordGuild() = delete;
        DiscordGuild(DiscordCoreInternal::GuildData guildData) {
            this->data.guildId = guildData.id;
            this->getDataFromDB().get();
            this->data.guildName = guildData.name;
            this->data.memberCount = guildData.memberCount;
        }

        task<void> writeDataToDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_WRITE;
                workload.guildData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordGuild::writeDataToDB() Error: " << error.what() << endl << endl;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordGUild::writeDataToDB() Error: " << e.what() << endl << endl;
            }
        }

        task<void> getDataFromDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_READ;
                workload.guildData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordGuild::getDataFromDB() Error 01: " << error.what() << endl << endl;
                }
                DiscordGuildData guildData;
                try_receive(databaseManager.discordGuildOutputBuffer, guildData);
                if (guildData.guildId != "") {
                    this->data = guildData;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordGuild::getDataFromDB() Error 02: " << e.what() << endl << endl;
            }

        }
    };

    class DiscordGuildMember {
    public:
        DiscordGuildMemberData data;
        DiscordGuildMember() = delete;
        DiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
            this->data.guildMemberId = guildMemberData.user.id;
            this->data.guildId = guildMemberData.guildId;
            this->data.globalId = this->data.guildId + " + " + this->data.guildMemberId;
            this->getDataFromDB().get();
            this->data.guildMemberId = guildMemberData.user.id;
            this->data.guildId = guildMemberData.guildId;
            this->data.globalId = this->data.guildId + " + " + this->data.guildMemberId;
            this->data.displayName = guildMemberData.user.username;
            this->data.userName = guildMemberData.user.username;
        }

        task<void> writeDataToDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE;
                workload.guildMemberData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordGuildMember::writeDataToDB() Error 01: " << error.what() << endl << endl;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordGuildMember::writeDataToDB() Error 02: " << e.what() << endl << endl;
            }
        }

        task<void> getDataFromDB() {
            try {
                DatabaseManager databaseManager;
                DatabaseWorkload workload;
                workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ;
                workload.guildMemberData = this->data;
                send(databaseManager.requestBuffer, workload);
                databaseManager.start();
                agent::wait(&databaseManager);
                exception error;
                while (databaseManager.getError(error)) {
                    cout << "DiscordGuildMember::getDataFromDB() Error 01: " << error.what() << endl << endl;
                }
                DiscordGuildMemberData guildMemberData;
                try_receive(databaseManager.discordGuildMemberOutputBuffer, guildMemberData);
                if (guildMemberData.guildId != "") {
                    this->data = guildMemberData;
                }
                co_return;
            }
            catch (mongocxx::v_noabi::logic_error& e) {
                cout << "DiscordGuildMember::getDataFromDB() Error 02: " << e.what() << endl << endl;
            }

        }
    };
    Scheduler* DatabaseManager::pScheduler;
    string DatabaseManager::botUserId;
    mongocxx::instance* DatabaseManager::instance;
    mongocxx::collection DatabaseManager::collection;
    mongocxx::database DatabaseManager::dataBase;
    mongocxx::client DatabaseManager::client;
};
#endif

