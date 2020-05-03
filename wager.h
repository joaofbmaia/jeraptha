#pragma once

#include <string>
#include <list>
#include <ctime>

#include "bettor.h"
#include "bet.h"


class wager {
    public:
        wager();
        wager(int ID_, std::string description_, std::string creatorID_, std::time_t date_);
        wager(const wager &obj);

        friend std::ostream &operator<<(std::ostream &out, const wager &obj);
        friend std::istream &operator>>(std::istream &in,  wager &obj);

        int ID;
        std::string description;
        std::string creatorID;
        std::time_t date;
        int odds(bool outcome);
        int openInterest();
    private:
        bool active;
        bool canceled;
        bool outcome;
        std::list <bet> betList;
};