#include "Game3D.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../ui/Gui.h"
#include "asset/ResourceManager.h"
#include "Timer.hpp"
#include <unistd.h>
#include <fstream>
#include "../scene/ModelComponent.h"
#include "../scene/TransformComponent.h"
#include "../render/primitives/curved/Sphere.h"

const unsigned SCREEN_WIDTH = 1280;
const unsigned SCREEN_HEIGHT = 720;

// callbacks
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

Game3D::Game3D()
    : camera(glm::vec3(0.0f, 20.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -33.68f),
      firstMouse(true),
      lastX(SCREEN_WIDTH / 2.0f),
      lastY(SCREEN_HEIGHT / 2.0f),
      deltaTime(0.0f),
      lastFrame(0.0f),
      cursorEnabled(true),
      projection(glm::mat4(1.0f)),
      showModelsWindow(false),
      selectedModel(0),
      useDynamicShapes(false),
      showDynamicShapesWindow(false),
      useSolarSystemScene(true),
      showCartesianPlane(false),
      showTriangleContours(false),
      runMode(true),
      baseMovementSpeed(2.5f),
      window(nullptr)
{
}

void Game3D::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Model Viewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glfwSetWindowUserPointer(window, this);
    
    auto mouse_callback_lambda = [](GLFWwindow* w, double x, double y){
        static_cast<Game3D*>(glfwGetWindowUserPointer(w))->mouse_callback(x, y);
    };
    glfwSetCursorPosCallback(window, mouse_callback_lambda);

    auto scroll_callback_lambda = [](GLFWwindow* w, double x, double y){
        static_cast<Game3D*>(glfwGetWindowUserPointer(w))->scroll_callback(x, y);
    };
    glfwSetScrollCallback(window, scroll_callback_lambda);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Gui::Init(window);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        ResourceManager::root = cwd;
    }

    ResourceManager::LoadShader("3d.vs", "3d.fs", nullptr, "model");
    ResourceManager::LoadShader("3d.vs", "outline.fs", nullptr, "outline");

    renderer.init();

    if (useSolarSystemScene) {
        initSolarSystemScene();
    } else {
        loadModels(std::string(cwd) + "/models", std::string(cwd) + "/bin/models");
    }
}

void Game3D::initSolarSystemScene() {
    std::cout << "Initializing Solar System Scene" << std::endl;

    // Create a sun (yellow sphere)
    auto sun = std::make_shared<m3D::Sphere>("Sun", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(2.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    scene.AddObject(sun);

    // Set sun as point light
    renderer.pointLight.enabled = true;
    renderer.pointLight.position = sun->position;
    renderer.pointLight.diffuse = glm::vec3(1.0f, 1.0f, 0.8f); // Yellowish light
    renderer.pointLight.specular = glm::vec3(1.0f, 1.0f, 0.8f);

    // Create Earth (blue sphere) orbiting the sun
    auto earth = std::make_shared<m3D::Sphere>("Earth", glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    scene.AddObject(earth);
}

void Game3D::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Timers::tick();
        processInput();

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        Gui::Start();

        // render UI windows
        
        scene.update(deltaTime);
        renderer.render(scene, camera);

        Gui::Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ResourceManager::Clear();
    Gui::Clean();
    glfwTerminate();
}

void Game3D::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool enterToggle = false;
    if (toggleKey(GLFW_KEY_ENTER, enterToggle)) {
        toggleCursor();
        std::cout << "Cursor mode toggled: " << (cursorEnabled ? "Enabled" : "Disabled") << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::CAMERA_RIGHT, deltaTime);
}

void Game3D::mouse_callback(double xposIn, double yposIn)
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

    if (!cursorEnabled)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void Game3D::scroll_callback(double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


bool Game3D::toggleKey(int key, bool &toggleState) {
    bool keyCurrentlyPressed = glfwGetKey(window, key) == GLFW_PRESS;

    if (!keyWasPressed[key] && keyCurrentlyPressed) {
        toggleState = !toggleState;
        keyWasPressed[key] = true;
        return true;
    } else if (keyWasPressed[key] && !keyCurrentlyPressed) {
        keyWasPressed[key] = false;
    }

    return false;
}

void Game3D::toggleCursor() {
    cursorEnabled = !cursorEnabled;
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    if (!cursorEnabled) {
        firstMouse = true;
    }
}


bool Game3D::loadModel(const std::string& name,
               const std::string& relativePath,
               const std::string& modelRoot,
               const std::string& binRoot,
               const glm::vec3& position,
               const glm::vec3& rotation,
               const glm::vec3& scale)
{
    auto findModelPath = [&](const std::string &relPath) -> std::string {
        std::string regular = modelRoot + "/" + relPath;
        std::string bin = binRoot + "/" + relPath;
        if (std::ifstream(regular).good()) return regular;
        if (std::ifstream(bin).good()) return bin;
        return regular; // fallback, will fail on open
    };

    try {
        std::string path = findModelPath(relativePath);
        std::ifstream file(path);
        if (!file.good()) {
            std::cout << "Model file not found or unreadable: " << path << std::endl;
            return false;
        }

        auto entity = std::make_unique<Entity>();
        auto model = new m3D::Model(path);
        entity->addComponent<ModelComponent>(model);
        entity->addComponent<TransformComponent>(position, rotation, scale);
        scene.addEntity(std::move(entity));

        modelNames.push_back(name);
        std::cout << "Loaded model: " << name << " from " << path << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cout << "Error loading model " << name << ": " << e.what() << std::endl;
        return false;
    }
}

void Game3D::loadModels(const std::string& modelBasePath, const std::string& binModelBasePath) {
    std::cout << "Starting to load models using Scene class..." << std::endl;

    struct ModelData {
        std::string name;
        std::string relPath;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    std::vector<ModelData> modelsToLoad = {
        {"Backpack", "backpack/backpack.obj", glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0), glm::vec3(1.0f)},
        {"Mansion", "low_poly_mansion__house/scene.gltf", glm::vec3(-10.0f, -0.5f, -10.0f), glm::vec3(0.0f, 245.0f, 0.0f), glm::vec3(0.00625f)},
        //{"Tiptup", "n64/Tiptup/ObjectTortRunner.obj", glm::vec3(13.0f, 0.0f, 5.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.5f)},
        {"Solar System", "solar_system_model_orrery/scene.gltf", glm::vec3(0.0f, 0.0f, 18.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f)},
        {"Model", "koseki_bijou/Model.glb", glm::vec3(-1.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f)},
//        {"ModelH", "hakos_baelz_3d_model_-_mmd_download/scene.gltf", glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)},
        {"ModelM", "hatsune_miku.glb", glm::vec3(-4.0f, 1.0f, 9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.4f)},
        {"Island", "GameCube - The Legend of Zelda The Wind Waker - Windfall Island Lenzos Shop/Lenzos Shop.obj", glm::vec3(8.0f, 0.0f, -1.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.2f)},
        {"Bird", "GameCube - The Legend of Zelda The Wind Waker - Medli/MediBody.dae", glm::vec3(3.0f, 8.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f)},
        {"Pirate", "GameCube - The Legend of Zelda The Wind Waker - Pirate Ship Interior/Pirate Ship Interior.dae", glm::vec3(6.0f, 2.0f, 9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.003f)},
    };

    for (const auto& m : modelsToLoad) {
        loadModel(m.name, m.relPath, modelBasePath, binModelBasePath, m.position, m.rotation, m.scale);
    }
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}