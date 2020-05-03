#include "jeraptha.h"

#include <string>
#include <iostream>
#include <algorithm>

jerapthaClient::jerapthaClient(configuration *config, engine *gameEngine, const char numOfThreads)
: SleepyDiscord::DiscordClient::WebsocketppDiscordClient(config->token, numOfThreads), config(config), gameEngine(gameEngine) {}

void jerapthaClient::onReady(std::string *jsonMessage) {

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

        // register wager <description>
        command = "register wager";
        if (!message.content.compare(config->prefix.length(), command.length(), command)) {
        
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
