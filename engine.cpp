#include "engine.h"

#include <iostream>
#include <fstream>
#include <list>

engine::engine(std::string filename) {
    _filename = filename;
    readFile();
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
