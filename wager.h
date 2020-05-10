#pragma once

#include <string>
#include <list>
#include <ctime>

#include "bettor.h"

struct bet {
    std::string bettorID;
    bool outcome;
    int value;
};

class wager {
    public:
        wager();
        wager(int ID_, std::string description_, std::string creatorID_, std::time_t date_, int duration_);
        wager(const wager &obj);

        friend std::ostream &operator<<(std::ostream &out, const wager &obj);
        friend std::istream &operator>>(std::istream &in,  wager &obj);
        bool operator==(const int& ID_);

        int ID;
        std::string description;
        std::string creatorID;
        std::time_t date;
        int duration;
        int odds(bool outcome);
        int openInterest();
        bool active;
        bool open;
        bool canceled;
        bool outcome;
        std::list <bet> betList;
};