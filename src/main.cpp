#include <iostream>
#include "game/Game.h"
#include "game/Game3D.h"

int main(int argc, char *argv[])
{
    // Auto-start in 3D mode if no arguments are provided
    if (argc < 2) {
        std::cout << "No mode specified, auto-starting in 3D mode..." << std::endl;
        Game3D game;
        game.init();
        game.run();
        return 0;
    }

    std::string mode = argv[1];
    std::string type = argc > 2 ? argv[2] : "Default";

    if (mode == "2d") {
        // return game2d(argc, argv, type);
        return 0;
    } else if (mode == "3d") {
        Game3D game;
        game.init();
        game.run();
        return 0;
    } else {
        std::cout << "Invalid mode. Use 2d or 3d." << std::endl;
        return -1;
    }
}