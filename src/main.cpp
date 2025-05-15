#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <glm/gtc/random.hpp>
#include "util/Log.h"
#include "root.h"
#include "util/common.h"
#include "game/Game.h"

#include "setup.h"
#include "types.h"
#include "render/Vertex.h"
#include "Camera.hpp"
#include "asset/ResourceManager.h"
#include "Transform.hpp"
#include "util/Util.h"
#include "math.h"
#include "game/init2d.h"
#include "GameMode.h"
#include "ui/Gui.h"
#include "render/Model.h"

// Add missing declarations
extern Log logger;
std::string logl_root = ".";

// Function declarations - make them static to avoid conflicts
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
static void processInput(GLFWwindow *window);
// These functions are defined in their respective files
extern int game2d(int argc, char *argv[], const std::string& type);
extern int game3d(int argc, char *argv[], const std::string& type);
extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Global variables
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
float aspect;
bool canPrint = true;
bool isPaused = false;
double lastTime = 0.0;
bool web = false;
bool debug = false;

GameType gameType = GAME2D;
std::string gameTypeStr = "Default";

// Add key state tracking from game jam branch
struct {
    bool current[1024] = {false};
    bool previous[1024] = {false};
} KeyState;

// Keep the resolution helper from game jam branch
struct Resolution {
    int width;
    int height;
    float aspectRatio;
};

Resolution getOptimalResolution() {
    Resolution res;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        return {1280, 720, 16.0f/9.0f};
    }

    int count;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    if (!modes || count == 0) {
        return {1280, 720, 16.0f/9.0f};
    }

    const GLFWvidmode* bestMode = &modes[0];
    for (int i = 1; i < count; i++) {
        if (modes[i].width * modes[i].height > bestMode->width * bestMode->height) {
            bestMode = &modes[i];
        }
    }
    
    res.width = bestMode->width;
    res.height = bestMode->height;
    res.aspectRatio = static_cast<float>(res.width) / static_cast<float>(res.height);
    
    return res;
}

// Array to store 25 cube positions
#define CUBES -1
glm::vec3 cubePositions[CUBES+2];

// Vertex data for the trapezium

std::vector<glm::vec3> vertices = {
    // Front face
    {-0.5f, 0.5f, 0.5f},  // Top left
    {0.5f, 0.5f, 0.5f},   // Top right
    {0.3f, -0.5f, 0.5f},  // Bottom right
    {-0.3f, -0.5f, 0.5f}, // Bottom left

    // Back face
    {-0.5f, 0.5f, -0.5f},  // Top left
    {0.5f, 0.5f, -0.5f},   // Top right
    {0.3f, -0.5f, -0.5f},  // Bottom right
    {-0.3f, -0.5f, -0.5f}, // Bottom left
};

std::vector<glm::vec2> uvs = {
    // Front face
    {0.0f, 1.0f}, // Top left
    {1.0f, 1.0f}, // Top right
    {0.6f, 0.0f}, // Bottom right
    {0.4f, 0.0f}, // Bottom left

    // Back face
    {0.0f, 1.0f}, // Top left
    {1.0f, 1.0f}, // Top right
    {0.6f, 0.0f}, // Bottom right
    {0.4f, 0.0f}  // Bottom left
};

std::vector<uint32_t> indices = {
    // Front face
    0, 1, 2,
    0, 2, 3,

    // Right face
    1, 5, 6,
    1, 6, 2,

    // Back face
    5, 4, 7,
    5, 7, 6,

    // Left face
    4, 0, 3,
    4, 3, 7,

    // Top face
    4, 5, 1,
    4, 1, 0,

    // Bottom face
    3, 2, 6,
    3, 6, 7};

std::vector<glm::vec3> normals = {
    // Front face
    {0.0f, 0.0f, 1.0f}, // Top left
    {0.0f, 0.0f, 1.0f}, // Top right
    {0.0f, 0.0f, 1.0f}, // Bottom right
    {0.0f, 0.0f, 1.0f}, // Bottom left

    // Back face
    {0.0f, 0.0f, -1.0f}, // Top left
    {0.0f, 0.0f, -1.0f}, // Top right
    {0.0f, 0.0f, -1.0f}, // Bottom right
    {0.0f, 0.0f, -1.0f}  // Bottom left
};

