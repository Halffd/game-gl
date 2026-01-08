#include <iostream>
#include "game/Game.h"
#include "game/Game3D.h"
#include "graph/GraphApp.h"
#include "ConfigManager.hpp"

int main(int argc, char *argv[])
{
    bool loadModelsScene = false; // New flag

    // Initialize configuration system
    game::cfg().Load();

    if (argc < 2) {
        std::cout << "No mode specified, auto-starting in 3D mode..." << std::endl;
        Game3D game;
        game.init();
        game.run();
        return 0;
    }

    std::string mode = argv[1];

    if (mode == "--graph") {
        GraphApp app;
        app.run();
        return 0;
    } else if (mode == "--models") { // New mode
        loadModelsScene = true;
        // Fall through to 3d mode to initialize Game3D
    } else if (mode == "2d") {
        // return game2d(argc, argv, type);
        return 0;
    } else if (mode == "3d") {
        // This is the default 3D mode, will use solar system unless --models is specified
    } else {
        std::cout << "Invalid mode. Use --graph, --models, 2d or 3d." << std::endl;
        return -1;
    }

    // Initialize and run Game3D based on flags
    Game3D game;
    game.useSolarSystemScene = !loadModelsScene; // Set flag in Game3D
    game.init();
    game.run();
    return 0;
}