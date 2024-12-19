#include "glad/glad.h"
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
#include <glm/gtc/random.hpp> // for glm::linearRand

#include "setup.h"

#include "vertex.h"
#include "Camera.hpp"
#include "ResourceManager.h"
#include "Transform.hpp"
#include "Util.hpp"
#include "root_directory.h"

#include "mesh/plane.h"
#include "mesh/line_strip.h"
#include "mesh/sphere.h"
#include "mesh/cube.h"
#include "mesh/circle.h"
#include "mesh/quad.h"
#include "mesh/torus.h"
#include "mesh/ring.h"
#include "mesh/arc.h"
#include "math.h"
#include "game/init2d.h"
#include "GameMode.h"
#include "Gui.h"
#include "model.h"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Global variables
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
glm::mat4 projection, view;
float aspect;
bool canPrint = true;
bool isPaused = false;
double lastTime = 0.0;
bool web = false;

GameType gameType = GAME3D;
std::string gameTypeStr = "Default"; // Default game type string

// Array to store 25 cube positions
#define CUBES -1
glm::vec3 cubePositions[CUBES+2];

// Vertex data for the trapezium

std::vector<math::vec3> vertices = {
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

std::vector<math::vec2> uvs = {
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

std::vector<math::vec3> normals = {
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

std::vector<math::vec3> verticest = {
    math::vec3(-0.5f, 0.0f, 0.0f), // A
    math::vec3(0.5f, 0.0f, 0.0f),  // B
    math::vec3(0.0f, 0.5f, 0.0f)   // C
};

std::vector<math::vec2> uvst = {
    math::vec2(0.0f, 0.0f), // A
    math::vec2(1.0f, 0.0f), // B
    math::vec2(0.0f, 1.0f)  // C
};

std::vector<math::vec3> normalst = {
    math::vec3(0.0f, 0.0f, 1.0f), // A
    math::vec3(0.0f, 0.0f, 1.0f), // B
    math::vec3(0.0f, 0.0f, 1.0f)  // C
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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse && !cursor)
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
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse && !cursor)
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
    Gui::Start();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ImGui window for controlling the lighting and material properties
    ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Section for directional light controls
    static bool dirLightEnabled = true;
    static glm::vec3 dirLightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    static glm::vec3 dirLightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    static glm::vec3 dirLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    static glm::vec3 dirLightDirection = glm::vec3(-1.0f, -1.0f, -0.5f);

    if (ImGui::CollapsingHeader("Directional Light"))
    {
        ImGui::Checkbox("Enabled##DirLight", &dirLightEnabled);
        ImGui::SliderFloat3("Direction##DirLight", glm::value_ptr(dirLightDirection), -1.0f, 1.0f);
        ImGui::ColorEdit3("Ambient##DirLight", glm::value_ptr(dirLightAmbient));
        ImGui::ColorEdit3("Diffuse##DirLight", glm::value_ptr(dirLightDiffuse));
        ImGui::ColorEdit3("Specular##DirLight", glm::value_ptr(dirLightSpecular));
    }

    // Section for point light controls
    static bool pointLightEnabled = true;
    static glm::vec3 pointLightPosition = glm::vec3(0.0f, 1.0f, 2.0f);
    static glm::vec3 pointLightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    static glm::vec3 pointLightDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    static glm::vec3 pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    static float pointLightConstant = 1.0f, pointLightLinear = 0.09f, pointLightQuadratic = 0.032f;

    if (ImGui::CollapsingHeader("Point Light"))
    {
        ImGui::Checkbox("Enabled##PointLight", &pointLightEnabled);
        ImGui::SliderFloat3("Position##PointLight", glm::value_ptr(pointLightPosition), -10.0f, 10.0f);
        ImGui::ColorEdit3("Ambient##PointLight", glm::value_ptr(pointLightAmbient));
        ImGui::ColorEdit3("Diffuse##PointLight", glm::value_ptr(pointLightDiffuse));
        ImGui::ColorEdit3("Specular##PointLight", glm::value_ptr(pointLightSpecular));
        ImGui::SliderFloat("Constant##PointLight", &pointLightConstant, 0.1f, 2.0f);
        ImGui::SliderFloat("Linear##PointLight", &pointLightLinear, 0.01f, 1.0f);
        ImGui::SliderFloat("Quadratic##PointLight", &pointLightQuadratic, 0.01f, 0.1f);
    }

    // Section for spot light controls
    static bool spotLightEnabled = true;
    static glm::vec3 spotLightAmbient = glm::vec3(0.65f, 0.64f, 0.54f);
    static glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    static glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    static float spotLightInnerCutoff = 10.5f, spotLightOuterCutoff = 29.5f;
    static float spotLightConstant = 1.0f, spotLightLinear = 0.09f, spotLightQuadratic = 0.032f;

    if (ImGui::CollapsingHeader("Spot Light"))
    {
        ImGui::Checkbox("Enabled##SpotLight", &spotLightEnabled);
        ImGui::ColorEdit3("Ambient##SpotLight", glm::value_ptr(spotLightAmbient));
        ImGui::ColorEdit3("Diffuse##SpotLight", glm::value_ptr(spotLightDiffuse));
        ImGui::ColorEdit3("Specular##SpotLight", glm::value_ptr(spotLightSpecular));
        ImGui::SliderFloat("Inner Cutoff##SpotLight", &spotLightInnerCutoff, 0.0f, 290.0f);
        ImGui::SliderFloat("Outer Cutoff##SpotLight", &spotLightOuterCutoff, 0.0f, 290.0f);
        ImGui::SliderFloat("Constant##SpotLight", &spotLightConstant, -10.1f, 200.0f);
        ImGui::SliderFloat("Linear##SpotLight", &spotLightLinear, 0.001f, 12.0f);
        ImGui::SliderFloat("Quadratic##SpotLight", &spotLightQuadratic, 0.001f, 10.1f);
    }

    // Material controls for the objects
    static glm::vec3 materialDiffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    static glm::vec3 materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
    static float materialShininess = 7.0f;

    ImGui::ColorEdit3("Material Diffuse", glm::value_ptr(materialDiffuse));
    ImGui::ColorEdit3("Material Specular", glm::value_ptr(materialSpecular));
    ImGui::SliderFloat("Shininess", &materialShininess, 1.0f, 128.0f);
    // Add a section for cell shading
    static int cellShade = 4;
    static float edgeThreshold = 0.0f;
    ImGui::SliderInt("Cell Shade", &cellShade, 0, 80); // Use "##" to hide the label in the slider
    ImGui::SliderFloat("Dark Edge", &edgeThreshold, 0.0f,  0.9f); // Use "##" to hide the label in the slider

    // Model transformation controls
    static glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    static glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f);
    static glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    // Initialize cube positions when the app starts
    ImGui::SliderFloat3("Model Scale", glm::value_ptr(modelScale), 0.1f, 10.0f);
    ImGui::SliderFloat3("Model Rotation", glm::value_ptr(modelRotation), -360.0f, 360.0f);

    // Preset selection
    static int selectedPreset = 0; // Index of the selected preset
    std::vector<std::string> presets = {"Default", "Desert", "Factory", "Horror"};

    ImGui::Text("Select Environment Preset");
    if (ImGui::Combo("Preset", &selectedPreset, [](void *data, int idx, const char **out_text)
    {
        const auto& items = *reinterpret_cast<std::vector<std::string>*>(data);
        *out_text = items[idx].c_str();
        return true; }, reinterpret_cast<void *>(&presets), presets.size()))
    {
        // Set the light values based on the selected preset
        if (selectedPreset == 1) // Desert preset
        {
            dirLightDirection = glm::vec3(-0.5f, -1.0f, 0.0f);
            dirLightAmbient = glm::vec3(0.3f, 0.2f, 0.0f);
            dirLightDiffuse = glm::vec3(1.0f, 0.8f, 0.6f);
            dirLightSpecular = glm::vec3(1.0f, 0.9f, 0.8f);

            pointLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
            pointLightAmbient = glm::vec3(0.3f, 0.3f, 0.0f);
            pointLightDiffuse = glm::vec3(1.0f, 0.7f, 0.3f);
            pointLightSpecular = glm::vec3(1.0f, 0.9f, 0.5f);

            spotLightAmbient = glm::vec3(0.3f, 0.2f, 0.0f);
            spotLightDiffuse = glm::vec3(1.0f, 0.8f, 0.4f);
            spotLightSpecular = glm::vec3(1.0f, 0.9f, 0.6f);
        }
        else if (selectedPreset == 2) // Factory preset
        {
            dirLightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
            dirLightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
            dirLightDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
            dirLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

            pointLightPosition = glm::vec3(2.0f, 1.0f, 0.0f);
            pointLightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
            pointLightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

            spotLightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
            spotLightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
            spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else if (selectedPreset == 3) // Horror preset
        {
            dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.2f);
            dirLightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
            dirLightDiffuse = glm::vec3(0.2f, 0.0f, 0.0f);
            dirLightSpecular = glm::vec3(0.5f, 0.0f, 0.0f);

            pointLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
            pointLightAmbient = glm::vec3(1.0f, 0.7f, 0.9f);
            pointLightDiffuse = glm::vec3(0.4f, 0.0f, 0.0f);
            pointLightSpecular = glm::vec3(0.8f, 0.0f, 0.0f);

            spotLightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
            spotLightDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
            spotLightSpecular = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else // Default preset
        {
            // Reset to default values or original values from the UI.
            dirLightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
            dirLightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
            dirLightDiffuse = glm::vec3(1.0f, 0.0f, 0.0f);
            dirLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

            pointLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
            pointLightAmbient = glm::vec3(0.0f, 1.0f, 0.0f);
            pointLightDiffuse = glm::vec3(1.0f, 0.0f, 0.0f);
            pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

            spotLightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
            spotLightDiffuse = glm::vec3(1.0f, 0.0f, 0.0f);
            spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
        }
    }
    ImGui::End();
    ImGui::Begin("Positions/Textures", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGuiTextureSelector();
    editCubePositions();
    ImGui::End();
    // Set up shaders and uniforms
    Shader shader = ResourceManager::GetShader("main");
    shader.Use();

    auto& o = std::cout;
    shader.SetFloat("material.shininess", materialShininess);
    o << "Shininess: " << materialShininess << std::endl;
    // Lighting uniforms
    if (dirLightEnabled)
    {
        shader.SetVector3f("dirLight.direction", dirLightDirection);
        o << "DirLight Direction: " << dirLightDirection.x << ", " << dirLightDirection.y << ", " << dirLightDirection.z << std::endl;
        shader.SetVector3f("dirLight.ambient", dirLightAmbient);
        o << "DirLight Ambient: " << dirLightAmbient.x << ", " << dirLightAmbient.y << ", " << dirLightAmbient.z << std::endl;
        shader.SetVector3f("dirLight.diffuse", dirLightDiffuse);
        o << "DirLight Diffuse: " << dirLightDiffuse.x << ", " << dirLightDiffuse.y << ", " << dirLightDiffuse.z << std::endl;
        shader.SetVector3f("dirLight.specular", dirLightSpecular);
        o << "DirLight Specular: " << dirLightSpecular.x << ", " << dirLightSpecular.y << ", " << dirLightSpecular.z << std::endl;
    }

    if (spotLightEnabled)
    {
        shader.SetVector3f("spotLight.position", camera.Position);
        o << "SpotLight Position: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
        shader.SetVector3f("spotLight.direction", camera.Front);
        o << "SpotLight Direction: " << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << std::endl;
        shader.SetVector3f("spotLight.ambient", spotLightAmbient);
        o << "SpotLight Ambient: " << spotLightAmbient.x << ", " << spotLightAmbient.y << ", " << spotLightAmbient.z << std::endl;
        shader.SetVector3f("spotLight.diffuse", spotLightDiffuse);
        o << "SpotLight Diffuse: " << spotLightDiffuse.x << ", " << spotLightDiffuse.y << ", " << spotLightDiffuse.z << std::endl;
        shader.SetVector3f("spotLight.specular", spotLightSpecular);
        o << "SpotLight Specular: " << spotLightSpecular.x << ", " << spotLightSpecular.y << ", " << spotLightSpecular.z << std::endl;
        shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(spotLightInnerCutoff)));
        o << "SpotLight Inner Cutoff: " << spotLightInnerCutoff << std::endl;
        shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLightOuterCutoff)));
        o << "SpotLight Outer Cutoff: " << spotLightOuterCutoff << std::endl;

        shader.SetFloat("spotLight.constant", spotLightConstant);
        o << "SpotLight Constant: " << spotLightConstant << std::endl;
        shader.SetFloat("spotLight.linear", spotLightLinear);   
        o << "SpotLight Linear: " << spotLightLinear << std::endl;
        shader.SetFloat("spotLight.quadratic", spotLightQuadratic);
        o << "SpotLight Quadratic: " << spotLightQuadratic << std::endl;
    }
    if (input)
    {
        view = camera.GetViewMatrix();
    }
    else
    {
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    }
    projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
    shader.SetInteger("cellShade", cellShade);
    o << "Cell Shade: " << cellShade << std::endl;
    shader.SetFloat("edgeThreshold", edgeThreshold);
    o << "Edge Threshold: " << edgeThreshold << std::endl;
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);

    float radius = 0.8f;
    float angularSpeed = 0.4f;

    glm::vec3 lightColor = glm::vec3(1.0f);

    Shader light = ResourceManager::GetShader("light");
    light.Use();
    light.SetMatrix4("view", view);
    light.SetMatrix4("projection", projection);
    light.SetVector3f("lightColor", lightColor);
    o << "Light Color: " << lightColor.x << ", " << lightColor.y << ", " << lightColor.z << std::endl;
    for (unsigned int i = 0; i < 4; i++)
    {
        light.Use();
        glm::mat4 lightModel = transform(pointLightPositions[i], 0.2f, 0.0f);
        Texture2D *lightTex = new Texture2D[1]{
            ResourceManager::GetTexture2D(selectedLightTexture)};
        draw(light, lightTex, 1, meshes[3], lightModel);
        delete[] lightTex;
        logger.log("LIGHT-MODEL", lightModel);
        shader.Use();

        if (pointLightEnabled)
        {
            std::ostringstream oss;
            o << i << std::endl;
            // Construct the string using std::ostringstream
            oss << "pointLights[" << i << "].position";
            shader.SetVector3f(oss.str().c_str(), pointLightPositions[i]);
            o << "PointLight Position: " << pointLightPositions[i].x << ", " << pointLightPositions[i].y << ", " << pointLightPositions[i].z << std::endl;
            oss.str(""); // Clear the string stream
            oss << "pointLights[" << i << "].ambient";
            shader.SetVector3f(oss.str().c_str(), pointLightAmbient);
            o << "PointLight Ambient: " << pointLightAmbient.x << ", " << pointLightAmbient.y << ", " << pointLightAmbient.z << std::endl;
            oss.str("");
            oss << "pointLights[" << i << "].diffuse";
            shader.SetVector3f(oss.str().c_str(), pointLightDiffuse);

            oss.str("");
            oss << "pointLights[" << i << "].specular";
            shader.SetVector3f(oss.str().c_str(), pointLightSpecular);
            o << "PointLight Specular: " << pointLightSpecular.x << ", " << pointLightSpecular.y << ", " << pointLightSpecular.z << std::endl;
            oss.str("");
            oss << "pointLights[" << i << "].constant";
            shader.SetFloat(oss.str().c_str(), pointLightConstant);
            o << "PointLight Constant: " << pointLightConstant << std::endl;
            oss.str("");
            oss << "pointLights[" << i << "].linear";
            shader.SetFloat(oss.str().c_str(), pointLightLinear);
            o << "PointLight Linear: " << pointLightLinear << std::endl;
            oss.str("");
            oss << "pointLights[" << i << "].quadratic";
            shader.SetFloat(oss.str().c_str(), pointLightQuadratic);
            o << "PointLight Quadratic: " << pointLightQuadratic << std::endl;
        }
    }
    shader.Use();
    float stepsValue = 7.5f;
    o << "Steps: " << stepsValue << std::endl;
    shader.SetFloat("steps", stepsValue);

    shader.SetFloat("mixColor", 0.0f);

    for (unsigned int i = 0; i < CUBES; i++) {
        if(CUBES < 0){ break; }
        int c = i % 2 != 0 ? i % 3 == 0 ? 4 : 0 : 2;
        int t = i;
        float time2 = glfwGetTime();
        glm::vec3 rot = glm::vec3(i % 3 == 0 || i < 4 ? time2 : 0.0f, i % 2 == 0 ? 0.3f * time2 : 0.0f, i == 5 || i == 7 ? time2 : 0.0f);
        glm::mat4 model = transform(cubePositions[i], modelScale, rot * modelRotation);

        // Draw meshes
        Texture2D *textures2 = ResourceManager::GetTexture(selectedAdditionalTexture);
//                    ResourceManager::GetTexture2DByIndex(t);
        Texture2D *diff = ResourceManager::GetTexture(selectedDiffuseTexture);
        Texture2D *spec = ResourceManager::GetTexture(selectedSpecularTexture);
        draw(shader, textures2, 1, meshes[c], model, diff, spec);
        
        // Print transforms if needed
        logger.log("MODEL", model);
        logger.log("VIEW", view);
        logger.log("PROJECTION", projection);
    }
    if(cartesian){
        Texture2D *textures = ResourceManager::GetTexture("white");
        Texture2D *rect = ResourceManager::GetTexture("rect");

        // Texture2D *diff = ResourceManager::GetTexture("diffuse");
        // Texture2D *spec = ResourceManager::GetTexture("specular");
        glm::mat4 cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.01f, 0.01f, 500.0f), 0.0f);
        shader.SetMatrix4("model", cartesian);
        draw(shader, textures, 1, meshes[2], cartesian, textures, textures);
        cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.01f, 500.0f, 0.01f), 0.0f);
        shader.SetMatrix4("model", cartesian);
        draw(shader, textures, 1, meshes[2], cartesian, textures, textures);
        cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(500.0f, 0.01f, 0.01f), 0.0f);
        shader.SetMatrix4("model", cartesian);
        draw(shader, textures, 1, meshes[2], cartesian, textures, textures);
    }
    /*float st = sin(glfwGetTime());
    Texture2D *texture = ResourceManager::GetTexture("sky");
    glm::vec3 pos = glm::vec3(4.0f, -50.0f, -50.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, st * 45.0f);
    glm::mat4 cube = transform(pos, scale, rot);
    // Render the object
    draw(shader, texture, 1, meshes[2], cube, textures, textures);
    glCheckError(__FILE__, __LINE__);
    glm::mat4 tmat = glm::mat4(
        0.3f, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        5, 0, 0, 1);
    glm::translate(tmat, glm::vec3(0.0f, -220.0f, 700.0f));
    draw(shader, rect, 1, meshes[5], tmat, textures, textures);
    // Draw the base triangle
    glm::mat4 baseTransform = glm::mat4(1.0f); 
    glm::translate(baseTransform, glm::vec3(450.0f, -50.0f, 0.0f));
    draw(shader, rect, 1, meshes[5], baseTransform, textures, textures); // Draw base triangle
    // Draw the apex
    glm::mat4 apexTransform = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, -100.0f, 0.0f)); // Position apex
    draw(shader, rect, 1, meshes[5], apexTransform, textures, textures);                    // Draw apex
    */
    Shader model = ResourceManager::GetShader("model");
    model.Use();
    glm::mat4 modelTransform = transform(
        modelPosition, modelScale / 100.0f, modelRotation
    );
    model.SetMatrix4("model", modelTransform);
    model.SetMatrix4("view", view);
    model.SetMatrix4("projection", projection);
    model.SetVector3f("pointLightPos", pointLightPositions[0]);
    model.SetVector3f("viewPos", camera.Position);
    model.SetVector3f("spotLightPos", camera.Position);
    model.SetVector3f("pointLightColor", pointLightAmbient);
    model.SetVector3f("spotLightColor", spotLightAmbient);
    model.SetFloat("spotLightCutoff", spotLightInnerCutoff);
    model.SetFloat("spotLightOuterCutoff", spotLightOuterCutoff);
    model.SetFloat("shininess", materialShininess);

    meshes[6]->Draw(model);

    glBindVertexArray(0);
    canPrint = false;

    Gui::Render();
    glfwSwapBuffers(window);
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
void processInput(GLFWwindow *window)
{
    glfwPollEvents();

    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
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
        float cameraSpeed = 2.5f * deltaTime;

        static bool alt = false;
        if (toggleKey(GLFW_KEY_LEFT_ALT, alt))
        {
            toggleCursor(window, cursor);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
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
    std::string root;
    if (!web)
    {
        root = std::string(logl_root);
    }
    else
    {
        root = "";
    }
    ResourceManager::root = root;
    logger.setDir(root);
    if (argc > 1)
    {
        toLowerCase(argv[1]);

        // Check the first argument for game type
        if (includes(argv[1], "2"))
        {
            gameType = GAME2D;
            gameTypeStr = "2D";
        }
        else if (includes(argv[1], "3"))
        {
            gameType = GAME3D;
            gameTypeStr = "3D";
        }
        else
        {
            std::cout << "Unknown game type. Defaulting to 2D." << std::endl;
        }
    }
    if (argc > 2)
    {
        toLowerCase(argv[2]);
        gameTypeStr = argv[2];
    }

    switch (gameType)
    {
    case GAME2D:
        game2d(argc, argv, gameTypeStr);
        break;
    case GAME3D:
        game3d(argc, argv, gameTypeStr);
        break;
    default:
        game2d(argc, argv, gameTypeStr); // Default to 2D
        break;
    }

    return 0;
}
int game3d(int argc, char *argv[], std::string type)
{
    // Initialize GLFW
    // return vectortest();
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context of the window the current one
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Initialization code
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    toggleCursor(window, cursor);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Gui::Init(window);

    Shader shader = ResourceManager::LoadShader("vertex.glsl", "fragment.glsl", "main");
    Shader light = ResourceManager::LoadShader("light/vertex.glsl", "light/fragment.glsl", "light");
    Shader modelS = ResourceManager::LoadShader("model/vertex.glsl", "model/fragment.glsl", "model");

    std::cout << shader.ID << "\n";
    std::cout << light.ID << "\n";
    shader.Use();

    ResourceManager::LoadAllTexturesFromDirectory();
    //std::string modelPath = ResourceManager::GetModelPath("backpack/obj/backpack.obj"); //"backpack";
    //std::string modelPath = ResourceManager::GetModelPath("triangle-prism.glb"); //"backpack";
    //std::string modelPath = ResourceManager::GetModelPath("n64/Toxic Can/7398.obj"); //"backpack";
    std::string modelPath = ResourceManager::GetModelPath("n64/Starry Sky/sky.obj"); //"backpack";
    modelS.Use();
    m3D::Model model(modelPath.data());
    shader.Use();
    // Set up VAO, VBO, and EBO
    const float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
    float startAngleRad = 0.0f * DEG_TO_RAD;
    float endAngleRad = 360.0f * DEG_TO_RAD;
    
    // Cell::Cube mesh;
    Engine::Plane mesh(20, 20);
    // std::cout << Cell::HUEtoRGB(0.5f) << std::endl;
    Engine::Mesh trap(vertices, indices, uvs, normals);
    math::vec3 it = math::vec3();
    Engine::Mesh triang(verticest, indicest, uvst, normalst);
    triang.Finalize();
    Engine::Cube cube;
    trap.Finalize();
    light.Use();
    Engine::Cube lightCube;
    shader.Use();
    Engine::Sphere sphere(30, 30);

    std::vector<VO::VAO *> meshes = {
        &mesh,
        &trap,
        &cube,
        &lightCube,
        &sphere,
        &triang,
        &model
    };

    // Set up transformations
    aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    //    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    /*view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    */
    double nextRenderTime = 0.0; // Store the time when the next render should occur
    // Main loop
    initCubePositions();
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);
        if (!isPaused)
        {
            // Update your program state here
            renderScene(window, meshes);
        }
    }
    Gui::Clean();
    glfwDestroyWindow(window);
    // Terminate GLFW
    glfwTerminate();

    return 0;
}
