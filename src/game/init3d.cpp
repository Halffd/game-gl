#include "init3d.h"
#include "../ui/Gui.h"
#include <setup.h>
#include "../Transform.hpp"
#include "../render/Model.h"
#include "../include/Camera.hpp"  // Include the 3D camera header
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <fstream>
#include <unistd.h> // For getcwd
#include <random>
#include <ctime>
#include "../render/Scene.h"
#include "../render/ModelObject.h"
#include "../render/primitives/PrimitiveShapes.h" // Updated include path
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <cmath>
#define TESTING 1
// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH;
const unsigned SCREEN_HEIGHT = HEIGHT;

// Create a Camera instance from Camera.hpp - position it higher and further back
Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));
extern bool firstMouse;
extern float lastX;
extern float lastY;
extern float deltaTime;
extern float lastFrame;

// Add cursor control variables
bool cursorEnabled = true;
std::map<int, bool> keyWasPressed;

glm::mat4 projection = glm::mat4(1.0f);

// Model variables
std::vector<m3D::Model *> models;
std::vector<glm::vec3> modelPositions;
std::vector<glm::vec3> modelRotations;
std::vector<glm::vec3> modelScales;
bool showModelsWindow = false;
int selectedModel = 0;

// Scene management
Scene scene;
std::vector<std::string> modelNames;

// Light variables - make them static to avoid linker errors
// Directional light
struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

// Point light
struct PointLight {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

// Spot light
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

// Light instances
static DirLight dirLight = {
    glm::vec3(-0.5f, -1.0f, -0.3f), // direction - adjusted for better angle
    glm::vec3(0.63f, 0.63f, 0.63f), // ambient - increased from 0.4 to 0.5
    glm::vec3(1.0f, 1.0f, 1.0f), // diffuse
    glm::vec3(1.0f, 1.0f, 1.0f), // specular
    true // enabled
};

// Main point light (separate from random lights)
static PointLight pointLight = {
    glm::vec3(0.0f, 15.0f, 0.0f), // position - moved to center and higher
    1.0f, // constant
    0.07f, // linear - slightly reduced for less attenuation
    0.017f, // quadratic - slightly reduced for less attenuation
    glm::vec3(0.3f, 0.3f, 0.3f), // ambient - increased from 0.2 to 0.3
    glm::vec3(1.0f, 1.0f, 1.0f), // diffuse
    glm::vec3(1.0f, 1.0f, 1.0f), // specular
    true // enabled
};

// Update the spotlight to be a proper flashlight
static SpotLight spotLight = {
    glm::vec3(0.0f, 0.0f, 0.0f), // position - will be updated with camera position
    glm::vec3(0.0f, 0.0f, -1.0f), // direction - will be updated with camera direction
    glm::cos(glm::radians(12.5f)), // cutOff - inner cone (12.5 degrees)
    glm::cos(glm::radians(17.5f)), // outerCutOff - outer cone (17.5 degrees)
    1.0f, // constant
    0.09f, // linear
    0.032f, // quadratic
    glm::vec3(0.0f, 0.0f, 0.0f), // ambient - keep dark
    glm::vec3(1.0f, 1.0f, 1.0f), // diffuse - bright white
    glm::vec3(1.0f, 1.0f, 1.0f), // specular - bright white
    true // enabled
};

// Add random point lights
const int MAX_POINT_LIGHTS = 20;
std::vector<PointLight> randomPointLights;
float MIN_DISTANCE_BETWEEN_LIGHTS = 15.0f; // Increased minimum distance between lights
bool useRandomPointLights = true; // Enable random point lights by default

// Material settings
static float shininess = 32.0f;
static bool useNormalMap = true;
static bool useSpecularMap = true;
static bool useDetailMap = false;
static bool useScatterMap = false;

// Ground plane variables
unsigned int groundVAO = 0;
unsigned int groundVBO = 0;
unsigned int groundTexture = 0;
unsigned int groundNormalTexture = 0;

// Add these variables after the material settings
static float pointLightBrightness = 0.6f; // Increased to compensate for improved attenuation
static float dirLightBrightness = 0.5f; // Keep the same
static float spotLightBrightness = 0.7f; // Increased for better flashlight effect

// Add variables for primitive shapes
std::vector<std::shared_ptr<m3D::PrimitiveShape> > primitiveShapes;
std::vector<glm::vec3> rotationSpeeds; // Store rotation speeds for each primitive

// Dynamic shapes with mathematical transformations
std::vector<std::shared_ptr<m3D::PrimitiveShape> > dynamicShapes;
std::vector<std::shared_ptr<m3D::DynamicTransform> > dynamicTransforms;
bool useDynamicShapes = false;
bool showDynamicShapesWindow = false;

// Global variables for keyboard controls
bool showCartesianPlane = false; // Hidden by default
bool showTriangleContours = false;
bool runMode = true; // Default to run mode
float baseMovementSpeed = 2.5f; // Default movement speed

// Forward declare static functions
static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

static void processInput(GLFWwindow *window);

static void renderModelsWindow();

static void renderLightingWindow();

static void renderDynamicShapesWindow();

static void setupGround();

static void renderGround(Shader &shader);

static bool toggleKey(int key, bool &toggleState);

static void toggleCursor(GLFWwindow *window);

static void renderScene(Shader &shader);

static void setLightingUniforms(Shader &shader);

// Function to generate random point lights
void generateRandomPointLights() {
    // Clear existing random lights
    randomPointLights.clear();

    // If random lights are disabled, return early
    if (!useRandomPointLights) {
        return;
    }

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());

