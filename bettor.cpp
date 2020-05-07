#include "bettor.h"

#include <string>
#include <fstream>

std::ostream &operator<<(std::ostream &out, const bettor &obj) {
    out << obj.ID << "\n" << obj.balance << "\n" << obj.wins << "\n" << obj.plays << "\n" << obj.lastDrawTime << std::endl;
	return out;
}

std::istream &operator>>(std::istream &in,  bettor &obj) {
    in >> obj.ID;
	in >> obj.balance;
	in >> obj.wins;
    in >> obj.plays;
	in >> obj.lastDrawTime;
	return in;
}

bool bettor::operator==(const std::string& ID_) {
	return ID == ID_;
}