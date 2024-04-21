#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <string.h>
char *readFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return nullptr;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    if (content.empty())
    {
        std::cout << "Shader " << filePath << " is empty" << std::endl;
        return nullptr;
    }

    char *pContent = new char[content.size() + 1];
    strcpy(pContent, content.c_str());

    return pContent;
}
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Square", nullptr, nullptr);
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

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexShaderSource = readFile("vertex.glsl");
    std::cout << vertexShaderSource << "\n";
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    GLint success;
    GLchar infoLog[512];
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = readFile("fragment.glsl");
    std::cout << fragmentShaderSource << "\n";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // Create the shader program and link the shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // Delete the individual shaders as they are no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define the vertices of the square
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom-left vertex
        0.5f, -0.5f, 0.0f,  // Bottom-right vertex
        0.5f, 0.5f, 0.0f,   // Top-right vertex
        -0.5f, 0.5f, 0.0f   // Top-left vertex
    };

    // Define the indices of the square
    GLuint indices[] = {
        0, 1, 2,   // First triangle
        0, 2, 3    // Second triangle
    };

    // Createa Vertex Array Object (VAO) and Vertex Buffer Object (VBO) to store the square's vertex data
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind the VBO and copy the vertex data into it
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind the EBO and copy the index data into it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Process events
        glfwPollEvents();

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        GLint i = 0;
        glGetIntegerv(GL_POLYGON_MODE, &i);
        std::cout << i << "\n";
        glPolygonMode(GL_FRONT_AND_BACK, i == 6913 ? GL_FILL : GL_LINE);
         // Add a delay of 1000 milliseconds
            glfwWaitEventsTimeout(1);
        }
        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the square
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}