    // Spherical distribution parameters
    std::uniform_real_distribution<float> radiusDist(10.0f, 50.0f); // Distance from center - increased range
    std::uniform_real_distribution<float> thetaDist(0.0f, 2.0f * M_PI); // Horizontal angle (0 to 2π)
    std::uniform_real_distribution<float> phiDist(0.0f, M_PI); // Vertical angle (0 to π)

    // Color distribution
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);

    // Attenuation distributions
    std::uniform_real_distribution<float> constantDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> linearDist(0.01f, 0.1f);
    std::uniform_real_distribution<float> quadraticDist(0.001f, 0.01f);

    // Number of lights to generate - increased to 35
    const int NUM_LIGHTS = 35;

    // Reduce minimum distance to allow more lights
    float tempMinDistance = MIN_DISTANCE_BETWEEN_LIGHTS;
    MIN_DISTANCE_BETWEEN_LIGHTS = 28.0f; // Temporarily reduce minimum distance

    // Generate random lights
    for (int i = 0; i < NUM_LIGHTS; i++) {
        // Generate a random position using spherical coordinates
        float radius = radiusDist(gen);
        float theta = thetaDist(gen);
        float phi = phiDist(gen);

        // Convert spherical to Cartesian coordinates
        float x = radius * sin(phi) * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float z = radius * cos(phi);

        glm::vec3 position(x, y, z);

        // Check if this position is far enough from existing lights
        bool validPosition = true;
        for (const auto &light: randomPointLights) {
            float distance = glm::length(position - light.position);
            if (distance < MIN_DISTANCE_BETWEEN_LIGHTS) {
                validPosition = false;
                break;
            }
        }

        // If position is not valid, try again (but limit retries to avoid infinite loop)
        if (!validPosition) {
            if (i > 0 && randomPointLights.size() >= NUM_LIGHTS * 0.8) {
                // If we've already got 80% of the desired lights, just continue
                continue;
            }
            i--; // Retry this iteration
            continue;
        }

        // Generate a random color
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));

        // Create the light
        PointLight light;
        light.position = position;
        light.ambient = color * 0.1f;
        light.diffuse = color;
        light.specular = color;
        light.constant = constantDist(gen);
        light.linear = linearDist(gen);
        light.quadratic = quadraticDist(gen);
        light.enabled = true;

        // Add to the list
        randomPointLights.push_back(light);
    }

    // Restore original minimum distance
    MIN_DISTANCE_BETWEEN_LIGHTS = tempMinDistance;

    std::cout << "Generated " << randomPointLights.size() << " random point lights in a spherical distribution" <<
            std::endl;
}

// Function to generate primitive shapes
void generatePrimitiveShapes() {
    // Call the refactored function from the PrimitiveShapes.h file
    m3D::generatePrimitiveShapes(scene, primitiveShapes, rotationSpeeds);
}

// Function to update primitive rotations
void updatePrimitiveRotations(float deltaTime) {
    for (size_t i = 0; i < primitiveShapes.size(); i++) {
        if (i < rotationSpeeds.size() && primitiveShapes[i]) {
            // Update rotation based on rotation speed and delta time
            primitiveShapes[i]->rotation.x += rotationSpeeds[i].x * deltaTime;
            primitiveShapes[i]->rotation.y += rotationSpeeds[i].y * deltaTime;
            primitiveShapes[i]->rotation.z += rotationSpeeds[i].z * deltaTime;

            // Keep rotations in the range [0, 360]
            primitiveShapes[i]->rotation.x = fmod(primitiveShapes[i]->rotation.x, 360.0f);
            primitiveShapes[i]->rotation.y = fmod(primitiveShapes[i]->rotation.y, 360.0f);
            primitiveShapes[i]->rotation.z = fmod(primitiveShapes[i]->rotation.z, 360.0f);
        }
    }
}

// Function to generate dynamic shapes with mathematical transformations
void generateDynamicShapes() {
    // Call the function from the PrimitiveShapes.h file
    m3D::generateDynamicShapes(scene, dynamicShapes, dynamicTransforms);
}

