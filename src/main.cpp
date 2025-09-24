#include <iostream>
#include "game/Game.h"
#include "game/Game3D.h"
#include "graph/Graph.h"
#include "graph/Graph2D.h"
#include "graph/Graph3D.h"

void run_graph(Graph* graph);

#include "graph_main.cpp"

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

    if (mode == "--graph2d") {
        Graph2D graph("sin(x)");
        run_graph(&graph);
        return 0;
    } else if (mode == "--graph3d") {
        Graph3D graph("sin(x*x+y*y)");
        run_graph(&graph);
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
        std::cout << "Invalid mode. Use --graph2d, --graph3d, 2d or 3d." << std::endl;
        return -1;
    }
}