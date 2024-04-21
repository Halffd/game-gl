#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <string.h>

float verticesTriangle[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f};
float vertices[] = {
    0.5f, 0.5f, 0.0f,   // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f   // top left
};
unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource1 = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.05, 0.9, 1.0);
}
)";

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
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}
GLuint createAndBindVAO(GLuint vbo, GLfloat *vertices, GLsizei vertexSize, GLuint attributeIndex, GLint attributeSize, GLenum attributeType, GLboolean attributeNormalized = GL_FALSE, GLsizei attributeStride = 0, const void *attributePointer = nullptr)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(attributeIndex, attributeSize, attributeType, attributeNormalized, attributeStride, attributePointer);
    glEnableVertexAttribArray(attributeIndex);

    glBindVertexArray(0);

    return vao;
}
int main()
{
    // Initialize GLFW
    printf("Start\n");
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current directory: " << currentPath << std::endl;

    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // Set the viewport size
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = readFile("fragment.glsl");
    std::cout << fragmentShaderSource << "\n";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::FAILED\n"
                  << infoLog << "\n";
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // Create and bind a vertex array object (VAO)
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

unsigned int EBO;
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

GLuint VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering commands
        //glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers(window);

        // Poll for events
        glfwPollEvents();
    }

    // Clean up
    glfwTerminate();
    return 0;
}