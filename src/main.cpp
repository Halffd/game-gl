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
#include "arc.h"

// Global variables
Texture containerTexture, inTexture;
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
// Vertex data for the trapezium
std::vector<math::vec3> vertices = {
    {-0.5f, 0.5f, 0.0f},  // Top left
    {0.5f, 0.5f, 0.0f},   // Top right
    {0.3f, -0.5f, 0.0f},  // Bottom right
    {-0.3f, -0.5f, 0.0f}  // Bottom left
};

std::vector<math::vec2> uvs = {
    {0.0f, 1.0f},  // Top left
    {1.0f, 1.0f},  // Top right
    {0.6f, 0.0f},  // Bottom right
    {0.4f, 0.0f}   // Bottom left
};

// Index data for the trapezium
std::vector<uint32_t> indices = {
    0, 1, 2,
    0, 2, 3
};
VAO vao;
VBO vbo;
EBO ebo;

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
void renderScene(GLFWwindow *window, Shader shader, std::vector<VAO*>& meshes)
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
    //static float zoom = 0.2f;
    //static bool inc = true;
    //if(inc) zoom += 0.05f;
    //else zoom -= 0.05f;
    //if(zoom >= 10.0f || zoom <= 0.01f) inc = !inc;
    //view = glm::scale(view, glm::vec3(1.0f/zoom,1.0f/zoom,1.0f/zoom)); // Zoom 2x

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // First container
    //glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        float time = glfwGetTime() * 90.0f;
        glm::mat4 model = transform(cubePositions[i], std::max(0.1f*((float)i + 0.3f), 1.0f), glm::vec3(time * (float)(1+i)/4.4f, 0.3f * time / 3.0f * (float)(1+i), 0.1f * i));
        Texture *textures = new Texture[2]{containerTexture, inTexture};
        draw(shader, textures, 2, meshes[0], model);
        // Print transforms if needed
        if (canPrint)
        {
            printTransform(model);
        }
    }
meshes[1]->bind();
glm::mat4 trans = glm::mat4(1.0f);
shader.setMat4("model", trans);
glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, 0);
meshes[1]->unbind();
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

    // Set up VAO, VBO, and EBO
    const float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
float startAngleRad = 0.0f * DEG_TO_RAD;
float endAngleRad = 360.0f * DEG_TO_RAD;

//Cell::Cube mesh;
Cell::Plane mesh(20,20);
//std::cout << Cell::HUEtoRGB(0.5f) << std::endl;
Cell::Mesh trap(vertices, indices, uvs);
trap.Finalize();
    shader.use();
    
std::vector<VAO*> meshes = {
    &mesh,
    &trap
};


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
            renderScene(window, shader, meshes);
        }
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
