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

// Model variables
std::vector<m3D::Model*> models;
std::vector<glm::vec3> modelPositions;
std::vector<glm::vec3> modelRotations;
std::vector<glm::vec3> modelScales;
bool showModelsWindow = true;
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
    glm::vec3(-0.2f, -1.0f, -0.3f),  // direction
    glm::vec3(0.4f, 0.4f, 0.4f),     // ambient - increased from 0.2 to 0.4
    glm::vec3(1.0f, 1.0f, 1.0f),     // diffuse - increased from 0.8 to 1.0
    glm::vec3(1.0f, 1.0f, 1.0f),     // specular - already at max
    true                             // enabled
};

// Main point light (separate from random lights)
static PointLight pointLight = {
    glm::vec3(1.2f, 1.0f, 2.0f),     // position
    1.0f,                            // constant
    0.09f,                           // linear
    0.032f,                          // quadratic
    glm::vec3(0.2f, 0.2f, 0.2f),     // ambient - increased from 0.1 to 0.2
    glm::vec3(1.0f, 1.0f, 1.0f),     // diffuse - increased from 0.8 to 1.0
    glm::vec3(1.0f, 1.0f, 1.0f),     // specular - already at max
    true                             // enabled
};

// Update the spotlight to be enabled by default and increase brightness
static SpotLight spotLight = {
    glm::vec3(0.0f, 10.0f, 0.0f),     // position - raised higher for better coverage
    glm::vec3(0.0f, -1.0f, 0.0f),    // direction - pointing straight down
    glm::cos(glm::radians(15.0f)),   // cutOff - slightly wider beam
    glm::cos(glm::radians(20.0f)),   // outerCutOff - slightly wider outer beam
    1.0f,                            // constant
    0.01f,                           // linear - decreased further for less attenuation
    0.001f,                          // quadratic - decreased further for less attenuation
    glm::vec3(0.0f, 0.0f, 0.0f),     // ambient - keep at 0
    glm::vec3(5.0f, 5.0f, 5.0f),     // diffuse - increased significantly
    glm::vec3(5.0f, 5.0f, 5.0f),     // specular - increased significantly
    true                             // enabled (now true by default)
};

// Add random point lights
const int MAX_POINT_LIGHTS = 20;
std::vector<PointLight> randomPointLights;
float MIN_DISTANCE_BETWEEN_LIGHTS = 10.0f; // Minimum distance between lights - now non-const
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
static float pointLightBrightness = 0.05f; // Increased from 0.5 to 0.8
static float dirLightBrightness = 1.7f;   // Increased from 2.0 to 2.5
static float spotLightBrightness = 1.0f;  // Increased from 2.5 to 4.0

// Add variables for primitive shapes
std::vector<std::shared_ptr<m3D::PrimitiveShape>> primitiveShapes;
std::vector<glm::vec3> rotationSpeeds; // Store rotation speeds for each primitive

