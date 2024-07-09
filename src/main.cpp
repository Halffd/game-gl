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
Texture containerTexture, inTexture;
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

    unbindTextures();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 0);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // First container 
    float time = glfwGetTime() * 90.0f;
    glm::mat4 containerTransform1 = transform(0.0f, 1.0f, glm::vec3(time, time, time));

    Texture *textures = new Texture[2]{containerTexture, inTexture};
    draw(shader, textures, 2, vao, containerTransform1);

    // Print transforms if needed
    if (canPrint)
    {
        printTransform(containerTransform1);
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

    // Set up transformations
    aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    std::cout << aspect << " --> " << WIDTH << "/" << HEIGHT << std::endl;
    //    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    view = glm::mat4(1.0f); // No camera movement
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

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