// Function to update dynamic shapes based on time
void updateDynamicShapes(float time) {
    if (!useDynamicShapes) return;

    for (size_t i = 0; i < dynamicShapes.size(); i++) {
        if (i < dynamicTransforms.size() && dynamicShapes[i]) {
            // Update the dynamic transform based on time
            dynamicTransforms[i]->update(time);

            // Apply the updated transform to the shape
            dynamicShapes[i]->position = dynamicTransforms[i]->getPosition();
            dynamicShapes[i]->rotation = dynamicTransforms[i]->getRotation() * 57.2958f;
            // Convert to degrees (radians * 180/PI)
            dynamicShapes[i]->scale = dynamicTransforms[i]->getScale();

            // Update material properties
            dynamicShapes[i]->SetColor(dynamicTransforms[i]->getColor());

            // Update shader parameters for material properties
            // These will be applied when the shape is drawn
            dynamicShapes[i]->SetMaterialProperty("diffuse", dynamicTransforms[i]->getDiffuse());
            dynamicShapes[i]->SetMaterialProperty("ambient", dynamicTransforms[i]->getAmbient());
            dynamicShapes[i]->SetMaterialProperty("specular", dynamicTransforms[i]->getSpecular());
        }
    }
}

// Function to render dynamic shapes control window
static void renderDynamicShapesWindow() {
    if (!showDynamicShapesWindow) return;

    ImGui::Begin("Dynamic Shapes", &showDynamicShapesWindow);

    ImGui::Checkbox("Enable Dynamic Shapes", &useDynamicShapes);

    if (ImGui::Button("Regenerate Dynamic Shapes", ImVec2(200, 0))) {
        // Clear existing dynamic shapes
        dynamicShapes.clear();
        dynamicTransforms.clear();

        // Generate new dynamic shapes
        generateDynamicShapes();
    }

    ImGui::Text("Total Dynamic Shapes: %zu", dynamicShapes.size());

    ImGui::End();
}

// Function to load models using the Scene class
bool loadModel(const std::string& name,
               const std::string& relativePath,
               const std::string& modelRoot,
               const std::string& binRoot,
               const glm::vec3& position,
               const glm::vec3& rotation,
               const glm::vec3& scale)
{
    auto findModelPath = [&](const std::string &relPath) -> std::string {
        std::string regular = modelRoot + "/" + relPath;
        std::string bin = binRoot + "/" + relPath;
        if (std::ifstream(regular).good()) return regular;
        if (std::ifstream(bin).good()) return bin;
        return regular; // fallback, will fail on open
    };

    try {
        std::string path = findModelPath(relativePath);
        std::ifstream file(path);
        if (!file.good()) {
            std::cout << "Model file not found or unreadable: " << path << std::endl;
            return false;
        }

        auto obj = scene.CreateModelObject(name, path, position, rotation, scale);
        if (!obj) {
            std::cout << "Failed to create model object: " << name << std::endl;
            return false;
        }

        modelNames.push_back(name);
        std::cout << "Loaded model: " << name << " from " << path << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cout << "Error loading model " << name << ": " << e.what() << std::endl;
        return false;
    }
}
void loadModels(const std::string& modelBasePath, const std::string& binModelBasePath) {
    std::cout << "Starting to load models using Scene class..." << std::endl;

    struct ModelData {
        std::string name;
        std::string relPath;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    std::vector<ModelData> modelsToLoad = {
        {"Backpack", "backpack/backpack.obj", glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0), glm::vec3(1.0f)},
        {"Mansion", "low_poly_mansion__house/scene.gltf", glm::vec3(-10.0f, -0.5f, -10.0f), glm::vec3(0.0f, 245.0f, 0.0f), glm::vec3(0.00625f)},
        //{"Tiptup", "n64/Tiptup/ObjectTortRunner.obj", glm::vec3(13.0f, 0.0f, 5.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.5f)},
        {"Solar System", "solar_system_model_orrery/scene.gltf", glm::vec3(0.0f, 0.0f, 18.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f)},
        {"Model", "koseki_bijou/Model.glb", glm::vec3(-1.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f)},
//        {"ModelH", "hakos_baelz_3d_model_-_mmd_download/scene.gltf", glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)},
        {"ModelM", "hatsune_miku.glb", glm::vec3(-4.0f, 1.0f, 9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.4f)},
        {"Island", "GameCube - The Legend of Zelda The Wind Waker - Windfall Island Lenzos Shop/Lenzos Shop.obj", glm::vec3(8.0f, 0.0f, -1.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.2f)},
        {"Bird", "GameCube - The Legend of Zelda The Wind Waker - Medli/MediBody.dae", glm::vec3(3.0f, 8.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f)},
        {"Pirate", "GameCube - The Legend of Zelda The Wind Waker - Pirate Ship Interior/Pirate Ship Interior.dae", glm::vec3(6.0f, 2.0f, 9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.003f)},
    };

    for (const auto& m : modelsToLoad) {
        loadModel(m.name, m.relPath, modelBasePath, binModelBasePath, m.position, m.rotation, m.scale);
    }

    std::cout << "Loaded " << scene.GetObjectCount() << " models successfully\n";
}

int game3d(int argc, char *argv[], const std::string &type) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);
    // Get the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
#ifdef TESTING
        // Find the last '/' and terminate the string there
        char* lastSlash = strrchr(cwd, '/');
        if (lastSlash != nullptr) {
            *lastSlash = '\0'; // Chop off the last directory
        }
