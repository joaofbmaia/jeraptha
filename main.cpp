#include "jeraptha.h"
#include "configuration.h"
#include "engine.h"

#define CONFIG_FILE "jeraptha.cfg"
#define SAVE_FILE "jeraptha.dat"

int main() {
    configuration config(CONFIG_FILE);
    engine gameEngine(SAVE_FILE);

    jerapthaClient client(&config, &gameEngine, 2);
    client.run();
}