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
    glm::vec3(0.2f, 0.2f, 0.2f),     // ambient - increased from 0.1 to 0.2
    glm::vec3(0.8f, 0.8f, 0.8f),     // diffuse - increased from 0.4 to 0.8
    glm::vec3(1.0f, 1.0f, 1.0f),     // specular - already at max
    true                             // enabled
};

// Main point light (separate from random lights)
static PointLight pointLight = {
    glm::vec3(1.2f, 1.0f, 2.0f),     // position
    1.0f,                            // constant
    0.09f,                           // linear
    0.032f,                          // quadratic
    glm::vec3(0.1f, 0.1f, 0.1f),     // ambient
    glm::vec3(0.8f, 0.8f, 0.8f),     // diffuse
    glm::vec3(1.0f, 1.0f, 1.0f),     // specular
    true                             // enabled
};

// Update the spotlight to be enabled by default
static SpotLight spotLight = {
    glm::vec3(0.0f, 5.0f, 0.0f),     // position
    glm::vec3(0.0f, -1.0f, 0.0f),    // direction
    glm::cos(glm::radians(12.5f)),   // cutOff
    glm::cos(glm::radians(17.5f)),   // outerCutOff
    1.0f,                            // constant
    0.09f,                           // linear
    0.032f,                          // quadratic
    glm::vec3(0.0f, 0.0f, 0.0f),     // ambient - keep at 0
    glm::vec3(2.0f, 2.0f, 2.0f),     // diffuse - doubled from 1.0 to 2.0
    glm::vec3(2.0f, 2.0f, 2.0f),     // specular - doubled from 1.0 to 2.0
    true                             // enabled (now true by default)
};

