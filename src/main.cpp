#include <iostream>
#include "game/HavelIntegration.h"
#include "game/GameRuntime.h"
#include "game/GameRuntime3D.h"
#include "game/Game3D.h"
#include "graph/GraphApp.h"

static void runScriptGame(HavelScriptIntegration& havel, GameRuntime* rt, const std::string& scriptPath) {
    havel.registerRuntimeBindings(rt);
    if (havel.isInitialized()) {
        if (!havel.executeFile(scriptPath)) {
            std::cerr << "[Game] Script failed: " << havel.getLastError() << std::endl;
            return;
        }
        havel.callFunction("runtime_run");
    }
}

static void runScriptGame3D(HavelScriptIntegration& havel, GameRuntime3D* rt3d, const std::string& scriptPath) {
    havel.registerRuntime3DBindings(rt3d);
    if (havel.isInitialized()) {
        if (!havel.executeFile(scriptPath)) {
            std::cerr << "[Game] Script failed: " << havel.getLastError() << std::endl;
            return;
        }
        havel.callFunction("runtime_run3d");
    }
}

int main(int argc, char *argv[])
{
    HavelScriptIntegration havel;
    if (havel.initialize()) {
        havel.registerDefaultBindings();
        std::cout << "[Main] Havel scripting initialized" << std::endl;
    } else {
        std::cerr << "[Main] Warning: Havel initialization failed" << std::endl;
    }

    if (argc >= 2 && std::string(argv[1]) == "--script-test") {
        if (havel.isInitialized()) {
            if (!havel.executeFile("scripts/test.hv"))
                std::cerr << "[Main] Script failed: " << havel.getLastError() << std::endl;
        }
        return 0;
    }

    std::string mode = argc >= 2 ? argv[1] : "";

    if (mode == "pong") {
        GameRuntime rt;
        if (rt.initWindow("Pong", 800, 600))
            runScriptGame(havel, &rt, "scripts/pong.hv");
        return 0;
    }

    if (mode == "snake") {
        GameRuntime rt;
        if (rt.initWindow("Snake", 600, 600))
            runScriptGame(havel, &rt, "scripts/snake.hv");
        return 0;
    }

    if (mode == "fps") {
        GameRuntime3D rt3d;
        if (rt3d.initWindow("FPS", 1024, 768))
            runScriptGame3D(havel, &rt3d, "scripts/fps.hv");
        return 0;
    }

    Game3D game;
    game.useSolarSystemScene = mode != "--models";
    game.init();
    havel.setGameEngineContext(&game);

    if (havel.isInitialized()) {
        if (!havel.executeFile("scripts/test.hv"))
            std::cerr << "[Main] Script failed: " << havel.getLastError() << std::endl;
    }

    if (mode.empty()) {
        std::cout << "No mode specified, auto-starting in 3D mode..." << std::endl;
        game.run();
    } else if (mode == "--graph") {
        GraphApp app; app.run();
    } else if (mode == "3d" || mode == "--models") {
        game.run();
    } else {
        std::cout << "Use: pong, snake, --graph, --models, 2d, 3d, --script-test" << std::endl;
    }
    return 0;
}
