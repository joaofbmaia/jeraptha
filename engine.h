#pragma once

#include <string>
#include <list>

#include "bettor.h"
#include "wager.h"
#include "economy.h"

class engine {
    public:
        engine(std::string filename);
        void registerWager(std::string description, std::string creatorID, std::time_t date);
        int addBet(int wagerID, bool outcome, int value);
        void settle(int wagerID, bool outcome);
        void cancel(int wagerID);
        wager *getWager(int wagerID);
    private:
        void readFile();
        void writeFile();
        std::string _filename;
        economy eco;
        std::list <bettor> bettorList;
        std::list <wager> wagerList;
};