// Forward declare static functions
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);
static void renderModelsWindow();
static void renderLightingWindow();
static void setupGround();
static void renderGround(Shader &shader);
static bool toggleKey(int key, bool &toggleState);
static void toggleCursor(GLFWwindow *window);

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
    
    // Position distributions
    std::uniform_real_distribution<float> posDistX(-20.0f, 20.0f);
    std::uniform_real_distribution<float> posDistY(0.5f, 10.0f);
    std::uniform_real_distribution<float> posDistZ(-20.0f, 20.0f);
    
    // Color distribution
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
    
    // Attenuation distributions
    std::uniform_real_distribution<float> constantDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> linearDist(0.01f, 0.1f);
    std::uniform_real_distribution<float> quadraticDist(0.001f, 0.01f);
    
    // Number of lights to generate
    const int NUM_LIGHTS = 10;
    
    // Generate random lights
    for (int i = 0; i < NUM_LIGHTS; i++) {
        // Generate a random position
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        
        // Check if this position is far enough from existing lights
        bool validPosition = true;
        for (const auto& light : randomPointLights) {
            float distance = glm::length(position - light.position);
            if (distance < MIN_DISTANCE_BETWEEN_LIGHTS) {
                validPosition = false;
                break;
            }
        }
        
        // If position is not valid, try again
        if (!validPosition) {
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
    
    std::cout << "Generated " << randomPointLights.size() << " random point lights" << std::endl;
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

// Function to load models using the Scene class
void loadModels(const std::string& modelBasePath, const std::string& binModelBasePath) {
    std::cout << "Starting to load models using Scene class..." << std::endl;
    
    // Function to check both regular and bin paths for models
    auto findModelPath = [&](const std::string& relativePath) -> std::string {
        std::string regularPath = modelBasePath + "/" + relativePath;
        std::string binPath = binModelBasePath + "/" + relativePath;
        
        std::ifstream regularFile(regularPath);
        if (regularFile.good()) {
            std::cout << "Found model at: " << regularPath << std::endl;
            return regularPath;
        }
        
        std::ifstream binFile(binPath);
        if (binFile.good()) {
            std::cout << "Found model at bin path: " << binPath << std::endl;
            return binPath;
        }
        
        std::cout << "Model not found at either path: " << regularPath << " or " << binPath << std::endl;
        return regularPath; // Return regular path anyway, will fail with proper error
    };
    
    // Load backpack model
    try {
        std::string backpackPath = findModelPath("backpack/backpack.obj");
        std::cout << "Attempting to load backpack model from " << backpackPath << std::endl;
        
        // Check if file exists
        std::ifstream backpackFile(backpackPath);
        if (!backpackFile.good()) {
            std::cout << "Backpack model file does not exist or cannot be opened!" << std::endl;
        } else {
            std::cout << "Backpack model file exists and can be opened." << std::endl;
            backpackFile.close();
            
            auto backpackObj = scene.CreateModelObject(
                "Backpack", 
                backpackPath, 
                glm::vec3(0.0f, 1.5f, 0.0f),  // Position above ground (adjusted for lowered ground)
                glm::vec3(0.0f, 0.0f, 0.0f),  // No rotation
                glm::vec3(1.0f, 1.0f, 1.0f)   // Normal scale
            );
            
            if (backpackObj) {
                modelNames.push_back("Backpack");
                std::cout << "Successfully loaded backpack model" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load backpack model: " << e.what() << std::endl;
    }
    
    // Load mansion model
    try {
        std::string mansionPath = findModelPath("low_poly_mansion__house/scene.gltf");
        std::cout << "Attempting to load mansion model from " << mansionPath << std::endl;
        
        // Check if file exists
        std::ifstream mansionFile(mansionPath);
        if (!mansionFile.good()) {
            std::cout << "Mansion model file does not exist or cannot be opened!" << std::endl;
        } else {
            std::cout << "Mansion model file exists and can be opened." << std::endl;
            mansionFile.close();
            
            // Use a much smaller scale for the mansion (0.00625 instead of 0.05, 800% smaller)
            auto mansionObj = scene.CreateModelObject(
                "Mansion", 
                mansionPath, 
                glm::vec3(-10.0f, -0.5f, -10.0f),  // Position on ground (adjusted for lowered ground)
                glm::vec3(0.0f, 45.0f, 0.0f),     // Rotate 45 degrees
                glm::vec3(0.00625f, 0.00625f, 0.00625f)    // 800% smaller scale
            );
            
            if (mansionObj) {
                modelNames.push_back("Mansion");
                std::cout << "Successfully loaded mansion model" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load mansion model: " << e.what() << std::endl;
    }
    
    // Load tiptup model
    try {
        std::string tiptupPath = findModelPath("n64/Tiptup/ObjectTortRunner.obj");
        std::cout << "Attempting to load tiptup model from " << tiptupPath << std::endl;
        
        // Check if file exists
        std::ifstream tiptupFile(tiptupPath);
        if (!tiptupFile.good()) {
            std::cout << "Tiptup model file does not exist or cannot be opened!" << std::endl;
        } else {
            std::cout << "Tiptup model file exists and can be opened." << std::endl;
            tiptupFile.close();
            
            auto tiptupObj = scene.CreateModelObject(
                "Tiptup", 
                tiptupPath, 
                glm::vec3(5.0f, 0.0f, 5.0f),      // Position on ground (adjusted for lowered ground)
                glm::vec3(0.0f, 180.0f, 0.0f),    // Rotate 180 degrees
                glm::vec3(0.5f, 0.5f, 0.5f)       // Half scale
            );
            
            if (tiptupObj) {
                modelNames.push_back("Tiptup");
                std::cout << "Successfully loaded tiptup model" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load tiptup model: " << e.what() << std::endl;
    }
    
    // Load terry model
    try {
        std::string terryPath = findModelPath("n64/Terry/ObjectTerryboss.obj");
        std::cout << "Attempting to load terry model from " << terryPath << std::endl;
        
        // Check if file exists
        std::ifstream terryFile(terryPath);
        if (!terryFile.good()) {
            std::cout << "Terry model file does not exist or cannot be opened!" << std::endl;
        } else {
            std::cout << "Terry model file exists and can be opened." << std::endl;
            terryFile.close();
            
            auto terryObj = scene.CreateModelObject(
                "Terry", 
                terryPath, 
                glm::vec3(-5.0f, -0.5f, 5.0f),     // Position on ground (adjusted for lowered ground)
                glm::vec3(0.0f, 135.0f, 0.0f),    // Rotate 135 degrees
                glm::vec3(0.02f, 0.02f, 0.02f)    // Very small scale
            );
            
            if (terryObj) {
                modelNames.push_back("Terry");
                std::cout << "Successfully loaded terry model" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load terry model: " << e.what() << std::endl;
    }
    
    std::cout << "Loaded " << scene.GetObjectCount() << " models successfully" << std::endl;
    
    // Generate primitive shapes
    std::cout << "\n=== About to call generatePrimitiveShapes() ===\n" << std::endl;
    generatePrimitiveShapes();
    std::cout << "\n=== Returned from generatePrimitiveShapes() ===\n" << std::endl;
}

int game3d(int argc, char *argv[], const std::string& type) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);
// 
    // Get the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }

    // Set ResourceManager root path to the parent directory of the current directory
    std::string currentDir = cwd;
    std::string parentDir = currentDir;
    
    // Log all files and directories in the current directory
    std::cout << "\n=== Files and directories in current directory ===\n";
    system("ls -la");
    
    // Set ResourceManager root path to the current directory
    ResourceManager::root = currentDir;
    std::cout << "ResourceManager root set to: " << ResourceManager::root << std::endl;
    
    // Log the structure of important directories
    std::cout << "\n=== Checking models directory ===\n";
    system("ls -la models 2>/dev/null || echo 'models directory not found'");
    
    std::cout << "\n=== Checking shaders directory ===\n";
    system("ls -la shaders 2>/dev/null || echo 'shaders directory not found'");
    
    // Create bin directory if it doesn't exist
    system("mkdir -p bin/models bin/shaders");
    
    // Copy necessary files to bin directory
    std::cout << "\n=== Copying resources to bin directory ===\n";
    system("cp -rv shaders/* bin/shaders/ 2>/dev/null || echo 'No shader files to copy'");
    system("cp -rv models/* bin/models/ 2>/dev/null || echo 'No model files to copy'");

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Model Viewer", nullptr, nullptr);
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

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

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
    
    // Set initial cursor mode - start with cursor disabled for movement
    cursorEnabled = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    firstMouse = true;

    // Load shaders with absolute paths
    std::string vertexShaderPath = std::string(cwd) + "/shaders/3d.vs";
    std::string fragmentShaderPath = std::string(cwd) + "/shaders/3d.fs";
    
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
            std::cout << "Successfully loaded model shader using ResourceManager" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Failed to load shader using ResourceManager: " << e.what() << std::endl;
            std::cout << "Falling back to direct path loading..." << std::endl;
            
            // Fall back to direct path loading
            ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), nullptr, "model");
            std::cout << "Successfully loaded model shader using direct paths" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load model shader: " << e.what() << std::endl;
        // Continue anyway to see if we can at least render something
    }
    
    // Setup ground plane with improved textures
    setupGround();
    
    // Initialize random point lights
    useRandomPointLights = false; // Start with random lights disabled
    
    // Load models using the Scene class
    loadModels(currentDir, std::string(cwd) + "/bin/models");
    
    // Game loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start ImGui frame
        Gui::Start();
        
        // Render models window
        renderModelsWindow();
        
        // Render lighting window
        renderLightingWindow();
        
        // Update primitive rotations
        updatePrimitiveRotations(deltaTime);

        // Get the shader and activate it once before setting all uniforms
        try {
            Shader shader = ResourceManager::GetShader("model");
            shader.Use();

            // Set projection matrix
            float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);
            shader.SetMatrix4("projection", projection);
            
            // Set view matrix
            glm::mat4 view = camera.GetViewMatrix();
            shader.SetMatrix4("view", view);

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
            
            // Set spotlight properties
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
            
            // Draw the ground
            renderGround(shader);
            
            // Draw all scene objects
            scene.Draw(shader);
            
        } catch (const std::exception& e) {
            std::cout << "Error in render loop: " << e.what() << std::endl;
        }
        
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
        -50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 0.0f,      1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, -0.5f,  50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 50.0f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         
        -50.0f, -0.5f, -50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, -0.5f,  50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 50.0f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
        -50.0f, -0.5f,  50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 50.0f,      1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f
    };
    
    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Texture coords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    
    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    
    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    
    glBindVertexArray(0);
    
    // Create a larger, more detailed ground texture (4x4 grid instead of 2x2)
    unsigned char groundTextureData[] = {
        // Row 1
        20, 40, 100, 255,  30, 50, 110, 255,  30, 50, 110, 255,  20, 40, 100, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        20, 40, 100, 255,  30, 50, 110, 255,  30, 50, 110, 255,  20, 40, 100, 255,
        
        // Row 2
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        40, 60, 120, 255,  50, 70, 130, 255,  50, 70, 130, 255,  40, 60, 120, 255,
        40, 60, 120, 255,  50, 70, 130, 255,  50, 70, 130, 255,  40, 60, 120, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        
        // Row 3
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        40, 60, 120, 255,  50, 70, 130, 255,  50, 70, 130, 255,  40, 60, 120, 255,
        40, 60, 120, 255,  50, 70, 130, 255,  50, 70, 130, 255,  40, 60, 120, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        
        // Row 4
        20, 40, 100, 255,  30, 50, 110, 255,  30, 50, 110, 255,  20, 40, 100, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        30, 50, 110, 255,  40, 60, 120, 255,  40, 60, 120, 255,  30, 50, 110, 255,
        20, 40, 100, 255,  30, 50, 110, 255,  30, 50, 110, 255,  20, 40, 100, 255
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
            const char* items[modelNames.size()];
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
        ImGui::ColorEdit3("Ambient##DirLight", (float*)&dirLight.ambient);
        ImGui::ColorEdit3("Diffuse##DirLight", (float*)&dirLight.diffuse);
        ImGui::ColorEdit3("Specular##DirLight", (float*)&dirLight.specular);
        ImGui::SliderFloat3("Direction##DirLight", (float*)&dirLight.direction, -1.0f, 1.0f);
        ImGui::Separator();
        
        // Point light
        ImGui::Text("Point Light");
        ImGui::Checkbox("Enable##PointLight", &pointLight.enabled);
        ImGui::ColorEdit3("Ambient##PointLight", (float*)&pointLight.ambient);
        ImGui::ColorEdit3("Diffuse##PointLight", (float*)&pointLight.diffuse);
        ImGui::ColorEdit3("Specular##PointLight", (float*)&pointLight.specular);
        ImGui::SliderFloat3("Position##PointLight", (float*)&pointLight.position, -20.0f, 20.0f);
        ImGui::SliderFloat("Constant##PointLight", &pointLight.constant, 0.0f, 1.0f);
        ImGui::SliderFloat("Linear##PointLight", &pointLight.linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Quadratic##PointLight", &pointLight.quadratic, 0.0f, 1.0f);
        ImGui::Separator();
        
        // Spot light
        ImGui::Text("Spot Light");
        ImGui::Checkbox("Enable##SpotLight", &spotLight.enabled);
        ImGui::ColorEdit3("Ambient##SpotLight", (float*)&spotLight.ambient);
        ImGui::ColorEdit3("Diffuse##SpotLight", (float*)&spotLight.diffuse);
        ImGui::ColorEdit3("Specular##SpotLight", (float*)&spotLight.specular);
        ImGui::SliderFloat3("Position##SpotLight", (float*)&spotLight.position, -20.0f, 20.0f);
        ImGui::SliderFloat3("Direction##SpotLight", (float*)&spotLight.direction, -1.0f, 1.0f);
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
        keyWasPressed[key] = true;  // Set the flag for this key
        return true;                // Indicate that the state was toggled
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

static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Only process scroll if ImGui is not capturing the mouse
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static void processInput(GLFWwindow* window) {
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
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} 