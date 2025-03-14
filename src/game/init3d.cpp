#include "init3d.h"
#include "../ui/Gui.h"
#include <setup.h>
#include "../Transform.hpp"
#include "../render/Model.h"
#include "../include/Camera.hpp"  // Include the 3D camera header
#include <vector>
#include <string>

// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH;
const unsigned SCREEN_HEIGHT = HEIGHT;

// Create a Camera instance from Camera.hpp
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
extern bool firstMouse;
extern float lastX;
extern float lastY;
extern float deltaTime;
extern float lastFrame;

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

int game3d(int argc, char *argv[], const std::string& type) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);

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

    // Load shaders
    ResourceManager::LoadShader("shaders/3d.vs", "shaders/3d.fs", nullptr, "model");
    
    // Setup ground plane
    setupGround();
    
    // Load models
    try {
        // Load backpack model
        models.push_back(new m3D::Model("models/backpack/backpack.obj"));
        modelPositions.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelScales.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Load mansion model
        models.push_back(new m3D::Model("models/low_poly_mansion__house/scene.gltf"));
        modelPositions.push_back(glm::vec3(5.0f, 0.0f, 0.0f));
        modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelScales.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
        
        // Load N64 Tiptup model
        models.push_back(new m3D::Model("models/n64/Tiptup/ObjectTortRunner.obj"));
        modelPositions.push_back(glm::vec3(-5.0f, 0.5f, 0.0f));
        modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelScales.push_back(glm::vec3(0.1f, 0.1f, 0.1f));
        
        // Load N64 Terry model
        models.push_back(new m3D::Model("models/n64/Terry/ObjectTerryboss.obj"));
        modelPositions.push_back(glm::vec3(0.0f, 0.5f, 5.0f));
        modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelScales.push_back(glm::vec3(0.1f, 0.1f, 0.1f));
        
        // Load N64 Toxic Can model
        models.push_back(new m3D::Model("models/n64/Toxic Can/7398.obj"));
        modelPositions.push_back(glm::vec3(0.0f, 0.5f, -5.0f));
        modelRotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelScales.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
        
        std::cout << "Successfully loaded " << models.size() << " models" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading models: " << e.what() << std::endl;
    }

    // Get shader
    Shader shader = ResourceManager::GetShader("model");
    shader.Use();

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

        shader.Use();

        // Set lighting properties
        shader.SetVector3f("lightPos", lightPos);
        shader.SetVector3f("viewPos", camera.Position);
        shader.SetVector3f("lightColor", lightColor);

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
            
            // Draw model
            models[i]->Draw(shader);
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
    
    // Create a simple blue texture for the ground
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
    
    // Draw ground
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void renderModelsWindow() {
    if (showModelsWindow) {
        ImGui::Begin("3D Models", &showModelsWindow);
        
        ImGui::Text("Models loaded: %zu", models.size());
        ImGui::Separator();
        
        // Model selection
        const char* modelNames[] = {"Backpack", "Mansion", "Tiptup", "Terry", "Toxic Can"};
        ImGui::Combo("Select Model", &selectedModel, modelNames, IM_ARRAYSIZE(modelNames));
        
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

    // Only process mouse movement if right mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
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
    
    // Additional camera controls
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.Position.y += camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.Position.y -= camera.MovementSpeed * deltaTime;
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} 