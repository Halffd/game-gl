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
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "setup.h"
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

    // Set the viewport size and define the resize callback
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    Shader shader("shaders/shader.vs", "shaders/shader.fs");
    VAO vao;
    vao.bind();

    std::vector<float> triangle = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    std::vector<float> vertices = {
        // positions          // colors           // texture coords
        0.8f,  0.8f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    float texCoords[] = {
        0.0f, 0.0f, // lower-left corner
        1.0f, 0.0f, // lower-right corner
        0.5f, 1.0f  // top-center corner
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    VBO vbo;    
    vbo.genBuffer();
    vbo.bind();
    vbo.setup(vertices);
    EBO ebo;
    ebo.genBuffer();
    ebo.setup(indices);

    vao.set(0, 3, 8 * sizeof(float), (void*)0);
    // color attribute
    vao.set(1, 3, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // texture coord attribute
    vao.set(2, 2, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   
    vao.unbind();
    vbo.unbind();

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    {
        int width, height, nrChannels;
        unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
        //std::cout << "Data: " << data << "\n";
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
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
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.use();

        // float timeValue = glfwGetTime();
        // float colorValue = (std::sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = shader.get("uniformColor");
        // std::cout << vertexColorLocation << "\n";
        // std::vector<float> color = {colorValue, colorValue, colorValue, 1.0f};
        // float mixFactor = 0.1f + (std::cos(timeValue) / 2.9f + 0.5f) * 0.85f;
        // shader.setFloat("mixFactor", mixFactor);
        // shader.setFloat("uniformColor", color);
        // shader.setFloat("offset", pos);
        // shader.setFloat("scale", pos);
        // glUniform4f(vertexColorLocation, color[0], color[1], color[2], color[3]);
        //  Bind the VAO

        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
        vao.unbind();

        // Unbind the VAO
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return 0;
}