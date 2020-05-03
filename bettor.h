#pragma once

#include <string>

class bettor {
    public:
        friend std::ostream &operator<<(std::ostream &out, const bettor &obj);
        friend std::istream &operator>>(std::istream &in,  bettor &obj);

        friend bettor *getBettor(std::string ID);
        
        std::string ID;
        int balance;
        int wins;
        int plays;
};