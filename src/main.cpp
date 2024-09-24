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
#include "math.h"
#include "Game/init2d.h"

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
Log logger("", "game.log", Log::Mode::OVERWRITE);

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, -4.0f, 0.0f),
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
// Vertices
// Define the vertices, UVs, normals, and indices
std::vector<math::vec3> verticest = {
    math::vec3(-0.5f, 0.0f, 0.0f), // A
    math::vec3(0.5f, 0.0f, 0.0f), // B
    math::vec3(0.0f, 0.5f, 0.0f)  // C
};

std::vector<math::vec2> uvst = {
    math::vec2(0.0f, 0.0f), // A
    math::vec2(1.0f, 0.0f), // B
    math::vec2(0.0f, 1.0f)  // C
};

std::vector<math::vec3> normalst = {
    math::vec3(0.0f, 0.0f, 1.0f), // A
    math::vec3(0.0f, 0.0f, 1.0f), // B
    math::vec3(0.0f, 0.0f, 1.0f)  // C
};

std::vector<unsigned int> indicest = {
    0, 1, 2 // Triangle
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
    float dark = clamp(t, cos, 0.9f, 1.0f);
    //shader.SetFloat("darkness",  dark);
    float radius = 0.8f;
    float angularSpeed = 0.4f;

    lightPos.x = radius * cos(angularSpeed * t);
    lightPos.y = radius * sin(angularSpeed * t);
    lightPos.z = radius * sin(angularSpeed * t * 0.2f);

    float pLinear = -clamp(t, sin, 1.0f, 380.f);
    float pQuadratic = -clamp(t, cos, 1.0f, 220.f);
    float constant = clamp(t, tan, 0.7f, 3.5f);
    shader.SetFloat("light.constant", constant);
    shader.SetFloat("light.linear", pow(2, pLinear));
    shader.SetFloat("light.quadratic", pow(2, pQuadratic));

    // Avoid undefined values for tan and sec:
    //float clampedTanInput = glm::clamp(t, 0.0f, glm::pi<float>()); // Clamp to [0, π]
    float co = clamp(t, tan, 10.0f, 40.f);

    shader.SetFloat("light.cutOff", glm::cos(glm::radians(co)));
    shader.SetFloat("light.outerCutOff", glm::cos(glm::radians(co + clamp(t, sin, 0.1f, 45.0f))));
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetVector3f("light.position",  camera.Position);
    shader.SetVector3f("light.direction", camera.Front);
    glm::vec3 lightColor = glm::vec3(dark, dark, dark);

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
        glm::vec3 ambientValue(0.5f, 0.5f, 0.5f); // All three components are the same
        glm::vec3 diffuseValue(0.7f, 0.7f, 0.7f); // All three components are the same
        glm::vec3 specularValue(clamp(t, sin), clamp(t, cot), clamp(time, sin, 0.6f, 1.0f));
        float stepsValue = 7.5f;

        shader.SetFloat("material.shininess", shininessValue);
        shader.SetVector3f("light.ambient", ambientValue);
        shader.SetVector3f("light.diffuse", diffuseValue);
        shader.SetVector3f("light.specular", specularValue);
        shader.SetFloat("steps", stepsValue);
        float time2 = canPrint ? 0.0f : time * 15.0f;
        glm::vec3 rot = glm::vec3(i % 3 == 0 || i < 4 ? time2 : 0.0f, i % 2 == 0 ? 0.3f * time2 : 0.0f, i == 5 || i == 7 ? time2 : 0.0f);
        glm::mat4 model = transform(cubePositions[i],
                                    std::max(0.1f * ((float)i + 0.3f), 1.0f),
                                    rot);
        //std::cout << model << i << "\n";
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
        Texture2D *diff = ResourceManager::GetTexture("diffuse");
        Texture2D *spec = ResourceManager::GetTexture("specular");
        draw(shader, textures2, 1, meshes[c], model, diff, spec);
        // Print transforms if needed
        if (canPrint)
        {
            logger.log("MODEL", model);
            logger.log("LIGHT-MODEL", lightModel);
            logger.log("VIEW", view);
            logger.log("PROJECTION", projection);
        }
    }
    Texture2D *textures = ResourceManager::GetTexture("white");
    Texture2D *rect = ResourceManager::GetTexture("rect");

    //Texture2D *diff = ResourceManager::GetTexture("diffuse");
    //Texture2D *spec = ResourceManager::GetTexture("specular");
    glm::mat4 cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                                    glm::vec3(0.01f, 0.01f, 500.0f), 0.0f);
    shader.SetMatrix4("model", cartesian);
    draw(shader, textures, 1, meshes[2], cartesian, textures, textures);
    cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                          glm::vec3(0.01f, 500.0f, 0.01f), 0.0f);
    shader.SetMatrix4("model", cartesian);
    draw(shader, textures, 1, meshes[2], cartesian, textures, textures);
    cartesian = transform(glm::vec3(0.0f, 0.0f, 0.0f),
                          glm::vec3(500.0f, 0.01f, 0.01f), 0.0f);
    shader.SetMatrix4("model", cartesian);
    draw(shader, textures, 1, meshes[2], cartesian, textures, textures);

    float st = sin(glfwGetTime());
    Texture2D *texture = ResourceManager::GetTexture("sky");
    glm::vec3 pos = glm::vec3(4.0f, 0.0f, -3.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f,st*45.0f);
    glm::mat4 cube = transform(pos, scale, rot);
    // Render the object
    draw(shader, texture, 1, meshes[2], cube, textures, textures);
    glCheckError(__FILE__, __LINE__);
    glm::mat4 tmat = glm::mat4(
        0.3f, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        5, 0, 0, 1
    );
    draw(shader, rect, 1, meshes[5], tmat, textures, textures);
    glBindVertexArray(0);
    // Draw the base triangle
    glm::mat4 baseTransform = glm::mat4(1.0f); // Identity matrix for the base
    draw(shader, rect, 1, meshes[5], baseTransform, textures, textures); // Draw base triangle

    // Draw the apex
    glm::mat4 apexTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Position apex
    draw(shader, rect, 1, meshes[5], apexTransform, textures, textures); // Draw apex

    // Draw edges connecting the apex to the base vertices
    for (int i = 0.0f; i < 3.0f; i += 0.2) {
        glm::mat4 edgeTransform = glm::mat4(1.0f); // Identity matrix for edge
        glm::vec3 baseVertex = (i == 0) ? glm::vec3(-0.5f, 0.0f, -0.5f) :
                                   (i == 1) ? glm::vec3(0.5f, 0.0f, -0.5f) :
                                       glm::vec3(0.0f, 0.0f, 0.5f);

        // Midpoint between the apex and the base vertex
        glm::vec3 midpoint = (baseVertex + glm::vec3(0.0f, i, 0.0f));
        edgeTransform = glm::scale(edgeTransform, glm::vec3(i,i,i));
        edgeTransform = glm::translate(edgeTransform, midpoint);

        draw(shader, rect, 1, meshes[5], edgeTransform, textures, textures); // Draw edge
    }
    canPrint = false;

    // Unbind the VAO

    // Swap buffers
    glfwSwapBuffers(window);
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