#endif
        std::cout << "Current working directory: " << cwd << std::endl;
    }

    // Set ResourceManager root path to the current directory
    ResourceManager::root = cwd;
    std::cout << "ResourceManager root set to: " << ResourceManager::root << std::endl;

    // Define shader paths relative to the current directory
    std::string vertexShaderPath = ResourceManager::root + "/shaders/3d.vs";
    std::string fragmentShaderPath = ResourceManager::root + "/shaders/3d.fs";

    std::cout << "Loading vertex shader from: " << vertexShaderPath << std::endl;
    std::cout << "Loading fragment shader from: " << fragmentShaderPath << std::endl;

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Model Viewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Configure global OpenGL state
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Initialize ImGui
    Gui::Init(window);

    // Set initial cursor mode - start with cursor disabled for movement
    cursorEnabled = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    firstMouse = true;
    toggleCursor(window);
    std::cout << "Loading vertex shader from: " << vertexShaderPath << std::endl;
    std::cout << "Loading fragment shader from: " << fragmentShaderPath << std::endl;

    try {
        // Check if shader files exist
        std::ifstream vertexFile(vertexShaderPath);
        std::ifstream fragmentFile(fragmentShaderPath);

        if (!vertexFile.good()) {
            std::cout << "Vertex shader file does not exist or cannot be opened!" << std::endl;

            // Try bin/shaders as an alternative
            std::string binVertexPath = std::string(cwd) + "/bin/shaders/3d.vs";
            std::ifstream binVertexFile(binVertexPath);
            if (binVertexFile.good()) {
                std::cout << "Found vertex shader in bin/shaders instead: " << binVertexPath << std::endl;
                vertexShaderPath = binVertexPath;
            }
        } else {
            std::cout << "Vertex shader file exists and can be opened." << std::endl;
        }

        if (!fragmentFile.good()) {
            std::cout << "Fragment shader file does not exist or cannot be opened!" << std::endl;

            // Try bin/shaders as an alternative
            std::string binFragmentPath = std::string(cwd) + "/bin/shaders/3d.fs";
            std::ifstream binFragmentFile(binFragmentPath);
            if (binFragmentFile.good()) {
                std::cout << "Found fragment shader in bin/shaders instead: " << binFragmentPath << std::endl;
                fragmentShaderPath = binFragmentPath;
            }
        } else {
            std::cout << "Fragment shader file exists and can be opened." << std::endl;
        }

        vertexFile.close();
        fragmentFile.close();

        // Try loading the shader using ResourceManager
        std::cout << "Attempting to load shaders using ResourceManager..." << std::endl;
        try {
            ResourceManager::LoadShader("3d.vs", "3d.fs", nullptr, "model");
            ResourceManager::LoadShader("3d.vs", "outline.fs", nullptr, "outline");
            std::cout << "Successfully loaded model shader using ResourceManager" << std::endl;
        } catch (const std::exception &e) {
            std::cout << "Failed to load shader using ResourceManager: " << e.what() << std::endl;
            std::cout << "Falling back to direct path loading..." << std::endl;

            // Fall back to direct path loading
            ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), nullptr, "model");
            std::cout << "Successfully loaded model shader using direct paths" << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Failed to load model shader: " << e.what() << std::endl;
        // Continue anyway to see if we can at least render something
    }

    // Setup ground plane with improved textures
    setupGround();

    // Initialize random point lights
    useRandomPointLights = false; // Start with random lights disabled

    // Load models using the Scene class
    loadModels(cwd, std::string(cwd) + "/models");

    // Generate primitive shapes
    generatePrimitiveShapes();

    // Generate dynamic shapes with mathematical transformations
    generateDynamicShapes();

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Start ImGui frame
        Gui::Start();

        // Render models window
        renderModelsWindow();

        // Render lighting window
        renderLightingWindow();

        // Render dynamic shapes window
        if (showDynamicShapesWindow) {
            renderDynamicShapesWindow();
        }

        // Update dynamic shapes
        if (useDynamicShapes) {
            updateDynamicShapes(currentFrame);
        }

        // Configure shader for rendering
        Shader shader = ResourceManager::GetShader("model");
        shader.Use();

        // Set camera uniforms
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                                0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.SetMatrix4("projection", projection);
        shader.SetMatrix4("view", view);

        // Set lighting uniforms
        setLightingUniforms(shader);
        // On startup
        //glClearStencil(0); // Initial stencil value = 0

        // In render loop
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // First pass - write to stencil
        //glStencilFunc(GL_ALWAYS, 1, 0xFF);
        //glStencilMask(0xFF);

        // Second pass - use stencil
        //glStencilFunc(GL_EQUAL, static_cast<int>(currentFrame) % 2, 0xFF); // Flicker between 0/1
        //glStencilMask(0x00);
        //glStencilOp(GL_INVERT, GL_INCR, GL_REPLACE);

        // Render the scene
        renderScene(shader);

        // Render ImGui
        Gui::Render();

        // Check if random lights need to be updated
        static bool prevRandomLightsState = useRandomPointLights;
        if (useRandomPointLights != prevRandomLightsState) {
            if (useRandomPointLights) {
                generateRandomPointLights();
            } else {
                randomPointLights.clear();
            }
            prevRandomLightsState = useRandomPointLights;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    scene.Clear();

    // Clean up ground resources
    if (groundVAO) {
        glDeleteVertexArrays(1, &groundVAO);
        glDeleteBuffers(1, &groundVBO);
    }

    ResourceManager::Clear();
    Gui::Clean();
    glfwTerminate();
    return 0;
}

