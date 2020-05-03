#pragma once

#include <string>

class configuration {
    public:
        configuration(std::string filename);
        std::string token;
        std::string prefix;
        std::string serverID;
        std::string ownerID;
        std::string adminRoleID;
        void writeFile(void);
    private:
        void readFile(void);
        std::string _filename;
};