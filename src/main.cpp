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
#include "ResourceManager.h"
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
///#include "Game/Game.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Global variables
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
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
    // Front face
    {-0.5f, 0.5f, 0.5f}, // Top left
    {0.5f, 0.5f, 0.5f},  // Top right
    {0.3f, -0.5f, 0.5f}, // Bottom right
    {-0.3f, -0.5f, 0.5f}, // Bottom left

    // Back face
    {-0.5f, 0.5f, -0.5f}, // Top left
    {0.5f, 0.5f, -0.5f},  // Top right
    {0.3f, -0.5f, -0.5f}, // Bottom right
    {-0.3f, -0.5f, -0.5f}, // Bottom left
};

std::vector<math::vec2> uvs = {
    // Front face
    {0.0f, 1.0f}, // Top left
    {1.0f, 1.0f}, // Top right
    {0.6f, 0.0f}, // Bottom right
    {0.4f, 0.0f}, // Bottom left

    // Back face
    {0.0f, 1.0f}, // Top left
    {1.0f, 1.0f}, // Top right
    {0.6f, 0.0f}, // Bottom right
    {0.4f, 0.0f}  // Bottom left
};

std::vector<uint32_t> indices = {
    // Front face
    0, 1, 2,
    0, 2, 3,

    // Right face
    1, 5, 6,
    1, 6, 2,

    // Back face
    5, 4, 7,
    5, 7, 6,

    // Left face
    4, 0, 3,
    4, 3, 7,

    // Top face
    4, 5, 1,
    4, 1, 0,

    // Bottom face
    3, 2, 6,
    3, 6, 7
};
std::vector<math::vec3> normals = {
    // Front face
    {0.0f, 0.0f, 1.0f}, // Top left
    {0.0f, 0.0f, 1.0f}, // Top right
    {0.0f, 0.0f, 1.0f}, // Bottom right
    {0.0f, 0.0f, 1.0f}, // Bottom left

    // Back face
    {0.0f, 0.0f, -1.0f}, // Top left
    {0.0f, 0.0f, -1.0f}, // Top right
    {0.0f, 0.0f, -1.0f}, // Bottom right
    {0.0f, 0.0f, -1.0f}  // Bottom left
};
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

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int game(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    Breakout.Init();

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        Breakout.ProcessInput(deltaTime);

        // update game state
        // -----------------
        Breakout.Update(deltaTime);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = false;
    }
}
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
void renderScene(GLFWwindow *window, std::vector<VAO *> &meshes)
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    Shader shader = ResourceManager::GetShader("main");
    shader.Use();
    
    shader.SetInteger("texture1", 0);
    shader.SetInteger("texture2", 0);
    shader.SetInteger("material.diffuse", 0);

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

    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
    // First container
    // glBindVertexArray(VAO);
    float t = glfwGetTime();
    float dark = clamp(t, cos, 0.5f, 1.0f);
    shader.SetFloat("darkness",  dark);
    glm::vec3 lightColor = glm::vec3(dark, dark, dark);
    shader.SetVector3f("lightColor",  lightColor);
    float radius = 3.0f;
    float angularSpeed = 0.5f;

    lightPos.x = radius * cos(angularSpeed * t);
    lightPos.y = radius * sin(angularSpeed * t);
    lightPos.z = radius * sin(angularSpeed * t * 0.5f);
    shader.SetVector3f("lightPos", lightPos);
    shader.SetVector3f("viewPos", camera.Position);

    Shader light = ResourceManager::GetShader("light");
    light.Use();
    light.SetMatrix4("view", view);
    light.SetMatrix4("projection", projection);
    light.SetVector3f("lightColor",  lightColor);
    glm::mat4 lightModel = transform(lightPos, 0.2f, 1.0f);
    Texture2D* lightTex = new Texture2D[1] {
        ResourceManager::GetTexture2D("light")
    };
    draw(light, lightTex, 1, meshes[3], lightModel);

    for (unsigned int i = 0; i < 10; i++)
    {
        shader.Use();
        float time = glfwGetTime();
        float targetValue = 1.0f;
        float interpolatedValue = lerp(0.0f, targetValue, time);

        float shininessValue = 1024.0f;
        float ambientValue = 0.5f; //clamp(time, 0.05f, 0.68f);
        float diffuseValue1 = 0.3f; //clamp(time * 1.7f, 0.0f, 1.0f);
        float diffuseValue2 = clamp(time * 0.5f, 0.0f, 1.0f);
        float diffuseValue3 = 0.3f; //clamp(time * 0.7f, 0.0f, 1.0f);
        float specularLightValue1 = 0.25; //clamp(time, 0.5f, 1.0f);
        float specularLightValue2 = clamp(time * 0.3f, 0.0f, 1.0f);
        float stepsValue = 7.5f;

        shader.SetFloat("material.shininess", shininessValue);
        shader.SetVector3f("light.ambient", ambientValue, 0.28f, 0.3f);
        shader.SetVector3f("light.diffuse", diffuseValue1, diffuseValue2, diffuseValue3);
        shader.SetVector3f("light.specular", specularLightValue1, specularLightValue2, cos(time));
        shader.SetFloat("steps", stepsValue);    float time2 = canPrint ? 0.0f : time * 15.0f;
        glm::vec3 rot = glm::vec3(i % 3 == 0 || i < 4 ? time2 : 0.0f, i % 2 == 0 ? 0.3f * time2 : 0.0f, i == 5 || i == 7 ? time2 : 0.0f);
        glm::mat4 model = transform(cubePositions[i],
            std::max(0.1f * ((float)i + 0.3f), 1.0f),
            rot);
        std::cout << model << i << "\n";
        shader.SetMatrix4("model", model);
        int c = i % 2 != 0 ? i % 3 == 0 ? 4 : 0 : 2;
        int t = i; //5 - (int)(i / 2);
        // Draw the mesh
        float r = std::max((std::sin(i * 0.5f) + 1.0f) * 0.5f, 0.1f);
        float g = std::max((std::cos(i * 0.7f) + 1.0f) * 0.5f, 0.1f);
        float b = std::max((std::sin(i * 0.9f) + 1.0f) * 0.5f, 0.1f);
        //shader.SetVector3f("objectColor", r, g, b);
        shader.SetFloat("mixColor", 0.0f);
        Texture2D *textures2 = // ResourceManager::GetTexture("diffuse");
            ResourceManager::GetTexture2DByIndex(t);
        Texture2D *diff = ResourceManager::GetTexture("diffusex");
        Texture2D *spec = ResourceManager::GetTexture("specular");
        draw(shader, textures2, 1, meshes[c], model, diff, spec);
        // Print transforms if needed
        if (canPrint)
        {
            printTransform(model);
        }
    }
    /*
    meshes[1]->bind();
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-0.8f, 0.7f, 0.7f));
    shader.SetMatrix4("model", trans);
    glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, 0);
    glCheckError(__FILE__, __LINE__);
    meshes[1]->unbind();*/
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


    Shader shader = ResourceManager::LoadShader(fs.shader("vertex.glsl") , fs.shader("fragment.glsl"), "main");
    Shader light = ResourceManager::LoadShader(fs.shader("light/vertex.glsl"), fs.shader("light/fragment.glsl"), "light");

    std::cout << shader.ID << "\n";
    std::cout << light.ID << "\n";
    shader.Use();

    ResourceManager::LoadTexture2D(fs.texture("bookshelf.jpg"), "bookshelf");
    ResourceManager::LoadTexture2D(fs.texture("copper.png"));
    ResourceManager::LoadTexture2D(fs.texture("yellowstone.jpg"));
    ResourceManager::LoadTexture2D(fs.texture("dbricks.png"));
    ResourceManager::LoadTexture2D(fs.texture("container.jpg"), "main");
    ResourceManager::LoadTexture2D(fs.texture("glowstone.jpg"), "light");
    ResourceManager::LoadTexture2D(fs.texture("pumpkin.png"));
    ResourceManager::LoadTexture2D(fs.texture("maps/diffuse_container.png"), "diffused");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-01-18-39-42_cf_m_face_00_Texture2.png"), "diffusex");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-18-18-28-39_cf_m_tang_DetailMask.png"), "diffuse");
    ResourceManager::LoadTexture2D(fs.texture("maps/specular_container.png"), "speculars");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-01-18-40-01_cf_m_face_00_NormalMask.png"), "specular");
    ResourceManager::LoadTexture2D(fs.texture("maps/WaterBottle_specularGlossiness.png"), "speculard");


    // Set up VAO, VBO, and EBO
    const float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
    float startAngleRad = 0.0f * DEG_TO_RAD;
    float endAngleRad = 360.0f * DEG_TO_RAD;

    // Cell::Cube mesh;
    Engine::Plane mesh(20, 20);
    // std::cout << Cell::HUEtoRGB(0.5f) << std::endl;
    Engine::Mesh trap(vertices, indices, uvs, normals);
    Engine::Cube cube;
    trap.Finalize();
    light.Use();
    Engine::Cube lightCube;
    shader.Use();
    Engine::Sphere sphere(30, 30);

    std::vector<VAO *> meshes = {
        &mesh,
        &trap,
        &cube,
        &lightCube,
        &sphere
    };

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
            renderScene(window, meshes);
        }
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
