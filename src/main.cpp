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
#include <map>

#include "setup.h"
#include "root.h"

#include "vertex.h"
#include "Camera.hpp"
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

typedef std::map<std::string, Shader> shaderMap;
typedef std::map<std::string, Texture> textureMap;
typedef std::map<std::string, VAO> meshMap;

// Global variables
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
Texture containerTexture, inTexture;
glm::mat4 projection, view;
float aspect;
bool canPrint = true;
bool isPaused = false;
double lastTime = 0.0;
bool web = false;

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
    {-0.5f, 0.5f, 0.0f}, // Top left
    {0.5f, 0.5f, 0.0f},  // Top right
    {0.3f, -0.5f, 0.0f}, // Bottom right
    {-0.3f, -0.5f, 0.0f} // Bottom left
};

std::vector<math::vec2> uvs = {
    {0.0f, 1.0f}, // Top left
    {1.0f, 1.0f}, // Top right
    {0.6f, 0.0f}, // Bottom right
    {0.4f, 0.0f}  // Bottom left
};

// Index data for the trapezium
std::vector<uint32_t> indices = {
    0, 1, 2,
    0, 2, 3};
VAO vao;
VBO vbo;
EBO ebo;

bool input = true;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glCheckError(__FILE__, __LINE__);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
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

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float cameraSpeed = 2.5f * deltaTime;

 if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void renderScene(GLFWwindow *window, shaderMap shaders, std::vector<VAO *> &meshes, Texture *textures)
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    Shader shader = shaders["main"];
    shader.use();

    unbindTextures();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 0);
    if (input)
    {
        view = camera.GetViewMatrix();
    }
    else
    {
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    }
    // static float zoom = 0.2f;
    // static bool inc = true;
    // if(inc) zoom += 0.05f;
    // else zoom -= 0.05f;
    // if(zoom >= 10.0f || zoom <= 0.01f) inc = !inc;
    // view = glm::scale(view, glm::vec3(1.0f/zoom,1.0f/zoom,1.0f/zoom)); // Zoom 2x
    projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // First container
    // glBindVertexArray(VAO);
    float time1 = glfwGetTime();
    shader.setFloat("darkness",  (std::cos(time1) + 1.0f) * 0.5f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    shader.setVec3("lightColor",  lightColor);
    shader.setVec3("lightPos", lightPos);

    shaders["light"].use();
    shaders["light"].setMat4("view", view);
    shaders["light"].setMat4("projection", projection);
    shaders["light"].setVec3("lightColor",  lightColor);
    glm::mat4 lightModel = transform(lightPos, 0.2f, 1.0f);
    draw(shaders["light"], meshes[3], lightModel);

    for (unsigned int i = 0; i < 10; i++)
    {
        shader.use();
        float time = glfwGetTime();
        float time2 = canPrint ? 0.0f : time * 15.0f;
        glm::mat4 model = transform(cubePositions[i],
            std::max(0.1f * ((float)i + 0.3f), 1.0f),
            glm::vec3(time2, 0.3f * time2, 0.0f * i));
        std::cout << model << i << "\n";
        shader.setMat4("model", model);
        int c = i % 2 != 0 ? 0 : 2;
        int t = (i < 4) ? (i + 1) : (i < 7) ? (i + 4)
                                            : (i + 8);
        // Draw the mesh
        float r = std::max((std::sin(i * 0.5f) + 1.0f) * 0.5f, 0.1f);
        float g = std::max((std::cos(i * 0.7f) + 1.0f) * 0.5f, 0.1f);
        float b = std::max((std::sin(i * 0.9f) + 1.0f) * 0.5f, 0.1f);
        shader.setVec3("objectColor", r, g, b);
        shader.setFloat("mixColor", (float)std::cos(time) * 1.2f);
        Texture *textures2 = nullptr; //new Texture[2]{textures[t], textures[t + 1]};
        draw(shader, textures2, 0, meshes[c], model);
        // Print transforms if needed
        if (canPrint)
        {
            printTransform(model);
        }
    }

    unbindTextures();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 0);
    meshes[1]->bind();
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-0.8f, 0.7f, 0.7f));
    shader.setMat4("model", trans);
    glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, 0);
    glCheckError(__FILE__, __LINE__);
    meshes[1]->unbind();
    canPrint = false;

    // Unbind the VAO
    glBindVertexArray(0);
    glCheckError(__FILE__, __LINE__);

    // Swap buffers
    glfwSwapBuffers(window);
}

int main()
{
    if(!web){
        fs.root = std::string(logl_root);
    } else {
        fs.root = "";
    }
    std::cout << fs.root << "\n"
              << fs.file("src/main.cpp") << "\n";
    std::cout << fs.shader("") << "\n"
              << fs.root.empty() << "\n";
    // Initialize GLFW
    // return vectortest();
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    shaderMap shaders = {
        {"main",
            Shader(fs.shader("vertex.glsl"),
                fs.shader("fragment.glsl")
                )},
        {"light",
            Shader(fs.shader("light/vertex.glsl"),
                fs.shader("light/fragment.glsl")
                )}
    };
    std::cout << shaders["main"].ID << "\n";
    std::cout << shaders["light"].ID << "\n";
    shaders["main"].use();

    containerTexture.Load(fs.texture("container.jpg"));

    inTexture.Load(fs.texture("troll.png"));
    Texture bricks;
    bricks.Load(fs.texture("dbrick.png"));
    Texture mm;
    mm.Load(fs.texture("mm.png"));
    Texture sm(fs.texture("sm.png"));
    Texture sky(fs.texture("sky.png"));
    Texture *textures = new Texture[6]{
        bricks, inTexture, containerTexture, mm, sky, sm};

    // Set up VAO, VBO, and EBO
    const float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
    float startAngleRad = 0.0f * DEG_TO_RAD;
    float endAngleRad = 360.0f * DEG_TO_RAD;

    // Cell::Cube mesh;
    Cell::Plane mesh(20, 20);
    // std::cout << Cell::HUEtoRGB(0.5f) << std::endl;
    Cell::Mesh trap(vertices, indices, uvs);
    Cell::Cube cube;
    trap.Finalize();
    shaders["light"].use();
    Cell::Cube lightCube;
    shaders["main"].use();

    std::vector<VAO *> meshes = {
        &mesh,
        &trap,
        &cube,
        &lightCube};

    // Set up transformations
    aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    //    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

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
            renderScene(window, shaders, meshes, textures);
        }
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
