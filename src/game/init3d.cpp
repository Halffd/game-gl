#include "init3d.h"
#include "../ui/Gui.h"
#include <setup.h>
#include "../Transform.hpp"
#include "../render/Model.h"
#include "../include/Camera.hpp"  // Include the 3D camera header

// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH;
const unsigned SCREEN_HEIGHT = HEIGHT;

// Create a Camera instance from Camera.hpp
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
extern bool firstMouse;
extern float lastX;
extern float lastY;
extern float deltaTime;
extern float lastFrame;

// Forward declare static functions
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);

int game3d(int argc, char *argv[], const std::string& type) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Game", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize ImGui
    Gui::Init(window);

    // Load shaders and textures
    ResourceManager::LoadShader("shaders/3d.vs", "shaders/3d.fs", nullptr, "3d");
    ResourceManager::LoadTexture2D("textures/container.jpg", "container", false);

    // Get shader
    Shader shader = ResourceManager::GetShader("3d");
    shader.Use();

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // Pass projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        shader.SetMatrix4("projection", projection);

        // Camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.SetMatrix4("view", view);

        // Render game objects here
        // TODO: Add your 3D game rendering code

        // Start ImGui frame
        Gui::Start();
        
        // Render ImGui
        Gui::Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ResourceManager::Clear();
    Gui::Clean();
    glfwTerminate();
    return 0;
}

static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_RIGHT, deltaTime);
}

// Add the missing framebufferSizeCallback function
static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} 