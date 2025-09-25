#include <iostream>
#include "game/Game.h"
#include "game/Game3D.h"
#include "graph/GraphApp.h"

int main(int argc, char *argv[])
{
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
    } else if (mode == "2d") {
        // return game2d(argc, argv, type);
        return 0;
    } else if (mode == "3d") {
        Game3D game;
        game.init();
        game.run();
        return 0;
    } else {
        std::cout << "Invalid mode. Use --graph, 2d or 3d." << std::endl;
        return -1;
    }
}