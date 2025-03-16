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
static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
static glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// Ground plane variables
unsigned int groundVAO = 0;
unsigned int groundVBO = 0;
unsigned int groundTexture = 0;

// Forward declare static functions
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);
static void renderModelsWindow();
static void setupGround();
static void renderGround(Shader &shader);
static bool toggleKey(int key, bool &toggleState);
static void toggleCursor(GLFWwindow *window);

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
    
    // Setup ground plane
    setupGround();
    
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
    
    // Load toxic can model
    try {
        std::string toxicCanPath = findModelPath("n64/Toxic Can/7398.obj");
        std::cout << "Attempting to load toxic can model from " << toxicCanPath << std::endl;
        
        // Check if file exists
        std::ifstream toxicCanFile(toxicCanPath);
        if (!toxicCanFile.good()) {
            std::cout << "Toxic can model file does not exist or cannot be opened!" << std::endl;
        } else {
            std::cout << "Toxic can model file exists and can be opened." << std::endl;
            toxicCanFile.close();
            
            models.push_back(new m3D::Model(toxicCanPath));
            std::cout << "Successfully loaded toxic can model" << std::endl;
            modelPositions.push_back(glm::vec3(10.0f, 0.0f, -5.0f)); // Position on ground
            modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            modelScales.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to load toxic can model: " << e.what() << std::endl;
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

        // Get the shader and activate it once before setting all uniforms
        try {
            Shader shader = ResourceManager::GetShader("model");
            shader.Use();

            // Set lighting properties
            shader.SetVector3f("lightPos", lightPos);
            shader.SetVector3f("viewPos", camera.Position);
            shader.SetVector3f("lightColor", lightColor);
            
            // Only log every 100 frames to avoid spamming the console
            static int frameCount = 0;
            if (frameCount % 100 == 0) {
                std::cout << "Rendering with light position: (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")" << std::endl;
                std::cout << "Light color: (" << lightColor.x << ", " << lightColor.y << ", " << lightColor.z << ")" << std::endl;
                std::cout << "Camera position: (" << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")" << std::endl;
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
        // positions          // normals           // texture coords
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 0.0f,
         50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f,
         
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f,
        -50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f
    };
    
    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Texture coords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    
    // Create a dark blue texture for the ground
    unsigned char groundTextureData[] = {
        10, 20, 80, 255,  // Dark blue color (RGBA)
        10, 20, 80, 255,
        10, 20, 80, 255,
        10, 20, 80, 255
    };
    
    // Generate and bind texture
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundTextureData);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void renderGround(Shader &shader) {
    // Set model matrix for ground
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);
    
    // Bind ground texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    
    // Set the texture uniform
    shader.SetInteger("texture_diffuse1", 0);
    
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
            const char* modelNames[] = {"Backpack", "Mansion", "Tiptup", "Terry", "Toxic Can"};
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
        
        ImGui::Separator();
        
        // Light controls
        ImGui::Text("Light");
        ImGui::SliderFloat("Light X", &lightPos.x, -20.0f, 20.0f);
        ImGui::SliderFloat("Light Y", &lightPos.y, -20.0f, 20.0f);
        ImGui::SliderFloat("Light Z", &lightPos.z, -20.0f, 20.0f);
        ImGui::ColorEdit3("Light Color", &lightColor[0]);
        
        ImGui::End();
    }
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