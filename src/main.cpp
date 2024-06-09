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
#include "setup.h"    
//#include "root.h"

std::string root; // Declare the root variable as extern
#define TEXTURE_DIR "textures"
#define SHADER_DIR "shaders"

#define APPEND_DIR(dir, filename) (std::string(root) + "/" + std::string(dir) + "/" + std::string(filename)).c_str()

#define SHADER(filename) APPEND_DIR(SHADER_DIR, filename)
#define TEXTURE(filename) APPEND_DIR(TEXTURE_DIR, filename)

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Util.hpp"
#include "root_directory.h"

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
    root = std::string(logl_root); //GetParentDirectory();
    std::cout << root << "\n";
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

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;

    Shader shader(SHADER("shader.vs"), SHADER("shader.fs"));
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

    std::vector<uint32_t> indices = {  
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

    Texture texture1, texture2;
    bool res = texture1.Load(TEXTURE("container.jpg"));
    if(!res){
        std::cout << "Error in Texture1\n";
    }
    res = texture2.Load(TEXTURE("troll.png"));
    if(!res){
        std::cout << "Error in Texture2\n";
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    // or set it via the texture class
    shader.setInt("texture2", 1);

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

        // bind textures on corresponding texture units
        texture1.Activate(GL_TEXTURE0);
        texture2.Activate(GL_TEXTURE1);

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