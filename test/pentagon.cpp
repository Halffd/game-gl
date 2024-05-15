#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Util.hpp"

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    static bool spacePressed;

    // in handling
    static bool spaceCurrentlyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (!spacePressed && spaceCurrentlyPressed)
    { // wasn't before, is now
        GLint i = 0;
        glGetIntegerv(GL_POLYGON_MODE, &i);
        // std::cout << i << "\n";
        glPolygonMode(GL_FRONT_AND_BACK, i == 6913 ? GL_FILL : GL_LINE);
        // Add a delay of 100 milliseconds
        glfwWaitEventsTimeout(0.1);
    }
    spacePressed = spaceCurrentlyPressed;
}
int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Square", nullptr, nullptr);
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

    // Set the viewport size and define the resize callback
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    Shader shader("shader.vs", "shader.fs");
    VAO vao;
    vao.bind();

    std::vector<float> vertices = {
        // Positions       // Colors
        0.0f, 0.8f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top vertex (red)
        -0.5f, 0.2f, 0.0f,  0.0f, 1.0f, 0.0f, // Upper-left vertex (green)
        -0.3f, -0.4f, 0.0f,  0.0f, 0.0f, 1.0f, // Lower-left vertex (blue)
        0.3f, -0.4f, 0.0f,  1.0f, 1.0f, 0.0f,  // Lower-right vertex (yellow)
        0.5f, 0.2f, 0.0f,  1.0f, 0.0f, 1.0f   // Upper-right vertex (magenta)
    };

    VBO vbo;
    vbo.genBuffer();
    vbo.bind();
    vbo.setup(vertices);

    GLuint positionIndex = 0;
    GLint positionSize = 3;
    GLenum positionType = GL_FLOAT;
    GLboolean positionNormalized = GL_FALSE;
    GLsizei positionStride = 6 * sizeof(GLfloat);
    const void* positionPointer = (void*)0;
    vao.setVertexAttribPointer(positionIndex, positionSize, positionType, positionNormalized, positionStride, positionPointer);

    GLuint colorIndex = 1;
    GLint colorSize = 3;
    GLenum colorType = GL_FLOAT;
    GLboolean colorNormalized = GL_FALSE;
    GLsizei colorStride = 6 * sizeof(GLfloat);
    const void* colorPointer = (void*)(3 * sizeof(GLfloat));
    vao.setVertexAttribPointer(colorIndex, colorSize, colorType, colorNormalized, colorStride, colorPointer);

    vao.unbind();
    vbo.unbind();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Process events
        glfwPollEvents();

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        shader.use();
        float timeValue = glfwGetTime();
        float colorValue = (std::sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = shader.get("uniformColor");
        //std::cout << vertexColorLocation << "\n";
        std::vector<float> color = {colorValue, colorValue, colorValue, 1.0f};
        float mixFactor = 0.1f + (std::cos(timeValue) / 2.9f + 0.5f) * 0.85f;
        shader.setFloat("mixFactor", mixFactor);
        shader.setFloat("uniformColor", color);
        //glUniform4f(vertexColorLocation, color[0], color[1], color[2], color[3]);
        // Bind the VAO
        
        vao.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 6);
        vao.unbind();

        // Unbind the VAO
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return 0;
}