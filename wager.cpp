#include "wager.h"
#include "bet.h"

#include <string>
#include <list>
#include <ctime>
#include <fstream>

wager::wager() 
: ID(-1), date(0), active(0), canceled(0), outcome(0) {}

wager::wager(int ID_, std::string description_, std::string creatorID_, std::time_t date_) 
: ID(ID_), description(description_), creatorID(creatorID_), date(date_), active(1), canceled(0), outcome(0) {}

wager::wager(const wager &obj) {
    ID = obj.ID;
    description = obj.description;
    creatorID = obj.creatorID;
    date = obj.date;
    active = obj.active;
    canceled = obj.canceled;
    outcome = obj.outcome;
    betList = obj.betList;
}

std::ostream &operator<<(std::ostream &out, const wager &obj) {
    out << obj.ID << "\n" << obj.description << "\n" << obj.creatorID << "\n" << obj.date <<  std::endl;
    out << obj.active << "\n" << obj.canceled << "\n" << obj.outcome << std::endl;

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
    in >> obj.active;
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