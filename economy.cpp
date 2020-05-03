#include "economy.h"

#include <ctime>
#include <cmath>
#include <time.h>
#include <random>
#include <string>
#include <fstream>

#define STARTING_COINS 5000
#define INFLATION_RATE 10 //anual inflation rate (1 = 100%)

#define INITIAL_COINS_DRAW 300

#define ESTIMATED_PLAYS_PER_DAY 5

#define LOGISTIC_MAX 10
#define LOGISTIC_STEEPNESS 0.7

#define STD_DEV_RATIO 0.17

std::ostream &operator<<(std::ostream &out, const economy &obj) {
	out << obj.startDate << "\n" << obj.coinsCreated << "\n" << obj.coinsDestroyed << std::endl;
	return out;
}

std::istream &operator>>(std::istream &in,  economy &obj) {
    in >> obj.startDate;
	in >> obj.coinsCreated;
	in >> obj.coinsDestroyed;
	return in;
}

int economy::targetCoinsToday(std::time_t now) {
    double dailyRate = pow(INFLATION_RATE, (double) 1 / 365);
    int daysElapsed = (now - startDate) / 86400;
    return round(STARTING_COINS * pow(dailyRate, daysElapsed));
}

// don't bother to understand
int economy::drawCoins() {
    std::time_t today = time(NULL);
    std::time_t yesterday = today - 86400;
    int dailyTarget = targetCoinsToday(today) - targetCoinsToday(yesterday);

    int currentCoins = coinsCreated - coinsDestroyed;

    double gain = (double) (targetCoinsToday(today) - currentCoins) / dailyTarget;
    
    double logisticMidpoint = log(LOGISTIC_MAX - 1) / LOGISTIC_STEEPNESS + 1;
    double logisticGain = LOGISTIC_MAX / (1 + exp(-LOGISTIC_STEEPNESS * (gain - logisticMidpoint)));

    double mean = logisticGain * ((double) dailyTarget / ESTIMATED_PLAYS_PER_DAY);

    std::random_device rd{};
    std::mt19937 gen{rd()};

    std::normal_distribution<> d{mean, mean * STD_DEV_RATIO};
    
    int coins = round(d(gen));
    if (coins < 0) coins = 0;

    coinsCreated += coins;

    return coins;
}

int economy::drawInitialCoins() {
    double mean = INITIAL_COINS_DRAW;

    std::random_device rd{};
    std::mt19937 gen{rd()};

    std::normal_distribution<> d{mean, mean * STD_DEV_RATIO};
    
    int coins = round(d(gen));
    if (coins < 0) coins = 0;

    coinsCreated += coins;

    return coins;
}

void economy::destroyCoins(int value) {
    coinsDestroyed += value;
}
