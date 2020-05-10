#include "jeraptha.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <ctime>
#include <stdio.h>

jerapthaClient::jerapthaClient(configuration *config, engine *gameEngine, const char numOfThreads)
: SleepyDiscord::DiscordClient::WebsocketppDiscordClient(config->token, numOfThreads), config(config), wageringEngine(gameEngine) {}

void jerapthaClient::onReady(std::string *jsonMessage) {
    // Construct list with all members IDs as strings
    //auto members = listMembers(config->serverID).list();
    std::list <std::string> membersIDs;
    auto members = listMembers(config->serverID, 100).vector();

    for (auto it = members.begin(); it != members.end(); it++) {
        // exclude bots
        if (!it->user.bot) {
            membersIDs.push_back(std::string(it->user.ID));
            //std::cout << std::string(it->user.ID) << std::endl;
        }
    }

    // call engine function to check new members and draw them initial coins
    std::list <std::string> newMembers;
    newMembers = wageringEngine->checkNewBettors(&membersIDs);

    // print results
    std::stringstream buffer;
    for (auto it = newMembers.begin(); it != newMembers.end(); it++) {
        buffer << "Welcome <@" << *it << ">, here's " << wageringEngine->balance(*it) << " credits to get you going!" << "\\n";
    }
    if (buffer.str().size() != 0) {
        sendMessage(config->defaultChannelID, buffer.str());
    }
}


void jerapthaClient::onMember(std::string *jsonMessage) {
    // Construct list with all members IDs as strings
    //auto members = listMembers(config->serverID).list();
    std::list <std::string> membersIDs;
    auto members = listMembers(config->serverID, 100).vector();

    for (auto it = members.begin(); it != members.end(); it++) {
        // exclude bots
        if (!it->user.bot) {
            membersIDs.push_back(std::string(it->user.ID));
            //std::cout << std::string(it->user.ID) << std::endl;
        }
    }

    // call engine function to check new members and draw them initial coins
    std::list <std::string> newMembers;
    newMembers = wageringEngine->checkNewBettors(&membersIDs);

    // print results
    std::stringstream buffer;
    for (auto it = newMembers.begin(); it != newMembers.end(); it++) {
        buffer << "Welcome <@" << *it << ">, here's " << wageringEngine->balance(*it) << " credits to get you going!" << "\\n";
    }
    if (buffer.str().size() != 0) {
        sendMessage(config->defaultChannelID, buffer.str());
    }
}

