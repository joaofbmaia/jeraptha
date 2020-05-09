#include "jeraptha.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <ctime>

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
