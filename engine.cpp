#include "engine.h"

#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <ctime>

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

int engine::registerWager(std::string description, std::string creatorID, std::time_t date) {
    int wagerID = wagerList.size() + 1;
    wager auxWager(wagerID, description, creatorID, date);
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