void jerapthaClient::onMessage(SleepyDiscord::Message message) {
    //server messages
    if (message.startsWith(config->prefix) && message.serverID == config->serverID) {
        std::string command;

        // admin role @role
        command = "admin role";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            //check if author has admin role
            auto roles = getMember(message.serverID, message.author.ID).cast().roles;
            if (std::find(roles.begin(), roles.end(), config->adminRoleID) != roles.end()
                || message.author.ID == config->ownerID) {
                //find role mention
                size_t startPosition = message.content.find("<@&", config->prefix.length() + command.length());
                if (startPosition != std::string::npos) {
                    size_t endPosition = message.content.find(">", config->prefix.length() + command.length());
                    //check if role mention found
                    if (endPosition != std::string::npos) {
                        std::string role = message.content.substr(startPosition + 3, endPosition - (startPosition + 3));
                        config->adminRoleID = role;
                        config->writeFile();
                        sendMessage(message.channelID, std::string("Admin role was changed to <@&") + role + ">");
                    }
                }
                else {
                    sendMessage(message.channelID, "No role was mentioned.");
                }
            }
            else {
                sendMessage(message.channelID, "You do not have permission to use this command.");
            }
        }

        // prefix <prefix>
        command = "prefix";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            //check if author has admin role
            auto roles = getMember(message.serverID, message.author.ID).cast().roles;
            if (std::find(roles.begin(), roles.end(), config->adminRoleID) != roles.end()
                || message.author.ID == config->ownerID) {
                //find role mention
                size_t startPosition = message.content.find(" ", config->prefix.length() + command.length());
                if (startPosition != std::string::npos) {
                    
                    //check if prefix string found
                    if (message.content.length() > config->prefix.length() + command.length() + 1) {
                        std::string prefix = message.content.substr(startPosition + 1);
                        config->prefix = prefix;
                        config->writeFile();
                        sendMessage(message.channelID, std::string("Prefix was changed to ") + prefix);
                    }
                }
                else {
                    sendMessage(message.channelID, "No prefix mentioned.");
                }
            }
            else {
                sendMessage(message.channelID, "You do not have permission to use this command.");
            }
        }

        // balance
        command = "balance";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            int bal = wageringEngine->balance(message.author.ID);
            sendMessage(message.channelID, message.author.username + std::string(", you have a balance of ") + std::to_string(bal) + std::string(" credits."));
        }

        // daily
        command = "daily";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            int coins = wageringEngine->drawCoins(message.author.ID);
            if (coins >= 0) {
                sendMessage(message.channelID, message.author.username + std::string(", you have received ") + std::to_string(coins) + std::string(" credits."));
            }
            else {
                sendMessage(message.channelID, message.author.username + std::string(", you've already redeemed you credits today."));
            }
        }

        // register wager <description>
        command = "register wager ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            std::string description = message.content.substr(config->prefix.length() + command.length());
            if (description.length() != 0) {
                int wagerID = wageringEngine->registerWager(description, message.author.ID, time(NULL));
                sendMessage(message.channelID, std::string("Registered wager \\\"") + description + std::string("\\\" with ID ") + std::to_string(wagerID));
            }
            else {
                sendMessage(message.channelID, std::string("Invalid format."));
            }
        }

        // list wagers
        command = "list wagers";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            auto activeWagers = wageringEngine->listActiveWagers();
            std::stringstream buffer;

            for (auto it = activeWagers.begin(); it != activeWagers.end(); it++) {
                buffer << std::to_string(*it) << ": " << wageringEngine->getWager(*it)->description << "\\n";
                buffer << ":white_check_mark: " << wageringEngine->getWager(*it)->odds(true) << "% | :x: " << wageringEngine->getWager(*it)->odds(false) << "%" << "\\n\\n";
            }

            if (buffer.str().size() != 0) {
                sendMessage(message.channelID, buffer.str());
            }
            else {
                sendMessage(message.channelID, std::string("No active wagers :disappointed_relieved:\\nWhy don't you register one?"));
            }
        }

        // bet <wagerID> <yes/no> <credits>
        command = "bet ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            std::string rawBet = message.content.substr(config->prefix.length() + command.length());
            int wagerID;
            char outcomeString[4];
            int value;
            bool outcome;

            int ret = sscanf(rawBet.c_str(), "%d %3s %d", &wagerID, outcomeString, &value); 
            if (ret != 3 || (strcmp(outcomeString, "yes") != 0 && strcmp(outcomeString, "no") != 0)) {
                sendMessage(message.channelID, std::string("Invalid format."));
            }
            else {
                if (strcmp(outcomeString, "yes") == 0) outcome = true;
                else outcome = false;
                int allowed = wageringEngine->addBet(message.author.ID, wagerID, outcome, value);
                if (allowed == 1) {
                    sendMessage(message.channelID, message.author.username + std::string(" bet ") + std::to_string(value) + std::string(" credits on ") + (outcome ? std::string("YES") : std::string("NO")) + std::string(" for \\\"") + wageringEngine->getWager(wagerID)->description + std::string(" \\\"."));
                }
                else if (allowed == 0) {
                    sendMessage(message.channelID, message.author.username + std::string(", you do not have enough credits to make this bet."));
                }
                else if (allowed == -3) {
                    sendMessage(message.channelID, std::string("You can only bet positive ammounts."));
                }
                else {
                    sendMessage(message.channelID, std::string("There's no wager with that ID."));
                }
            }
        }

    }
    
    //DM's + server messages
    if (message.startsWith(config->prefix)) {
        std::string command;

        // help
        command = "help";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            SleepyDiscord::DMChannel dmChannel = createDirectMessageChannel(message.author.ID);
            if (message.serverID == config->serverID) {
                sendMessage(message.channelID, std::string("Sent you a DM!"));
            }
            sendMessage(std::string(dmChannel.ID), std::string("help placeholder"));
        }

    }

}