int main()
{
    if(!web){
        fs.root = std::string(logl_root);
    } else {
        fs.root = "";
    }
    logger.setDir(fs.root);
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
    ResourceManager::LoadTexture2D(fs.texture("rect.jpg"), "rect");
    ResourceManager::LoadTexture2D(fs.texture("glowstone.jpg"), "light");
    ResourceManager::LoadTexture2D(fs.texture("pumpkin.png"));
    ResourceManager::LoadTexture2D(fs.texture("sky.png"), "sky");
    ResourceManager::LoadTexture2D(fs.texture("maps/diffuse_container.png"), "diffuse");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-01-18-39-42_cf_m_face_00_Texture2.png"), "diffusex");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-18-18-28-39_cf_m_tang_DetailMask.png"), "diffusez");
    ResourceManager::LoadTexture2D(fs.texture("maps/specular_container.png"), "speculars");
    ResourceManager::LoadTexture2D(fs.texture("maps/_Export_2024-01-01-18-40-01_cf_m_face_00_NormalMask.png"), "specularf");
    ResourceManager::LoadTexture2D(fs.texture("maps/WaterBottle_specularGlossiness.png"), "specular");
    ResourceManager::LoadTexture2D(fs.texture("white.png"), "white");

    // Set up VAO, VBO, and EBO
    const float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
    float startAngleRad = 0.0f * DEG_TO_RAD;
    float endAngleRad = 360.0f * DEG_TO_RAD;

    // Cell::Cube mesh;
    Engine::Plane mesh(20, 20);
    // std::cout << Cell::HUEtoRGB(0.5f) << std::endl;
    Engine::Mesh trap(vertices, indices, uvs, normals);
    math::vec3 it = math::vec3();
    Engine::Mesh triang(verticest, indicest, uvst, normalst);
    triang.Finalize();
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
        &sphere,
        &triang
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