// Add random point lights
const int MAX_POINT_LIGHTS = 20;
std::vector<PointLight> randomPointLights;
float MIN_DISTANCE_BETWEEN_LIGHTS = 10.0f; // Minimum distance between lights - now non-const

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
static float pointLightBrightness = 0.002f; // 120% lower (0.2 - 0.12 = 0.08)
static float dirLightBrightness = 0.6f;    // 40% higher
static float spotLightBrightness = 4.5f;   // 80% higher

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
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define distributions for position, color, and attenuation
    std::uniform_real_distribution<float> posDistX(-50.0f, 50.0f);
    std::uniform_real_distribution<float> posDistY(0.5f, 10.0f);
    std::uniform_real_distribution<float> posDistZ(-50.0f, 50.0f);
    
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> attenuationDist(0.001f, 0.01f);
    
    // Clear existing lights
    randomPointLights.clear();
    
    // Try to generate MAX_POINT_LIGHTS lights
    int attempts = 0;
    const int maxAttempts = 1000; // Prevent infinite loop
    
    while (randomPointLights.size() < MAX_POINT_LIGHTS && attempts < maxAttempts) {
        attempts++;
        
        // Generate random position
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        
        // Check distance from existing lights
        bool tooClose = false;
        for (const auto& light : randomPointLights) {
            if (glm::distance(position, light.position) < MIN_DISTANCE_BETWEEN_LIGHTS) {
                tooClose = true;
                break;
            }
        }
        
        if (!tooClose) {
            // Generate random color
            glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
            
            // Create new point light
            PointLight light = {
                position,                   // position
                1.0f,                       // constant
                0.09f,                      // linear
                attenuationDist(gen),       // quadratic - random for variety
                color * 0.1f,               // ambient - 10% of color
                color,                      // diffuse - full color
                color,                      // specular - full color
                true                        // enabled
            };
            
            randomPointLights.push_back(light);
            std::cout << "Added random point light at position: (" 
                      << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        }
    }
    
    std::cout << "Generated " << randomPointLights.size() << " random point lights" << std::endl;
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
    
    // Generate random point lights
    generateRandomPointLights();
    
    // Load models with absolute paths
    std::string modelBasePath = std::string(cwd) + "/models";
    std::cout << "Model base path: " << modelBasePath << std::endl;
    
    // Also check bin/models directory
    std::string binModelBasePath = std::string(cwd) + "/bin/models";
    std::cout << "Checking bin model path: " << binModelBasePath << std::endl;
    
    // Log the contents of both model directories
    std::cout << "\n=== Contents of models directory ===\n";
    system("find models -type f -name \"*.obj\" -o -name \"*.gltf\" 2>/dev/null || echo 'No model files found'");
    
    std::cout << "\n=== Contents of bin/models directory ===\n";
    system("find bin/models -type f -name \"*.obj\" -o -name \"*.gltf\" 2>/dev/null || echo 'No model files found'");
    
    try {
        Shader shader = ResourceManager::GetShader("model");
        std::cout << "Shader ID: " << shader.ID << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to get model shader: " << e.what() << std::endl;
    }
    
    std::cout << "Starting to load models..." << std::endl;
    
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
            
            models.push_back(new m3D::Model(backpackPath));
            std::cout << "Successfully loaded backpack model" << std::endl;
            modelPositions.push_back(glm::vec3(0.0f, 2.0f, 0.0f)); // Position above ground
            modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            modelScales.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
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
            
            models.push_back(new m3D::Model(mansionPath));
            std::cout << "Successfully loaded mansion model" << std::endl;
            modelPositions.push_back(glm::vec3(-10.0f, 0.0f, -10.0f)); // Position on ground
            modelRotations.push_back(glm::vec3(0.0f, 45.0f, 0.0f));
            modelScales.push_back(glm::vec3(0.1f, 0.1f, 0.1f));
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
            
            models.push_back(new m3D::Model(tiptupPath));
            std::cout << "Successfully loaded tiptup model" << std::endl;
            modelPositions.push_back(glm::vec3(5.0f, 0.5f, 5.0f)); // Position slightly above ground
            modelRotations.push_back(glm::vec3(0.0f, 180.0f, 0.0f));
            modelScales.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
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
            
            models.push_back(new m3D::Model(terryPath));
            std::cout << "Successfully loaded terry model" << std::endl;
            modelPositions.push_back(glm::vec3(-5.0f, 0.0f, 5.0f)); // Position on ground
            modelRotations.push_back(glm::vec3(0.0f, 135.0f, 0.0f));
            modelScales.push_back(glm::vec3(0.02f, 0.02f, 0.02f));
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load terry model: " << e.what() << std::endl;
    }
    
    std::cout << "Loaded " << models.size() << " models successfully" << std::endl;

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

        // Get the shader and activate it once before setting all uniforms
        try {
            Shader shader = ResourceManager::GetShader("model");
            shader.Use();

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
            
            // Set random point lights
            shader.SetInteger("numRandomPointLights", static_cast<int>(randomPointLights.size()));
            shader.SetInteger("useRandomPointLights", !randomPointLights.empty() ? 1 : 0);
            
            for (size_t i = 0; i < randomPointLights.size() && i < MAX_POINT_LIGHTS; i++) {
                std::string prefix = "randomPointLights[" + std::to_string(i) + "].";
                shader.SetVector3f((prefix + "position").c_str(), randomPointLights[i].position);
                shader.SetFloat((prefix + "constant").c_str(), randomPointLights[i].constant);
                shader.SetFloat((prefix + "linear").c_str(), randomPointLights[i].linear);
                shader.SetFloat((prefix + "quadratic").c_str(), randomPointLights[i].quadratic);
                shader.SetVector3f((prefix + "ambient").c_str(), randomPointLights[i].ambient);
                shader.SetVector3f((prefix + "diffuse").c_str(), randomPointLights[i].diffuse);
                shader.SetVector3f((prefix + "specular").c_str(), randomPointLights[i].specular);
            }
            
            // Set spot light properties
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
            
            // For normal mapping in vertex shader
            shader.SetVector3f("lightPos", pointLight.position);
            
            // Only log every 100 frames to avoid spamming the console
            static int frameCount = 0;
            if (frameCount % 100 == 0) {
                std::cout << "Rendering with point light position: (" 
                          << pointLight.position.x << ", " 
                          << pointLight.position.y << ", " 
                          << pointLight.position.z << ")" << std::endl;
                          
                std::cout << "Camera position: (" 
                          << camera.Position.x << ", " 
                          << camera.Position.y << ", " 
                          << camera.Position.z << ")" << std::endl;
            }
            frameCount++;

            // Pass projection matrix
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
            shader.SetMatrix4("projection", projection);

            // Camera/view transformation
            glm::mat4 view = camera.GetViewMatrix();
            shader.SetMatrix4("view", view);

            // Render ground
            renderGround(shader);
            
            // Render models
            for (size_t i = 0; i < models.size(); i++) {
                // Calculate model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, modelPositions[i]);
                model = glm::rotate(model, glm::radians(modelRotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(modelRotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(modelRotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, modelScales[i]);
                shader.SetMatrix4("model", model);
                
                // Draw the model
                try {
                    models[i]->Draw(shader);
                } catch (const std::exception& e) {
                    std::cout << "Error drawing model " << i << ": " << e.what() << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error in render loop: " << e.what() << std::endl;
        }
        
        // Render ImGui
        Gui::Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (auto model : models) {
        delete model;
    }
    models.clear();
    
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
    // Create a large ground plane
    float groundVertices[] = {
        // positions          // normals           // texture coords  // tangent                // bitangent
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 0.0f,      1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 50.0f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
         50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f,    50.0f, 50.0f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
        -50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f,    0.0f, 50.0f,      1.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f
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
        // Set window position and size
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("3D Scene Controls", &showModelsWindow, ImGuiWindowFlags_AlwaysAutoResize);
        
        // Add controls info
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Controls:");
        ImGui::Text("WASD - Move camera");
        ImGui::Text("QE - Move up/down");
        ImGui::Text("Enter - Toggle cursor mode");
        ImGui::Text("Escape - Exit");
        ImGui::Separator();
        
        ImGui::Text("Models loaded: %zu", models.size());
        ImGui::Separator();
        
        // Model selection
        if (models.size() > 0) {
            const char* modelNames[] = {"Backpack", "Mansion", "Tiptup", "Terry"};
            int maxModel = std::min(static_cast<int>(models.size()), static_cast<int>(IM_ARRAYSIZE(modelNames)));
            ImGui::Combo("Select Model", &selectedModel, modelNames, maxModel);
            
            if (selectedModel >= 0 && selectedModel < models.size()) {
                ImGui::Text("Model: %s", modelNames[selectedModel]);
                
                // Position controls
                ImGui::Text("Position");
                ImGui::SliderFloat("X##Pos", &modelPositions[selectedModel].x, -20.0f, 20.0f);
                ImGui::SliderFloat("Y##Pos", &modelPositions[selectedModel].y, -20.0f, 20.0f);
                ImGui::SliderFloat("Z##Pos", &modelPositions[selectedModel].z, -20.0f, 20.0f);
                
                // Rotation controls
                ImGui::Text("Rotation");
                ImGui::SliderFloat("X##Rot", &modelRotations[selectedModel].x, 0.0f, 360.0f);
                ImGui::SliderFloat("Y##Rot", &modelRotations[selectedModel].y, 0.0f, 360.0f);
                ImGui::SliderFloat("Z##Rot", &modelRotations[selectedModel].z, 0.0f, 360.0f);
                
                // Scale controls
                ImGui::Text("Scale");
                ImGui::SliderFloat("X##Scale", &modelScales[selectedModel].x, 0.1f, 5.0f);
                ImGui::SliderFloat("Y##Scale", &modelScales[selectedModel].y, 0.1f, 5.0f);
                ImGui::SliderFloat("Z##Scale", &modelScales[selectedModel].z, 0.1f, 5.0f);
            }
        }
        
        ImGui::End();
    }
}

static void renderLightingWindow() {
    ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 310, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Lighting Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Material settings
    if (ImGui::CollapsingHeader("Material Settings")) {
        ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);
        ImGui::Checkbox("Use Normal Maps", &useNormalMap);
        ImGui::Checkbox("Use Specular Maps", &useSpecularMap);
        ImGui::Checkbox("Use Detail Maps", &useDetailMap);
        ImGui::Checkbox("Use Scatter Maps", &useScatterMap);
    }
    
    // Light Brightness Adjustments
    if (ImGui::CollapsingHeader("Light Brightness")) {
        ImGui::SliderFloat("Point Light Brightness", &pointLightBrightness, 0.01f, 2.0f);
        ImGui::SliderFloat("Directional Light Brightness", &dirLightBrightness, 0.1f, 3.0f);
        ImGui::SliderFloat("Spot Light Brightness", &spotLightBrightness, 0.1f, 3.0f);
    }
    
    // Directional light
    if (ImGui::CollapsingHeader("Directional Light")) {
        ImGui::Checkbox("Enable##DirLight", &dirLight.enabled);
        ImGui::SliderFloat3("Direction", &dirLight.direction[0], -1.0f, 1.0f);
        ImGui::ColorEdit3("Ambient##Dir", &dirLight.ambient[0]);
        ImGui::ColorEdit3("Diffuse##Dir", &dirLight.diffuse[0]);
        ImGui::ColorEdit3("Specular##Dir", &dirLight.specular[0]);
    }
    
    // Main point light
    if (ImGui::CollapsingHeader("Main Point Light")) {
        ImGui::Checkbox("Enable##PointLight", &pointLight.enabled);
        ImGui::SliderFloat3("Position##Point", &pointLight.position[0], -20.0f, 20.0f);
        ImGui::ColorEdit3("Ambient##Point", &pointLight.ambient[0]);
        ImGui::ColorEdit3("Diffuse##Point", &pointLight.diffuse[0]);
        ImGui::ColorEdit3("Specular##Point", &pointLight.specular[0]);
        ImGui::SliderFloat("Constant", &pointLight.constant, 0.1f, 2.0f);
        ImGui::SliderFloat("Linear", &pointLight.linear, 0.001f, 0.5f);
        ImGui::SliderFloat("Quadratic", &pointLight.quadratic, 0.0001f, 0.1f);
    }
    
    // Random point lights
    if (ImGui::CollapsingHeader("Random Point Lights")) {
        static bool useRandomLights = !randomPointLights.empty();
        if (ImGui::Checkbox("Enable Random Lights", &useRandomLights)) {
            if (useRandomLights && randomPointLights.empty()) {
                generateRandomPointLights();
            } else if (!useRandomLights) {
                randomPointLights.clear();
            }
        }
        
        ImGui::Text("Random Lights: %zu", randomPointLights.size());
        
        if (ImGui::Button("Regenerate Random Lights")) {
            generateRandomPointLights();
        }
        
        ImGui::SliderFloat("Min Distance", &MIN_DISTANCE_BETWEEN_LIGHTS, 5.0f, 30.0f);
    }
    
    // Spot light
    if (ImGui::CollapsingHeader("Spot Light")) {
        ImGui::Checkbox("Enable##SpotLight", &spotLight.enabled);
        ImGui::SliderFloat3("Position##Spot", &spotLight.position[0], -20.0f, 20.0f);
        ImGui::SliderFloat3("Direction##Spot", &spotLight.direction[0], -1.0f, 1.0f);
        ImGui::ColorEdit3("Ambient##Spot", &spotLight.ambient[0]);
        ImGui::ColorEdit3("Diffuse##Spot", &spotLight.diffuse[0]);
        ImGui::ColorEdit3("Specular##Spot", &spotLight.specular[0]);
        
        // Convert cutOff angles from cosine to degrees for the UI
        float cutOffDegrees = glm::degrees(glm::acos(spotLight.cutOff));
        float outerCutOffDegrees = glm::degrees(glm::acos(spotLight.outerCutOff));
        
        if (ImGui::SliderFloat("Inner Cutoff", &cutOffDegrees, 0.0f, 90.0f)) {
            spotLight.cutOff = glm::cos(glm::radians(cutOffDegrees));
        }
        
        if (ImGui::SliderFloat("Outer Cutoff", &outerCutOffDegrees, 0.0f, 90.0f)) {
            spotLight.outerCutOff = glm::cos(glm::radians(outerCutOffDegrees));
        }
        
        ImGui::SliderFloat("Constant##Spot", &spotLight.constant, 0.1f, 2.0f);
        ImGui::SliderFloat("Linear##Spot", &spotLight.linear, 0.001f, 0.5f);
        ImGui::SliderFloat("Quadratic##Spot", &spotLight.quadratic, 0.0001f, 0.1f);
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