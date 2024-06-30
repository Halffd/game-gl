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

#include "setup.h"
#include "root.h"

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Util.hpp"
#include "root_directory.h"

// Global variables
Texture backgroundTexture, characterTexture, containerTexture;
VAO vao;
glm::mat4 projection, view;
float aspect;
bool canPrint = true;
bool isPaused = false;
double lastTime = 0.0;

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    glfwPollEvents();
    
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
    static bool isLineMode = false;         // Track the current polygon mode
    static bool spaceKeyWasPressed = false; // Flag to track if space key was pressed

    static bool spacePressed;
    bool spaceCurrentlyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (!spacePressed && spaceCurrentlyPressed)
    {
        // Space key was just pressed
        spaceKeyWasPressed = true; // Set the flag
        spacePressed = true;
    }
    else if (spacePressed && !spaceCurrentlyPressed)
    {
        // Space key was just released
        spacePressed = false;
    }
    if (spaceKeyWasPressed)
    {
        // Toggle polygon mode only once after the space key is pressed
        isLineMode = !isLineMode;
        glPolygonMode(GL_FRONT_AND_BACK, isLineMode ? GL_LINE : GL_FILL);
        spaceKeyWasPressed = false; // Reset the flag
    }
}
void renderScene(GLFWwindow *window, Shader shader)
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    shader.use();
    shader.setInt("ourTexture", 0);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Background transformation
    glm::mat4 backgroundTransform = transform(
        glm::vec3(0.0f, 0.0f, 0.0f),          // Center at (0,0) due to orthographic projection
        glm::vec3(2.0f * aspect, 2.0f, 1.0f), // Scale to cover the entire screen
        glm::vec3(0.0f));
    draw(shader, backgroundTexture, vao, backgroundTransform);

    // Character transformation
    float desiredCoverage = 0.75f; // Cover 75% of the screen height
    float screenHeight = 2.0f;     // Normalized screen height in orthographic projection
    float desiredCharacterHeight = screenHeight * desiredCoverage;
    float characterHeight = characterTexture.height; // Actual height of the character image
    float characterWidth = characterTexture.width;   // Actual width of the character image
    float characterScale = desiredCharacterHeight;

    // Calculate position, scale, and rotation
    glm::vec3 position = glm::vec3(0.0f, -1.0f + characterScale / 2, 0.0f);
    glm::vec3 scale = glm::vec3(characterScale * (characterWidth / characterHeight), characterScale, 1.0f); // Scale proportionally to aspect ratio
    glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(static_cast<float>(sin(glfwGetTime())) * 90.0f)));

    glm::mat4 characterTransform = transform(position, scale, rotation);
    draw(shader, characterTexture, vao, characterTransform);

    // First container transformation
    glm::mat4 containerTransform1 = transform(
        glm::vec3(-aspect + (2.0f * aspect / 2.5f), 0.5f, 0.0f), // Adjusted position
        glm::vec3(2.0f * aspect / 3.0f, 2.0f / 3.0f, 1.0f),      // Adjusted scale
        glm::vec3(0.0f, 0.0f, glm::degrees((float)glfwGetTime())));
    draw(shader, containerTexture, vao, containerTransform1);

    // Second container transformation
    float scaleAmount = static_cast<float>(sin(glfwGetTime())) * 700.0f / std::max(WIDTH, HEIGHT); // Adjust scaling
    glm::mat4 containerTransform2 = glm::mat4(1.0f);
    containerTransform2 = glm::translate(containerTransform2, glm::vec3(-aspect + 2.0f, 0.6f, 0.0f)); // Adjusted position
    containerTransform2 = glm::scale(containerTransform2, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
    draw(shader, containerTexture, vao, containerTransform2);

    // Print transforms if needed
    if (canPrint)
    {
        std::cout << "Background: " << backgroundTransform << std::endl;
        std::cout << "Character: " << characterTransform << std::endl;
        printTransform(characterTransform);
        printTransform(containerTransform1);
        printTransform(containerTransform2);
    }
    canPrint = false;

    // Unbind the VAO
    glBindVertexArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
}

int main()
{
    fs.root = std::string(logl_root); // GetParentDirectory();
    std::cout << fs.root << "\n"
              << fs.file("src/main.cpp") << "\n";
    // Initialize GLFW
    // return vectortest();
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Square", nullptr, nullptr);
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    Shader shader(fs.shader("shader.vs"), fs.shader("shader.fs"));

    // Load textures
    backgroundTexture.Load(fs.texture("bg/Bathroom.png"));

    characterTexture.Load(fs.texture("fg/sprite2.png"));

    containerTexture.Load(fs.texture("container.jpg"));

    // Define vertices for two quads (background and character)
    float vertices[] = {
        // positions          // texture coords
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f    // top right
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

    // Set up VAO, VBO, and EBO
    VBO vbo;
    EBO ebo;

    vao.genVertexArray();
    vbo.genBuffer();
    ebo.genBuffer();

    vao.bind();
    vbo.bind();
    vbo.setup(vertices, sizeof(vertices));

    ebo.bind();
    ebo.setup(indices, sizeof(indices));

    // Set vertex attribute pointers
    vao.linkAttrib(vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    vao.unbind();
    vbo.unbind();
    ebo.unbind();

    shader.use();
    shader.setInt("ourTexture", 0);

    // Set up transformations
    aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    std::cout << aspect << " --> " << WIDTH << "/" << HEIGHT << std::endl;
    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    view = glm::mat4(1.0f); // No camera movement

    double nextRenderTime = 0.0; // Store the time when the next render should occur
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);
        if (!isPaused)
        {
            // Update your program state here
            renderScene(window, shader);
        }
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
