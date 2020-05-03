#pragma once

#include <ctime>
#include <string>

class economy {
    public:
        friend std::ostream &operator<<(std::ostream &out, const economy &obj);
        friend std::istream &operator>>(std::istream &in,  economy &obj);
        int drawCoins();
        int drawInitialCoins();
        void destroyCoins(int value);
    private:
        std::time_t startDate;
        int coinsCreated;
        int coinsDestroyed;
        int targetCoinsToday(std::time_t now);
};
