#include <iostream>
#include "game/Game.h"
#include "game/Game3D.h"
#include "graph/GraphApp.h"
#include "game/HavelIntegration.h"

int main(int argc, char *argv[])
{
    HavelScriptIntegration havel;
    if (havel.initialize()) {
        havel.registerDefaultBindings();
        std::cout << "[Main] Havel scripting initialized" << std::endl;
    } else {
        std::cerr << "[Main] Warning: Havel initialization failed: " << havel.getLastError() << std::endl;
    }

    // Script-only test mode: skip game init entirely
    if (argc >= 2 && std::string(argv[1]) == "--script-test") {
        if (havel.isInitialized()) {
            std::string scriptPath = "/home/all/repos/game-gl/scripts/test.hv";
            std::cerr << "[Main] Loading script: " << scriptPath << std::endl;
            if (!havel.executeFile(scriptPath)) {
                std::cerr << "[Main] Script failed: " << havel.getLastError() << std::endl;
            }
        }
        return 0;
    }

    bool loadModelsScene = false;
    std::string mode;

    if (argc >= 2) {
        mode = argv[1];
        if (mode == "--models") {
            loadModelsScene = true;
        }
    }

    Game3D game;
    game.useSolarSystemScene = !loadModelsScene;
    game.init();

    havel.setGameEngineContext(&game);

    if (havel.isInitialized()) {
        if (!havel.executeFile("scripts/test.hv")) {
            std::cerr << "[Main] Script failed: " << havel.getLastError() << std::endl;
        }
    }

    if (argc < 2) {
        std::cout << "No mode specified, auto-starting in 3D mode..." << std::endl;
        game.run();
        return 0;
    }

    if (mode == "--graph") {
        GraphApp app;
        app.run();
        return 0;
    } else if (mode == "2d") {
        return 0;
    } else if (mode == "3d" || mode == "--models") {
        game.run();
        return 0;
    } else {
        std::cout << "Invalid mode. Use --graph, --models, 2d or 3d." << std::endl;
        return -1;
    }
}
