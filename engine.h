#pragma once

#include <string>
#include <list>

#include "bettor.h"
#include "wager.h"
#include "economy.h"

class engine {
    public:
        engine(std::string filename);
        int balance(std::string ID);
        bool gift(std::string sourceID, std::string destinationID);
        int drawCoins(std::string ID);
        int registerWager(std::string description, std::string creatorID, std::time_t date);
        std::list <int> listActiveWagers();
        int addBet(std::string bettorID, int wagerID, bool outcome, int value);
        void settle(int wagerID, bool outcome);
        void cancel(int wagerID);
        std::list <std::string> checkNewBettors(std::list <std::string> *membersList);
        wager *getWager(int wagerID);
    private:
        void readFile();
        void writeFile();
        std::string _filename;
        economy eco;
        std::list <bettor> bettorList;
        std::list <wager> wagerList;
};