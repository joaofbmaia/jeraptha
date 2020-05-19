#pragma once

#include <string>
#include <ctime>

class bettor {
    public:
        friend std::ostream &operator<<(std::ostream &out, const bettor &obj);
        friend std::istream &operator>>(std::istream &in,  bettor &obj);
        bool operator==(const std::string& ID_);
        
        std::string ID;
        int balance;
        int wins;
        int plays;
        std::time_t lastDrawTime;
};