#include "bettor.h"

#include <string>
#include <fstream>

std::ostream &operator<<(std::ostream &out, const bettor &obj) {
    out << obj.ID << "\n" << obj.balance << "\n" << obj.wins << "\n" << obj.plays << std::endl;
	return out;
}

std::istream &operator>>(std::istream &in,  bettor &obj) {
    in >> obj.ID;
	in >> obj.balance;
	in >> obj.wins;
    in >> obj.plays;
	return in;
}