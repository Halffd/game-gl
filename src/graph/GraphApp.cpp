#include "graph/GraphApp.h"
#include "ui/Gui.h"
#include <glad/glad.h>
#include <iostream>
#include "graph/VectorField.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GraphApp::GraphApp() : window(nullptr), lastFrame(0.0f), showGraphSelector(true) {
    init();
}

GraphApp::~GraphApp() {
    Gui::Clean();
    glfwTerminate();
}

void GraphApp::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GraphApp", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    Gui::Init(window);

    currentGraph = std::make_unique<Graph2D>("sin(x)");
}

void GraphApp::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        update(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Gui::Start();
        render();
        Gui::Render();

        glfwSwapBuffers(window);
    }
}

void GraphApp::processInput() {
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void GraphApp::update(float deltaTime) {
    if (currentGraph) {
        currentGraph->update(window, deltaTime);
    }
}

void GraphApp::render() {

    ImGui::Begin("Graph Selector");
    if (ImGui::Button("2D Graph")) {
        currentGraph = std::make_unique<Graph2D>("sin(x)");
    }
    ImGui::SameLine();
    if (ImGui::Button("3D Graph")) {
        currentGraph = std::make_unique<Graph3D>("sin(x*x+y*y)");
    }
    ImGui::SameLine();
    if (ImGui::Button("Vector Field")) {
        currentGraph = std::make_unique<VectorField>("Vector Field");
    }
    ImGui::End();

    if (currentGraph) {
        currentGraph->render();
    }

    Gui::Render();
}