// Vertices

// Define the vertices, UVs, normals, and indices

std::vector<glm::vec3> verticest = {
    glm::vec3(-0.5f, 0.0f, 0.0f), // A
    glm::vec3(0.5f, 0.0f, 0.0f),  // B
    glm::vec3(0.0f, 0.5f, 0.0f)   // C
};

std::vector<glm::vec2> uvst = {
    glm::vec2(0.0f, 0.0f), // A
    glm::vec2(1.0f, 0.0f), // B
    glm::vec2(0.0f, 1.0f)  // C
};

std::vector<glm::vec3> normalst = {
    glm::vec3(0.0f, 0.0f, 1.0f), // A
    glm::vec3(0.0f, 0.0f, 1.0f), // B
    glm::vec3(0.0f, 0.0f, 1.0f)  // C
};

std::vector<unsigned int> indicest = {
    0, 1, 2 // Triangle
};

// positions of the point lights

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)};

VO::VAO vao;

VO::VBO vbo;

VO::EBO ebo;

bool input = true;

// camera

// Use extern for camera since it's defined in init3d.cpp
extern Camera camera;

bool firstMouse = true;

bool cursor = true;

// timing

float deltaTime = 0.0f; // time between current frame and last frame

float lastFrame = 0.0f;

float lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;

bool cartesian = false;
static bool keyWasPressed[GLFW_KEY_LAST + 1]; // Array to track key press states

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Variables for storing selected textures
static std::string selectedDiffuseTexture = "diffuse";
static std::string selectedSpecularTexture = "specular";
static std::string selectedAdditionalTexture = "container";
static std::string selectedLightTexture = "light";

// ImGui function to create a dropdown for selecting textures
void ImGuiTextureSelector() {
    ImGui::Begin("Texture Selector");

    // Array to hold texture names for the dropdown
    std::vector<const char*> textureNames;
    for (const auto& pair : ResourceManager::Textures2D) {
        textureNames.push_back(pair.first.c_str());
    }

    // Dropdown for selecting diffuse texture
    static int currentDiffuseIndex = 0;
    if (ImGui::Combo("Diffuse Texture", &currentDiffuseIndex, textureNames.data(), textureNames.size())) {
        selectedDiffuseTexture = textureNames[currentDiffuseIndex];
    }

    // Dropdown for selecting specular texture
    static int currentSpecularIndex = 0;
    if (ImGui::Combo("Specular Texture", &currentSpecularIndex, textureNames.data(), textureNames.size())) {
        selectedSpecularTexture = textureNames[currentSpecularIndex];
    }

    // Dropdown for selecting additional texture
    static int currentAdditionalIndex = 0;
    if (ImGui::Combo("Texture", &currentAdditionalIndex, textureNames.data(), textureNames.size())) {
        selectedAdditionalTexture = textureNames[currentAdditionalIndex];
    }
    static int currentLightIndex = 0;
    if (ImGui::Combo("Light Texture", &currentLightIndex, textureNames.data(), textureNames.size())) {
        selectedLightTexture = textureNames[currentLightIndex];
    }

    ImGui::End();
}
// Initialize cube positions
void initCubePositions() {
    float lim = 35.0f;
    for (int i = 0; i < CUBES; ++i) {
        cubePositions[i] = glm::linearRand(glm::vec3(-lim, -lim, -lim), glm::vec3(lim, lim, lim));
    }
}

// Function to edit cube positions in ImGui
void editCubePositions() {
    if(CUBES < 0) return;
    ImGui::Begin("Cube Positions Editor");

    for (int i = 0; i < CUBES; ++i) {
        std::string label = "Position " + std::to_string(i);
        ImGui::SliderFloat3(label.c_str(), glm::value_ptr(cubePositions[i]), -15.0f, 15.0f);
    }

    ImGui::End();
}