static void setupGround() {
    // Create a large ground plane - lowered by 50% (y = -0.5f instead of 0.0f)
    float groundVertices[] = {
        // positions          // normals           // texture coords  // tangent                // bitangent
        -50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        50.0f, -0.5f, 50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        50.0f, -0.5f, 50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -50.0f, -0.5f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) 0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (3 * sizeof(float)));

    // Texture coords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (6 * sizeof(float)));

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (8 * sizeof(float)));

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (11 * sizeof(float)));

    glBindVertexArray(0);

    // Create a larger, more detailed ground texture (4x4 grid instead of 2x2)
    unsigned char groundTextureData[] = {
        // Row 1
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,

        // Row 2
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,

        // Row 3
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,

        // Row 4
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255
    };

    // Generate and bind texture
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use linear filtering

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundTextureData);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps to prevent flickering at distance

    // Create a normal map for the ground
    unsigned char normalMapData[16 * 16 * 4];
    // Fill with flat normals (128, 128, 255, 255)
    for (int i = 0; i < 16 * 16; i++) {
        normalMapData[i * 4 + 0] = 128;
        normalMapData[i * 4 + 1] = 128;
        normalMapData[i * 4 + 2] = 255;
        normalMapData[i * 4 + 3] = 255;
    }

    // Generate and bind normal texture
    glGenTextures(1, &groundNormalTexture);
    glBindTexture(GL_TEXTURE_2D, groundNormalTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload normal map data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalMapData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void renderGround(Shader &shader) {
    // Set model matrix for ground
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);

    // Bind ground diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    shader.SetInteger("texture_diffuse1", 0);

    // Bind ground normal texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, groundNormalTexture);
    shader.SetInteger("texture_normal1", 1);

    // Draw ground
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void renderModelsWindow() {
    if (showModelsWindow) {
        ImGui::Begin("Models", &showModelsWindow);

        if (scene.GetObjectCount() > 0) {
            const char *items[modelNames.size()];
            for (size_t i = 0; i < modelNames.size(); i++) {
                items[i] = modelNames[i].c_str();
            }

            ImGui::Combo("Select Model", &selectedModel, items, modelNames.size());

            if (selectedModel >= 0 && selectedModel < modelNames.size()) {
                std::string modelName = modelNames[selectedModel];
                auto modelObj = scene.GetModelObject(modelName);

                if (modelObj) {
                    ImGui::Text("Model: %s", modelName.c_str());

                    // Position controls
                    ImGui::Text("Position");
                    ImGui::SliderFloat("X##Pos", &modelObj->position.x, -20.0f, 20.0f);
                    ImGui::SliderFloat("Y##Pos", &modelObj->position.y, -20.0f, 20.0f);
                    ImGui::SliderFloat("Z##Pos", &modelObj->position.z, -20.0f, 20.0f);

                    // Rotation controls
                    ImGui::Text("Rotation");
                    ImGui::SliderFloat("X##Rot", &modelObj->rotation.x, 0.0f, 360.0f);
                    ImGui::SliderFloat("Y##Rot", &modelObj->rotation.y, 0.0f, 360.0f);
                    ImGui::SliderFloat("Z##Rot", &modelObj->rotation.z, 0.0f, 360.0f);

                    // Scale controls
                    ImGui::Text("Scale");
                    ImGui::SliderFloat("X##Scale", &modelObj->scale.x, 0.01f, 5.0f);
                    ImGui::SliderFloat("Y##Scale", &modelObj->scale.y, 0.01f, 5.0f);
                    ImGui::SliderFloat("Z##Scale", &modelObj->scale.z, 0.01f, 5.0f);

                    // Visibility toggle
                    ImGui::Checkbox("Visible", &modelObj->visible);
                }
            }
        }

        ImGui::End();
    }
}

static void renderLightingWindow() {
    if (ImGui::Begin("Lighting", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Material settings
        ImGui::Text("Material Settings");
        ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);
        ImGui::Checkbox("Use Normal Map", &useNormalMap);
        ImGui::Checkbox("Use Specular Map", &useSpecularMap);
        ImGui::Checkbox("Use Detail Map", &useDetailMap);
        ImGui::Checkbox("Use Scatter Map", &useScatterMap);
        ImGui::Separator();

        // Light brightness adjustments
        ImGui::Text("Light Brightness Adjustments");
        ImGui::SliderFloat("Point Light Brightness", &pointLightBrightness, 0.1f, 5.0f);
        ImGui::SliderFloat("Directional Light Brightness", &dirLightBrightness, 0.1f, 5.0f);
        ImGui::SliderFloat("Spot Light Brightness", &spotLightBrightness, 0.1f, 5.0f);
        ImGui::Separator();

        // Directional light
        ImGui::Text("Directional Light");
        ImGui::Checkbox("Enable##DirLight", &dirLight.enabled);
        ImGui::ColorEdit3("Ambient##DirLight", (float *) &dirLight.ambient);
        ImGui::ColorEdit3("Diffuse##DirLight", (float *) &dirLight.diffuse);
        ImGui::ColorEdit3("Specular##DirLight", (float *) &dirLight.specular);
        ImGui::SliderFloat3("Direction##DirLight", (float *) &dirLight.direction, -1.0f, 1.0f);
        ImGui::Separator();

        // Point light
        ImGui::Text("Point Light");
        ImGui::Checkbox("Enable##PointLight", &pointLight.enabled);
        ImGui::ColorEdit3("Ambient##PointLight", (float *) &pointLight.ambient);
        ImGui::ColorEdit3("Diffuse##PointLight", (float *) &pointLight.diffuse);
        ImGui::ColorEdit3("Specular##PointLight", (float *) &pointLight.specular);
        ImGui::SliderFloat3("Position##PointLight", (float *) &pointLight.position, -20.0f, 20.0f);
        ImGui::SliderFloat("Constant##PointLight", &pointLight.constant, 0.0f, 1.0f);
        ImGui::SliderFloat("Linear##PointLight", &pointLight.linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Quadratic##PointLight", &pointLight.quadratic, 0.0f, 1.0f);
        ImGui::Separator();

        // Spot light
        ImGui::Text("Spot Light (Flashlight)");
        ImGui::Checkbox("Enable##SpotLight", &spotLight.enabled);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Position and direction follow camera");
        ImGui::ColorEdit3("Ambient##SpotLight", (float *) &spotLight.ambient);
        ImGui::ColorEdit3("Diffuse##SpotLight", (float *) &spotLight.diffuse);
        ImGui::ColorEdit3("Specular##SpotLight", (float *) &spotLight.specular);
        ImGui::SliderFloat("Cut Off##SpotLight", &spotLight.cutOff, 0.0f, 1.0f);
        ImGui::SliderFloat("Outer Cut Off##SpotLight", &spotLight.outerCutOff, 0.0f, 1.0f);
        ImGui::SliderFloat("Constant##SpotLight", &spotLight.constant, 0.0f, 1.0f);
        ImGui::SliderFloat("Linear##SpotLight", &spotLight.linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Quadratic##SpotLight", &spotLight.quadratic, 0.0f, 1.0f);
        ImGui::Separator();

        // Random Point Lights
        ImGui::Text("Random Point Lights");
        ImGui::Checkbox("Enable Random Lights", &useRandomPointLights);

        ImGui::Text("Random Lights: %zu", randomPointLights.size());

        if (ImGui::Button("Regenerate Random Lights", ImVec2(200, 0))) {
            generateRandomPointLights();
        }

        ImGui::SliderFloat("Min Distance Between Lights", &MIN_DISTANCE_BETWEEN_LIGHTS, 5.0f, 30.0f);
    }
    ImGui::End();
}

static bool toggleKey(int key, bool &toggleState) {
    bool keyCurrentlyPressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;

    if (!keyWasPressed[key] && keyCurrentlyPressed) {
        // Key was just pressed
        toggleState = !toggleState; // Toggle the state
        keyWasPressed[key] = true; // Set the flag for this key
        return true; // Indicate that the state was toggled
    } else if (keyWasPressed[key] && !keyCurrentlyPressed) {
        // Key was just released
        keyWasPressed[key] = false; // Reset the flag for this key
    }

    return false; // Indicate that the state was not toggled
}

static void toggleCursor(GLFWwindow *window) {
    cursorEnabled = !cursorEnabled;
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    if (!cursorEnabled) {
        // When disabling cursor, reset firstMouse to recalculate reference posit`n
        firstMouse = true;
    }
}

static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    // Only process mouse movement if ImGui is not capturing the mouse

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

    // Process mouse movement if cursor is disabled
    if (!cursorEnabled) {
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    // Only process scroll if ImGui is not capturing the mouse
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static void processInput(GLFWwindow *window) {
    // Check for ImGui keyboard capture

    // Always process Escape and Enter keys regardless of ImGui focus
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle cursor with Enter key
    static bool enterToggle = false;
    if (toggleKey(GLFW_KEY_ENTER, enterToggle)) {
        toggleCursor(window);
        std::cout << "Cursor mode toggled: " << (cursorEnabled ? "Enabled" : "Disabled") << std::endl;
    }

    // Toggle Cartesian plane visibility with P key
    static bool pToggle = false;
    if (toggleKey(GLFW_KEY_P, pToggle)) {
        showCartesianPlane = !showCartesianPlane;
        std::cout << "Cartesian plane visibility: " << (showCartesianPlane ? "Shown" : "Hidden") << std::endl;
    }

    // Toggle triangle contours with G key
    static bool gToggle = false;
    if (toggleKey(GLFW_KEY_G, gToggle)) {
        showTriangleContours = !showTriangleContours;
        if (showTriangleContours) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show wireframe
            std::cout << "Triangle contours: Shown" << std::endl;
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Show filled polygons
            std::cout << "Triangle contours: Hidden" << std::endl;
        }
    }

    // Toggle run/walk mode with / key
    static bool slashToggle = false;
    if (toggleKey(GLFW_KEY_SLASH, slashToggle)) {
        runMode = !runMode;
        if (runMode) {
            camera.MovementSpeed = baseMovementSpeed;
            std::cout << "Movement mode: Run (Speed: " << camera.MovementSpeed << ")" << std::endl;
        } else {
            camera.MovementSpeed = baseMovementSpeed * 0.5f;
            std::cout << "Movement mode: Walk (Speed: " << camera.MovementSpeed << ")" << std::endl;
        }
    }

    // Manually increase speed with ] key
    static bool rightBracketToggle = false;
    if (toggleKey(GLFW_KEY_RIGHT_BRACKET, rightBracketToggle)) {
        baseMovementSpeed += 0.5f;
        if (runMode) {
            camera.MovementSpeed = baseMovementSpeed;
        } else {
            camera.MovementSpeed = baseMovementSpeed * 0.5f;
        }
        std::cout << "Movement speed increased to: " << camera.MovementSpeed << std::endl;
    }

    // Manually decrease speed with [ key
    static bool leftBracketToggle = false;
    if (toggleKey(GLFW_KEY_LEFT_BRACKET, leftBracketToggle)) {
        baseMovementSpeed = std::max(0.5f, baseMovementSpeed - 0.5f);
        if (runMode) {
            camera.MovementSpeed = baseMovementSpeed;
        } else {
            camera.MovementSpeed = baseMovementSpeed * 0.5f;
        }
        std::cout << "Movement speed decreased to: " << camera.MovementSpeed << std::endl;
    }

    // Zoom in with = key
    static bool equalToggle = false;
    if (toggleKey(GLFW_KEY_EQUAL, equalToggle)) {
        camera.Zoom = std::max(1.0f, camera.Zoom - 1.0f);
        std::cout << "Zoom level: " << camera.Zoom << std::endl;
    }

    // Zoom out with - key
    static bool minusToggle = false;
    if (toggleKey(GLFW_KEY_MINUS, minusToggle)) {
        camera.Zoom = std::min(45.0f, camera.Zoom + 1.0f);
        std::cout << "Zoom level: " << camera.Zoom << std::endl;
    }

    // Check for shift key (faster movement)
    float speedMultiplier = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        speedMultiplier = 2.0f; // Double speed when shift is pressed
    }

    // Check for ctrl key (slower movement)
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        speedMultiplier = 0.5f; // Half speed when ctrl is pressed
    }

    // Apply speed multiplier to camera
    float originalSpeed = camera.MovementSpeed;
    camera.MovementSpeed *= speedMultiplier;

    // Process movement keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_RIGHT, deltaTime);

    // Additional camera controls
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.Position.y += camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.Position.y -= camera.MovementSpeed * deltaTime;

    // Reset camera speed to original value
    camera.MovementSpeed = originalSpeed;
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Function to render the scene with the CartesianPlane visibility check
void renderScene(Shader &shader) {
    // PHASE 1: Render regular objects and mark them in stencil buffer
    glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor
    // Render the ground
    renderGround(shader);
    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // Render models from the scene
    for (const auto &modelName: modelNames) {
        auto modelObj = scene.GetModelObject(modelName);
        if (modelObj && modelObj->visible) {
            modelObj->Draw(shader);
        }
    }

    // Render primitive shapes
    for (auto &shape: primitiveShapes) {
        if (shape && shape->visible) {
            shape->Draw(shader);
        }
    }
    // Render dynamic shapes
    if (useDynamicShapes) {
        for (auto &shape: dynamicShapes) {
            // Skip CartesianPlane objects if they should be hidden
            std::string name = shape->name;
            if (!showCartesianPlane &&
                (name.find("CartesianPlane") != std::string::npos ||
                 name.find("WhiteGridCube") != std::string::npos)) {
                continue; // Skip rendering this shape
                 }

            shape->Draw(shader);
        }
    }

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    // Get the outline shader
    Shader outlineShader = ResourceManager::GetShader("outline");

    // Explicitly activate the shader before rendering
    outlineShader.Use();
    outlineShader.SetMatrix4("view", camera.GetViewMatrix());
    outlineShader.SetMatrix4("projection", projection);
    // Scale factor for outlines (slightly larger than original)
    const float outlineScale = 1.03f; // 5% larger

    // Render model outlines
    for (const auto &modelName: modelNames) {
        auto modelObj = scene.GetModelObject(modelName);
        if (modelObj && modelObj->visible) {
            // Store original transform
            glm::vec3 originalScale = modelObj->scale;

            // Apply scaled-up transformation
            modelObj->scale *= outlineScale;

            // Make sure shader is active before each draw call
            outlineShader.Use();

            try {
                // Draw with outline shader
                modelObj->Draw(outlineShader);
            } catch (const std::exception& e) {
                std::cerr << "Error drawing model outline for " << modelName
                          << ": " << e.what() << std::endl;
            }

            // Restore original scale
            modelObj->scale = originalScale;
        }
    }

    // Render primitive shape outlines
    for (auto &shape: primitiveShapes) {
        if (shape && shape->visible) {
            // Store original transform
            glm::vec3 originalScale = shape->scale;

            // Apply scaled-up transformation
            shape->scale *= outlineScale;

            // Make sure shader is active before each draw call
            outlineShader.Use();

            try {
                // Draw with outline shader
                shape->Draw(outlineShader);
            } catch (const std::exception& e) {
                std::cerr << "Error drawing shape outline: " << e.what() << std::endl;
            }

            // Restore original transform
            shape->scale = originalScale;
        }
    }

    // restore state
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}
// Function to set lighting uniforms
void setLightingUniforms(Shader &shader) {
    // Set material properties
    shader.SetFloat("shininess", shininess);
    shader.SetInteger("useNormalMap", useNormalMap ? 1 : 0);
    shader.SetInteger("useSpecularMap", useSpecularMap ? 1 : 0);
    shader.SetInteger("useDetailMap", useDetailMap ? 1 : 0);
    shader.SetInteger("useScatterMap", useScatterMap ? 1 : 0);

    // Set light brightness adjustment uniforms
    shader.SetFloat("pointLightBrightness", pointLightBrightness);
    shader.SetFloat("dirLightBrightness", dirLightBrightness);
    shader.SetFloat("spotLightBrightness", spotLightBrightness);

    // Set camera position for lighting calculations
    shader.SetVector3f("viewPos", camera.Position);

    // Update spotlight position and direction to match camera
    spotLight.position = camera.Position;
    spotLight.direction = camera.Front;

    // Set directional light properties
    shader.SetVector3f("dirLight.direction", dirLight.direction);
    shader.SetVector3f("dirLight.ambient", dirLight.ambient);
    shader.SetVector3f("dirLight.diffuse", dirLight.diffuse);
    shader.SetVector3f("dirLight.specular", dirLight.specular);
    shader.SetInteger("useDirLight", dirLight.enabled ? 1 : 0);

    // Set main point light properties
    shader.SetVector3f("pointLight.position", pointLight.position);
    shader.SetFloat("pointLight.constant", pointLight.constant);
    shader.SetFloat("pointLight.linear", pointLight.linear);
    shader.SetFloat("pointLight.quadratic", pointLight.quadratic);
    shader.SetVector3f("pointLight.ambient", pointLight.ambient);
    shader.SetVector3f("pointLight.diffuse", pointLight.diffuse);
    shader.SetVector3f("pointLight.specular", pointLight.specular);
    shader.SetInteger("usePointLight", pointLight.enabled ? 1 : 0);

    // Set spotlight position and direction for tangent space calculations
    shader.SetVector3f("spotLightPos", spotLight.position);
    shader.SetVector3f("spotLightDir", spotLight.direction);

    // Set spotlight uniforms (add these back)
    shader.SetVector3f("spotLight.position", spotLight.position);
    shader.SetVector3f("spotLight.direction", spotLight.direction);
    shader.SetFloat("spotLight.cutOff", spotLight.cutOff);
    shader.SetFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    shader.SetFloat("spotLight.constant", spotLight.constant);
    shader.SetFloat("spotLight.linear", spotLight.linear);
    shader.SetFloat("spotLight.quadratic", spotLight.quadratic);
    shader.SetVector3f("spotLight.ambient", spotLight.ambient);
    shader.SetVector3f("spotLight.diffuse", spotLight.diffuse);
    shader.SetVector3f("spotLight.specular", spotLight.specular);
    shader.SetInteger("useSpotLight", spotLight.enabled ? 1 : 0);

    // Set random point lights
    shader.SetInteger("numRandomPointLights", static_cast<int>(randomPointLights.size()));
    shader.SetInteger("useRandomPointLights", useRandomPointLights ? 1 : 0);

    for (size_t i = 0; i < randomPointLights.size() && i < MAX_POINT_LIGHTS; i++) {
        std::string index = std::to_string(i);
        shader.SetVector3f(("randomPointLights[" + index + "].position").c_str(), randomPointLights[i].position);
        shader.SetFloat(("randomPointLights[" + index + "].constant").c_str(), randomPointLights[i].constant);
        shader.SetFloat(("randomPointLights[" + index + "].linear").c_str(), randomPointLights[i].linear);
        shader.SetFloat(("randomPointLights[" + index + "].quadratic").c_str(), randomPointLights[i].quadratic);
        shader.SetVector3f(("randomPointLights[" + index + "].ambient").c_str(), randomPointLights[i].ambient);
        shader.SetVector3f(("randomPointLights[" + index + "].diffuse").c_str(), randomPointLights[i].diffuse);
        shader.SetVector3f(("randomPointLights[" + index + "].specular").c_str(), randomPointLights[i].specular);
    }
}
