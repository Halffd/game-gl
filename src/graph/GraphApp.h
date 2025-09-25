#pragma once

#include "graph/Graph.h"
#include "graph/Graph2D.h"
#include "graph/Graph3D.h"
#include <memory>
#include <GLFW/glfw3.h>

class GraphApp {
public:
    GraphApp();
    ~GraphApp();

    void run();

private:
    void init();
    void update(float deltaTime);
    void render();
    void processInput();

    GLFWwindow* window;
    std::unique_ptr<Graph> currentGraph;
    float lastFrame;
    bool showGraphSelector;
};
