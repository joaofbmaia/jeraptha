#include "configuration.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

configuration::configuration(std::string filename) {
    _filename = filename;
    readFile();
}

void configuration::readFile(void) {
    std::ifstream file;
    std::string line;

    file.open(_filename.c_str(), std::ios::in);
    if (file.fail()) {
        std::cerr << "Couldn't open config file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(file, line)) {
        if (line.length() == 0)
            continue;

        std::istringstream iss(line);
        std::string name;
        std::string value;

        if (!std::getline(iss, name, '=')) {
            std::cerr << "Error reading config file!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::getline(iss, value, '=');

        if (name == "token") {
            token = value;
        }
        else if (name == "prefix") {
            prefix = value;
        }
        else if (name == "serverID") {
            serverID = value;
        }
        else if (name == "ownerID") {
            ownerID = value;
        }
        else if (name == "adminRoleID") {
            adminRoleID = value;
        }
        else {
            std::cerr << "Unknown parameter in config file!" << std::endl;
            exit(EXIT_FAILURE);
        }

    }

    file.close();

}

void configuration::writeFile(void) {
    std::ofstream file;

    file.open(_filename.c_str(), std::ios::out | std::ios::trunc);
    if (file.fail()) {
        std::cerr << "Couldn't write to config file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    file << std::string("token=") << token << std::endl;
    file << std::string("prefix=") << prefix << std::endl;
    file << std::string("serverID=") << serverID << std::endl;
    file << std::string("ownerID=") << ownerID << std::endl;
    file << std::string("adminRoleID=") << adminRoleID << std::endl;

    file.close();
}