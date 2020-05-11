#include "wager.h"

#include <string>
#include <list>
#include <ctime>
#include <fstream>
#include <cmath>

wager::wager() 
: ID(-1), date(0), duration(0), active(0), open(0), canceled(0), outcome(0) {}

wager::wager(int ID_, std::string description_, std::string creatorID_, std::time_t date_, int duration_) 
: ID(ID_), description(description_), creatorID(creatorID_), date(date_), duration(duration_), active(1), open(1), canceled(0), outcome(0) {}

wager::wager(const wager &obj) {
    ID = obj.ID;
    description = obj.description;
    creatorID = obj.creatorID;
    date = obj.date;
    duration = obj.duration;
    active = obj.active;
    open = obj.open;
    canceled = obj.canceled;
    outcome = obj.outcome;
    betList = obj.betList;
}

std::ostream &operator<<(std::ostream &out, const wager &obj) {
    out << obj.ID << "\n" << obj.description << "\n" << obj.creatorID << "\n" << obj.date << "\n" << obj.duration <<  std::endl;
    out << obj.active << "\n" << obj.open << "\n" << obj.canceled << "\n" << obj.outcome << std::endl;

    out << obj.betList.size() << std::endl;

    std::list <bet>::const_iterator betIterator;
    for (betIterator = obj.betList.begin(); betIterator != obj.betList.end(); betIterator++) {
        out << betIterator->bettorID << "\n" << betIterator->outcome << "\n" << betIterator->value << std::endl;
    }

    return out;
}

std::istream &operator>>(std::istream &in,  wager &obj) {
    int size;
    bet auxBet;

    obj.betList.clear();

    in >> obj.ID;
    in.ignore(); //ignore newline
    std::getline(in, obj.description);
    in >> obj.creatorID;
    in >> obj.date;
    in >> obj.duration;
    in >> obj.active;
    in >> obj.open;
    in >> obj.canceled;
    in >> obj.outcome;

    in >> size;
    for (int i = 0; i < size; i++) {
        in >> auxBet.bettorID;
        in >> auxBet.outcome;
        in >> auxBet.value;

        obj.betList.push_back(auxBet);
    }

    return in;
}

bool wager::operator==(const int& ID_) {
	return ID == ID_;
}

int wager::odds(bool outcome) {
    int sumTrue = 0;
    int sumFalse = 0;
    for (auto it = betList.begin(); it != betList.end(); it++) {
        if (it->outcome == true) {
            sumTrue += it->value;
        }
        else {
            sumFalse += it->value;
        }
    }
    int probTrue = std::round((double) sumTrue * 100 / (double) (sumTrue + sumFalse));
    if (outcome) return probTrue;
    else return 100 - probTrue;
}

int wager::openInterest() {
    int sum = 0;
    for (auto it = betList.begin(); it != betList.end(); it++) {
        sum += it->value;
    }
    return sum;
}