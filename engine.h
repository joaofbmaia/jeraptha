#pragma once

#include <string>
#include <list>

#include "bettor.h"
#include "wager.h"
#include "economy.h"

struct settleResponse {
    std::string bettorID;
    int prize;
};

class engine {
    public:
        engine(std::string filename);
        int balance(std::string ID);
        bool gift(std::string sourceID, std::string destinationID);
        time_t timeElapsedDraw(std::string ID);
        int drawCoins(std::string ID);
        int registerWager(std::string description, std::string creatorID, std::time_t date, int duration);
        std::list <int> listActiveWagers();
        int addBet(std::string bettorID, int wagerID, bool outcome, int value);
        std::list <settleResponse> settle(int wagerID, bool outcome);
        void cancel(int wagerID);
        void close(int wagerID);
        std::list <std::string> checkNewBettors(std::list <std::string> *membersList);
        wager *getWager(int wagerID);
        void updateClosedBets();
        std::list <std::string> getSortedBettors();
    private:
        bool compareBalance(const std::string &ID1, const std::string &ID2);
        void readFile();
        void writeFile();
        std::string _filename;
        economy eco;
        std::list <bettor> bettorList;
        std::list <wager> wagerList;
};