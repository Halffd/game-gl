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

// Custom printing function for glm::vec4
std::ostream &operator<<(std::ostream &os, const glm::vec4 &v)
{
    os << "(" << std::fixed << std::setprecision(2) << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

// Custom printing function for glm::mat4
std::ostream &operator<<(std::ostream &os, const glm::mat4 &m)
{
    os << std::fixed << std::setprecision(2) << std::endl;
    for (int i = 0; i < 4; ++i)
    {
        os << "[" << m[0][i] << ", " << m[1][i] << ", " << m[2][i] << ", " << m[3][i] << "]" << std::endl;
    }
    return os;
}
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
int vectortest()
{
    {
        // Initial matrix and vector
        glm::vec4 initialVec(1.0f, 0.0f, 0.0f, 1.0f);
        glm::mat4 initialTrans(0.5f);

        std::cout << "Initial matrix and vector:" << std::endl;
        std::cout << "vec: " << initialVec << std::endl;
        std::cout << "trans:\n"
                  << initialTrans << std::endl;

        // Translate the matrix
        glm::vec3 transVec(1.0f, 1.0f, 0.0f);
        glm::mat4 translatedTrans = glm::translate(initialTrans, transVec);

        std::cout << "After translation:" << std::endl;
        std::cout << "trans:\n"
                  << translatedTrans << std::endl;

        glm::vec4 translatedVec = translatedTrans * initialVec;
        std::cout << "vec: " << translatedVec << std::endl;
        std::cout << "vec.x: " << translatedVec.x << ", vec.y: " << translatedVec.y << ", vec.z: " << translatedVec.z << std::endl;
    }

    {
        // Initial matrix
        glm::mat4 initialTrans(1.0f);

        std::cout << "Initial matrix:" << std::endl;
        std::cout << "trans:\n"
                  << initialTrans << std::endl;

        // Rotate the matrix
        float rotateAngle = glm::radians(90.0f);
        glm::vec3 rotateAxis(0.0, 0.0, 1.0);
        glm::mat4 rotatedTrans = glm::rotate(initialTrans, rotateAngle, rotateAxis);

        std::cout << "After rotation:" << std::endl;
        std::cout << "trans:\n"
                  << rotatedTrans << std::endl;

        // Scale the matrix
        glm::vec3 scaleFactors(0.5, 0.5, 0.5);
        glm::mat4 scaledTrans = glm::scale(rotatedTrans, scaleFactors);

        std::cout << "After scaling:" << std::endl;
        std::cout << "trans:\n"
                  << scaledTrans << std::endl;
    }
    return 0;
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
    Texture backgroundTexture;
    backgroundTexture.Load(fs.texture("bg/Bathroom.png"));

    Texture characterTexture;
    characterTexture.Load(fs.texture("fg/sprite2.png"));

    Texture containerTexture;
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
    VAO vao;
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


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        containerTexture.Activate(GL_TEXTURE0);


        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // first container
        // ---------------
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // get their uniform location and set matrix (using glm::value_ptr)
        unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // with the uniform matrix set, draw the first container
        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // second transformation
        // ---------------------
        transform = glm::mat4(1.0f); // reset it to identity matrix
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value

        // now with the uniform matrix being replaced with new transformations, draw it again.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}