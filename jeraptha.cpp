#include "jeraptha.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include <time.h>

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
    if (message.startsWith(config->prefix) && message.serverID == config->serverID && !message.author.bot) {
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
                time_t timeLeft = 86400 - wageringEngine->timeElapsedDraw(message.author.ID);
                auto timeLeftStruct = gmtime(&timeLeft);
                std::stringstream buffer;
                buffer << "Next daily available in ";
                if (timeLeftStruct->tm_hour == 0 && timeLeftStruct->tm_min == 0) {
                    buffer << timeLeftStruct->tm_sec << " seconds.";
                }
                else if (timeLeftStruct->tm_hour == 0 && timeLeftStruct->tm_min > 0) {
                    buffer << timeLeftStruct->tm_min << " minutes and " << timeLeftStruct->tm_sec << " seconds.";
                }
                else {
                    buffer << timeLeftStruct->tm_hour << " hours and " << timeLeftStruct->tm_min << " minutes.";
                }
                sendMessage(message.channelID, buffer.str());
            }
        }

        // register wager <duration> <description>
        command = "register wager ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            std::string rawWager = message.content.substr(config->prefix.length() + command.length());
            char descriptionString[512];
            int duration;

            int ret = sscanf(rawWager.c_str(), "%d %511[^\n]", &duration, descriptionString);
            if (ret != 2 || strlen(descriptionString) == 0 || duration < 0) {
                sendMessage(message.channelID, std::string("Invalid format."));
            }
            else {
                int wagerID = wageringEngine->registerWager(std::string(descriptionString), message.author.ID, time(NULL), duration);
                if (wagerID == -1) {
                    sendMessage(message.channelID, message.author.username + std::string(", you don't have enough credits to register a wager."));
                }
                else {
                    if (duration > 0) {
                        sendMessage(message.channelID, std::string("Registered wager \\\"") + std::string(descriptionString) + std::string("\\\" with ID ") + std::to_string(wagerID) + std::string(" and a duration of ") + std::to_string(duration) + std::string(" days.\\nIt cost you 5 credits."));
                    }
                    else {
                        sendMessage(message.channelID, std::string("Registered wager \\\"") + std::string(descriptionString) + std::string("\\\" with ID ") + std::to_string(wagerID) + std::string(" and indefinite duration.\\nIt cost you 5 credits."));
                    }
                }
            }
        }

        // list wagers
        command = "list wagers";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            auto activeWagers = wageringEngine->listActiveWagers();
            std::stringstream buffer;

            for (auto it = activeWagers.begin(); it != activeWagers.end(); it++) {
                buffer << std::to_string(*it) << ": " << wageringEngine->getWager(*it)->description << "\\n";
                if (wageringEngine->getWager(*it)->betList.size() != 0) {
                    buffer << ":white_check_mark: " << wageringEngine->getWager(*it)->odds(true) << "% | :x: " << wageringEngine->getWager(*it)->odds(false) << "%";
                }
                else {
                    buffer << "No bets yet! :disappointed_relieved:";
                }
                if (!wageringEngine->getWager(*it)->open) {
                    buffer << " (Closed)";
                }
                buffer << "\\n\\n";
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
                    sendMessage(message.channelID, message.author.username + std::string(" bet ") + std::to_string(value) + std::string(" credits on ") + (outcome ? std::string("YES") : std::string("NO")) + std::string(" for \\\"") + wageringEngine->getWager(wagerID)->description + std::string("\\\"."));
                }
                else if (allowed == 0) {
                    sendMessage(message.channelID, message.author.username + std::string(", you do not have enough credits to make this bet."));
                }
                else if (allowed == -3) {
                    sendMessage(message.channelID, std::string("You can only bet positive ammounts."));
                }
                else if (allowed == -2) {
                    sendMessage(message.channelID, std::string("That wager is closed."));
                }
                else {
                    sendMessage(message.channelID, std::string("There's no wager with that ID."));
                }
            }
        }

        // wager settle <wagerID> <outcome>
        command = "wager settle ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            //check if author has admin role
            auto roles = getMember(message.serverID, message.author.ID).cast().roles;
            if (std::find(roles.begin(), roles.end(), config->adminRoleID) != roles.end()
                || message.author.ID == config->ownerID) {
                int wagerID;
                char outcomeString[4];
                int ret = sscanf(message.content.substr(config->prefix.length() + command.length()).c_str(), "%d %3s", &wagerID, outcomeString);
                if (ret != 2 || (strcmp(outcomeString, "yes") != 0 && strcmp(outcomeString, "no") != 0)) {
                    sendMessage(message.channelID, std::string("Invalid format."));
                }
                else {
                    bool outcome;
                    if (strcmp(outcomeString, "yes") == 0) outcome = true;
                    else outcome = false;
                    if (wageringEngine->getWager(wagerID) == nullptr) {
                        sendMessage(message.channelID, std::string("There's no wager with that ID."));
                    }
                    else if (!wageringEngine->getWager(wagerID)->active) {
                        sendMessage(message.channelID, std::string("That wager is inactive."));
                    }
                    else {
                        auto prizeList = wageringEngine->settle(wagerID, outcome);
                        if (prizeList.empty()) {
                            sendMessage(message.channelID, std::string("There were no prizes for this wager."));
                        }
                        else {
                            std::stringstream buffer;
                            buffer << "The wager \\\"" << wageringEngine->getWager(wagerID)->description << "\\\" was settled to " << (outcome ? "YES" : "NO") << "!\\n";
                            for (auto it = prizeList.begin(); it != prizeList.end(); it++) {
                                buffer << "<@" << it->bettorID << "> won " << it->prize << " credits!\\n";
                            }
                            buffer << "Congratulations to the winners! :partying_face: :dollar:";
                            sendMessage(message.channelID, buffer.str());
                        }
                    }
                }
            }
            else {
                sendMessage(message.channelID, "You do not have permission to use this command.");
            }
        }

        // wager cancel <wagerID>
        command = "wager cancel ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            //check if author has admin role
            auto roles = getMember(message.serverID, message.author.ID).cast().roles;
            if (std::find(roles.begin(), roles.end(), config->adminRoleID) != roles.end()
                || message.author.ID == config->ownerID) {
                int wagerID;
                int ret = sscanf(message.content.substr(config->prefix.length() + command.length()).c_str(), "%d", &wagerID);
                if (ret != 1) {
                    sendMessage(message.channelID, std::string("Invalid format."));
                }
                else {
                    if (wageringEngine->getWager(wagerID) == nullptr) {
                        sendMessage(message.channelID, std::string("There's no wager with that ID."));
                    }
                    else if (!wageringEngine->getWager(wagerID)->active) {
                        sendMessage(message.channelID, std::string("That wager is inactive."));
                    }
                    else {
                        wageringEngine->cancel(wagerID);
                        sendMessage(message.channelID, std::string("Wager canceled. Credits were returned to bettors."));
                    }
                }
            }
            else {
                sendMessage(message.channelID, "You do not have permission to use this command.");
            }
        }

        // wager close <wagerID>
        command = "wager close ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            //check if author has admin role
            auto roles = getMember(message.serverID, message.author.ID).cast().roles;
            if (std::find(roles.begin(), roles.end(), config->adminRoleID) != roles.end()
                || message.author.ID == config->ownerID) {
                int wagerID;
                int ret = sscanf(message.content.substr(config->prefix.length() + command.length()).c_str(), "%d", &wagerID);
                if (ret != 1) {
                    sendMessage(message.channelID, std::string("Invalid format."));
                }
                else {
                    wageringEngine->updateClosedBets();
                    if (wageringEngine->getWager(wagerID) == nullptr) {
                        sendMessage(message.channelID, std::string("There's no wager with that ID."));
                    }
                    else if (!wageringEngine->getWager(wagerID)->active) {
                        sendMessage(message.channelID, std::string("That wager is inactive."));
                    }
                    else if (!wageringEngine->getWager(wagerID)->open) {
                        sendMessage(message.channelID, std::string("That wager is already closed."));
                    }
                    else {
                        wageringEngine->close(wagerID);
                        sendMessage(message.channelID, std::string("Wager closed. New bets can no longer be placed."));
                    }
                }
            }
            else {
                sendMessage(message.channelID, "You do not have permission to use this command.");
            }
        }

        // wager details <wagerID>
        command = "wager details ";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            int wagerID;
            int ret = sscanf(message.content.substr(config->prefix.length() + command.length()).c_str(), "%d", &wagerID);
            if (ret != 1) {
                sendMessage(message.channelID, std::string("Invalid format."));
            }
            else {
                wageringEngine->updateClosedBets();
                if (wageringEngine->getWager(wagerID) == nullptr) {
                    sendMessage(message.channelID, std::string("There's no wager with that ID."));
                }
                else {
                    std::stringstream buffer;
                    buffer << wagerID << ": " << wageringEngine->getWager(wagerID)->description << "\\n";
                if (wageringEngine->getWager(wagerID)->betList.size() != 0) {
                    buffer << ":white_check_mark: " << wageringEngine->getWager(wagerID)->odds(true) << "% | :x: " << wageringEngine->getWager(wagerID)->odds(false) << "%";
                }
                else {
                    buffer << "No bets yet! :disappointed_relieved:";
                }
                if (!wageringEngine->getWager(wagerID)->active && !wageringEngine->getWager(wagerID)->canceled) {
                    buffer << " (Settled)";
                    buffer << "\\nOutcome: " << (wageringEngine->getWager(wagerID)->outcome ? "YES :white_check_mark:" : "NO :x:");
                }
                else if (wageringEngine->getWager(wagerID)->canceled) {
                    buffer << " (Canceled)";
                }
                else if (!wageringEngine->getWager(wagerID)->open) {
                    buffer << " (Closed)";
                }
                else if (wageringEngine->getWager(wagerID)->duration == 0) {
                    buffer << " (Active)";
                }
                else {
                    buffer << " (Active for " << wageringEngine->getWager(wagerID)->duration << " days)";
                }

                buffer << "\\nCreated by " << getMember(config->serverID, wageringEngine->getWager(wagerID)->creatorID).cast().user.username << " on ";
                buffer << std::put_time(std::gmtime(&wageringEngine->getWager(wagerID)->date), "%Y-%m-%d") << "\\n\\n";
                buffer << "Total ammount bet on this wager: " << wageringEngine->getWager(wagerID)->openInterest() << " credits\\n";

                auto members = listMembers(config->serverID, 100).vector();
                for (auto it = wageringEngine->getWager(wagerID)->betList.begin(); it != wageringEngine->getWager(wagerID)->betList.end(); it++) {
                    std::string username = "[user left]";
                    for (auto memberIT = members.begin(); memberIT != members.end(); memberIT++) {
                        if (memberIT->user.ID == it->bettorID) {
                            username = memberIT->user.username;
                            break;
                        }
                    }

                    buffer << "- " << username << ": ";
                    buffer << it->value << " credits on ";
                    buffer << (it->outcome ? "YES :white_check_mark:" : "NO :x:") << "\\n";
                }

                sendMessage(message.channelID, buffer.str());

                }
            }
        }

    }
    
    //DM's + server messages
    if (message.startsWith(config->prefix) && !message.author.bot) {
        std::string command;

        // help
        command = "help";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
            SleepyDiscord::DMChannel dmChannel = createDirectMessageChannel(message.author.ID);
            if (message.serverID == config->serverID) {
                sendMessage(message.channelID, std::string("Sent you a DM!"));
            }
            std::stringstream helpString;
            helpString << "Hello! I'm Jeraptha, the wagering bot!\\n\\n";
            helpString << "**User commands:**\\n\\n";
            helpString << "*register wager <duration> <description>*\\n";
            helpString << "Creates a wager that's open for betting during <duration> days. To make this wager open indefinitely user 0 for <duration>.\\nOpening a wager will cost you 5 credits.\\n\\n";
            helpString << "*list wagers*\\n";
            helpString << "Lists all active wagers.\\n\\n";
            helpString << "*wager details <ID>*\\n";
            helpString << "Lists details for a wager with a certain <ID>.\\n\\n";
            helpString << "*bet <ID> <yes/no> <ammount>*\\n";
            helpString << "Creates a bet on the wager with that <ID> with the choosen <ammount> of credits. You can only bet \\\"yes\\\" or \\\"no\\\".\\n\\n";
            helpString << "*balance*\\n";
            helpString << "Shows your balance of credits.\\n\\n";
            helpString << "*daily*\\n";
            helpString << "Gifts you a random ammount of credits. Can only be used once every 24 hours.\\n\\n";
            helpString << "**Admin commands:**\\n\\n";
            helpString << "*wager close <ID>*\\n";
            helpString << "Manually closes betting for wager with that <ID>.\\n\\n";
            helpString << "*wager settle <ID> <outcome>*\\n";
            helpString << "Settles the wager with that <ID>, closing it and distributing the prizes. The <outcome> must be \\\"yes\\\" or \\\"no\\\".\\n\\n";
            helpString << "*wager cancel <ID>*\\n";
            helpString << "Cancels all bets on the wager with that <ID> and returns the credits back to bettors.\\n";
            sendMessage(std::string(dmChannel.ID), helpString.str());
        }

    }

}
