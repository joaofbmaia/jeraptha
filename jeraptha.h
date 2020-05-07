#pragma once

#include "sleepy_discord/websocketpp_websocket.h"

#include "configuration.h"
#include "engine.h"

#include <string>

class jerapthaClient : public SleepyDiscord::DiscordClient {
public:
    using SleepyDiscord::DiscordClient::DiscordClient;
    jerapthaClient(configuration *config, engine *gameEngine, const char numOfThreads);
    configuration *config;
    engine *wageringEngine;
    void onReady(std::string *jsonMessage);
    void onMember(std::string *jsonMessage);
    void onMessage(SleepyDiscord::Message message);
};