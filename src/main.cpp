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

#include "vertex.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Util.hpp"
#include "root_directory.h"

#include "plane.h"
#include "line_strip.h"
#include "sphere.h"
#include "cube.h"
#include "circle.h"
#include "quad.h"
#include "torus.h"
#include "ring.h"

// Global variables
Texture containerTexture, inTexture;
VAO vao;
glm::mat4 projection, view;
float aspect;
bool canPrint = true;
bool isPaused = false;
double lastTime = 0.0;
        // Index data for different primitive types
unsigned int pointIndices[] = {0, 1, 2, 3, 4, 5};
unsigned int lineIndices[] = {0, 1, 1, 2, 2, 0, 3, 4, 4, 5, 5, 3};
unsigned int lineStripIndices[] = {0, 1, 2};
unsigned int lineLoopIndices[] = {0, 1, 2, 0};
unsigned int triangleIndices[] = {0, 1, 2, 3, 4, 5};
unsigned int triangleStripIndices[] = {0, 1, 2, 3, 4, 5};
unsigned int triangleFanIndices[] = {3, 4, 0, 5, 1, 2};
glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)};

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
template<typename T>
void renderScene(GLFWwindow *window, Shader shader, T *mesh)
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    shader.use();

    unbindTextures();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 0);

    const float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); 
    float zoom = 0.3f;
    view = glm::scale(view, glm::vec3(1.0f/zoom,1.0f/zoom,1.0f/zoom)); // Zoom 2x

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // First container
    //glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10-9; i++)
    {
        float time = glfwGetTime() * 90.0f;
        glm::mat4 model = transform(cubePositions[i], std::max(0.1f*((float)i + 0.3f), 1.0f), glm::vec3(time * (float)(1+i)/4.4f, 0.3f * time / 3.0f * (float)(1+i), 0.1f * i));
        Texture *textures = new Texture[2]{containerTexture, inTexture};
        //draw(shader, textures, 2, mesh, model);
        // Print transforms if needed
        if (canPrint)
        {
            printTransform(model);
        }
    }
// Render points
draw(shader, &containerTexture, 1, mesh, glm::mat4(1.0f), 0, TRIANGLES);

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
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    Shader shader(fs.shader("shader.vs"), fs.shader("shader.fs"));

    containerTexture.Load(fs.texture("container.jpg"));

    inTexture.Load(fs.texture("troll.png"));

    // Define vertices for two quads (background and character)
    float vertices[] = {
        // Front face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // Back face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        // Left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f};
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 5, 6, 6, 7, 4,       // Back face
        8, 9, 10, 10, 11, 8,    // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Bottom face
        20, 21, 22, 22, 23, 20  // Top face
    };
    // Set up VAO, VBO, and EBO
    Cell::Ring mesh(25.0f, 25.0f, 7);

    shader.use();

    // Set up transformations
    aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    std::cout << aspect << " --> "  << WIDTH << "/" << HEIGHT << std::endl;
    //    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

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
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);
        if (!isPaused)
        {
            // Update your program state here
            renderScene(window, shader, &mesh);
        }
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