// Function to toggle cursor visibility


void toggleCursor(GLFWwindow *window, bool &cursorEnabled)
{
    cursorEnabled = !cursorEnabled; // Toggle the cursor state
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called

// ----------------------------------------------------------------------
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // Get the IO structure and check if ImGui wants to capture mouse input
    bool wantCaptureMouse = ImGui::GetIO().WantCaptureMouse;
    if (!wantCaptureMouse && !cursor)
    {
        // Handle scroll input here if ImGui is not capturing it
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
    else
    {
        // Pass the scroll event to ImGui
        ImGui::GetIO().MouseWheel += (float)yoffset; // Adjust as needed for ImGui
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // Get the IO structure and check if ImGui wants to capture mouse input
    bool wantCaptureMouse = ImGui::GetIO().WantCaptureMouse;
    if (!wantCaptureMouse && !cursor)
    {
        // Handle mouse input logic here
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}
void renderScene(GLFWwindow *window, std::vector<VO::VAO *> &meshes)
{
}
bool toggleKey(int key, bool &toggleState)
{
    bool keyCurrentlyPressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;

    if (!keyWasPressed[key] && keyCurrentlyPressed)
    {
        // Key was just pressed
        toggleState = !toggleState; // Toggle the state
        keyWasPressed[key] = true;  // Set the flag for this key
        return true;                // Indicate that the state was toggled
    }
    else if (keyWasPressed[key] && !keyCurrentlyPressed)
    {
        // Key was just released
        keyWasPressed[key] = false; // Reset the flag for this key
    }

    return false; // Indicate that the state was not toggled
}
static void processInput(GLFWwindow *window)
{
    glfwPollEvents();

    // Get the IO structure and check if ImGui wants to capture keyboard input
    bool wantCaptureKeyboard = ImGui::GetIO().WantCaptureKeyboard;
    if (!wantCaptureKeyboard)
    {
        // Handle keyboard input logic here
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, 1);
        static bool graveAccentPressed = false; // Flag to track if the key is pressed

        if (!graveAccentPressed && glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
        {
            // Key was just pressed
            isPaused = !isPaused;
            if (isPaused)
            {
                lastTime = glfwGetTime();
                std::cout << "Program paused. " << lastTime << std::endl;
            }
            else
            {
                std::cout << "Program resumed." << std::endl;
            }
            graveAccentPressed = true; // Set the flag
        }
        else if (graveAccentPressed && glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) != GLFW_PRESS)
        {
            // Key was just released
            graveAccentPressed = false; // Reset the flag
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            canPrint = true;
        }
        static bool isLineMode = false; // Track the current polygon mode

        static bool spacePressed = false;
        if (toggleKey(GLFW_KEY_SPACE, spacePressed))
        {
            // Toggle polygon mode only once after the space key is pressed
            isLineMode = !isLineMode;
            glPolygonMode(GL_FRONT_AND_BACK, isLineMode ? GL_LINE : GL_FILL);
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        static bool alt = false;
        if (toggleKey(GLFW_KEY_LEFT_ALT, alt))
        {
            toggleCursor(window, cursor);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::CAMERA_FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::CAMERA_BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::CAMERA_LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::CAMERA_RIGHT, deltaTime);
    }
}
// Function to convert a string to lowercase
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}
int main(int argc, char *argv[])
{
    // Auto-start in 3D mode if no arguments are provided
    if (argc < 2) {
        std::cout << "No mode specified, auto-starting in 3D mode..." << std::endl;
        return game3d(argc, argv, "Default");
    }

    std::string mode = argv[1];
    std::string type = argc > 2 ? argv[2] : "Default";

    if (mode == "2d") {
        return game2d(argc, argv, type);
    } else if (mode == "3d") {
        return game3d(argc, argv, type);
    } else {
        std::cout << "Invalid mode. Use 2d or 3d." << std::endl;
        return -1;
    }
}
