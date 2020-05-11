#include "engine.h"

#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <ctime>
#include <cmath>

#define BET_TAX_RATE 0.05
#define WAGER_PRICE 5

engine::engine(std::string filename) {
    _filename = filename;
    readFile();
}

int engine::balance(std::string ID) {
    std::list <bettor>::iterator it;
    it = std::find(bettorList.begin(), bettorList.end(), ID);

    return it->balance;
}

int engine::drawCoins(std::string ID) {
    std::list <bettor>::iterator it;
    it = std::find(bettorList.begin(), bettorList.end(), ID);

    if (time(NULL) - it->lastDrawTime < 86400) {
        return -1;
    }

    it->lastDrawTime = time(NULL);

    int newCoins = eco.drawCoins();
    it->balance += newCoins;
    
    writeFile();

    return newCoins;
}

int engine::registerWager(std::string description, std::string creatorID, std::time_t date, int duration) {
    auto bettorIt = std::find(bettorList.begin(), bettorList.end(), creatorID);
    if (bettorIt->balance < WAGER_PRICE) {
        return -1; // No balance
    }
    bettorIt->balance -= WAGER_PRICE;
    eco.destroyCoins(WAGER_PRICE);

    int wagerID = wagerList.size() + 1;
    wager auxWager(wagerID, description, creatorID, date, duration);
    wagerList.push_back(auxWager);
    writeFile();
    return wagerID;
}

std::list <int> engine::listActiveWagers() {
    std::list <int> activeWagers;

    for (auto it = wagerList.begin(); it != wagerList.end(); it++) {
        if (it->active) {
            activeWagers.push_back(it->ID);
        }
    }

    return activeWagers;
}

int engine::addBet(std::string bettorID, int wagerID, bool outcome, int value) {
    if (value <= 0) {
        return -3; // Negative or zero bet
    }

    auto wagerPtr = getWager(wagerID);
    if (wagerPtr == nullptr) {
        return -1; // No wager with that id
    }
    if (!wagerPtr->open) {
        return -2; // No open wager with that id
    }

    auto bettorIt = std::find(bettorList.begin(), bettorList.end(), bettorID);
    if (bettorIt->balance < value) {
        return 0; // No balance
    }

    for (auto it = wagerPtr->betList.begin(); it != wagerPtr->betList.end(); it++) {
        // if there's already a similar bet by this bettor
        if (bettorID == it->bettorID && outcome == it->outcome) {
            bettorIt->balance -= value;
            it->value += value;
            writeFile();
            return 1; // Repeated bet but okay
        }
    }
    
    bet auxBet;
    auxBet.bettorID = bettorID;
    auxBet.outcome = outcome;
    auxBet.value = value;

    bettorIt->balance -= value;
    wagerPtr->betList.push_back(auxBet);

    writeFile();

    return 1; // New bet okay
}

std::list <settleResponse>  engine::settle(int wagerID, bool outcome) {
    auto wagerPtr = getWager(wagerID);

    wagerPtr->outcome = outcome;
    wagerPtr->open = false;
    wagerPtr->active = false;
    wagerPtr->canceled = false;

    int totalSum = 0;
    int sumOutcome = 0;
    int tempPrize;
    int tax;
    settleResponse tempResponse;
    std::list <settleResponse> outputList;

    for (auto it = wagerPtr->betList.begin(); it != wagerPtr->betList.end(); it++) {
        if (it->outcome == outcome) {
            sumOutcome += it->value;
        }
        totalSum += it->value;
    }

    for (auto it = wagerPtr->betList.begin(); it != wagerPtr->betList.end(); it++) {
        if (it->outcome == outcome) {
            tempPrize = std::round((double) it->value * totalSum / (double) sumOutcome);
            tax = std::round((double) tempPrize * BET_TAX_RATE);
            eco.destroyCoins(tax);
            std::find(bettorList.begin(), bettorList.end(), it->bettorID)->balance += tempPrize - tax;
            tempResponse.bettorID = it->bettorID;
            tempResponse.prize = tempPrize - tax;
            outputList.push_back(tempResponse);
        }
    }

    writeFile();

    return outputList;
}

std::list <std::string> engine::checkNewBettors(std::list <std::string> *membersList) {
    std::list <std::string> newBettors;

    for (auto it = membersList->begin(); it != membersList->end(); it++) {
        auto res = std::find(bettorList.begin(), bettorList.end(), *it);
        // if new member is found
        if (res == bettorList.end()) {
            // add to bettorList
            bettor auxBettor;
            auxBettor.ID = *it;
            auxBettor.balance = eco.drawInitialCoins();
            auxBettor.plays = 0;
            auxBettor.wins = 0;
            auxBettor.lastDrawTime  = 0;
            bettorList.push_back(auxBettor);
            // add ID to return list
            newBettors.push_back(*it);
        }
    }

    writeFile();

    return newBettors;
}

wager *engine::getWager(int wagerID) {
    auto res = std::find(wagerList.begin(), wagerList.end(), wagerID);
    // if a matching is found
    if (res != wagerList.end()) {
        return &*res;
    }
    return nullptr;
}

void engine::readFile() {
    std::ifstream file;
    int size;
    bettor auxBettor;
    wager auxWager;

    bettorList.clear();
    wagerList.clear();

    file.open(_filename.c_str(), std::ios::in);
    if (file.fail()) {
        std::cerr << "Couldn't open save file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    file >> eco;
    file >> size;

    for (int i = 0; i < size; i++) {
        file >> auxBettor;
        bettorList.push_back(auxBettor);
    }

    file >> size;

    for (int i = 0; i < size; i++) {
        file >> auxWager;
        wagerList.push_back(auxWager);
    }

    file.close();
}

void engine::writeFile() {
    std::ofstream file;

    file.open(_filename.c_str(), std::ios::out | std::ios::trunc);
    if (file.fail()) {
        std::cerr << "Couldn't write to save file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    file << eco;
    file << bettorList.size() << std::endl;

    std::list <bettor>::iterator bettorIterator;
    for (bettorIterator = bettorList.begin(); bettorIterator != bettorList.end(); bettorIterator++) {
        file << *bettorIterator;
    }

    file << wagerList.size() << std::endl;

    std::list <wager>::iterator wagerIterator;
    for (wagerIterator = wagerList.begin(); wagerIterator != wagerList.end(); wagerIterator++) {
        file << *wagerIterator;
    }

    file.close();
}
