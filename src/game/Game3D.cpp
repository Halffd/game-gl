#include "Game3D.h"
#include <ctime>
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
#include "render/primitives/basic/Cube.h"

#include <random>

const unsigned SCREEN_WIDTH = 1600;
const unsigned SCREEN_HEIGHT = 900;

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
      baseMovementSpeed(6.5f),
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
        std::cerr << "Failed to create GLFW window" << std::endl;
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
    if(!m_framebuffer) {
        m_framebuffer = std::make_shared<Framebuffer>();
    }
    if(!m_screenQuad) {
        m_screenQuad = std::make_shared<VO::Quad>();
    }
    Gui::Init(window);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        ResourceManager::root = cwd;
    }
    std::cout << "Root directory: " << ResourceManager::root << std::endl;
    std::cout << "Loading shader: model" << std::endl;
    ResourceManager::LoadShader("3d.vs", "3d.fs", nullptr, "model");
    std::cout << "Creating framebuffer" << std::endl;
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    int fbWidth = 400;
    int fbHeight = fbWidth / aspectRatio;
    m_framebufferSize = glm::vec2(fbWidth, fbHeight);
    m_framebuffer->create(m_framebufferSize.x, m_framebufferSize.y);
    std::cout << "Loading shader: fb" << std::endl;
    ResourceManager::LoadShader("fb.vs", "fb.fs", nullptr, "fb");
    m_postProcessShader = ResourceManager::GetShader("fb");
    std::cout << "Creating screen quad" << std::endl;
    m_screenQuad->setup();
    std::cout << "Initializing renderer" << std::endl;
    renderer.init();
    std::cout << "Creating rear-view mirror" << std::endl;
    m_rearViewMirror = std::make_unique<Mirror>();
    if (!m_rearViewMirror->initialize()) {
        std::cerr << "Failed to initialize rear-view mirror!" << std::endl;
        m_rearViewMirror.reset(); // Clean up on failure
    }
    std::cout << "Loading models" << std::endl;
    if (useSolarSystemScene) {
        initSolarSystemScene();
    } else {
        loadModels(std::string(cwd) + "/models", std::string(cwd) + "/bin/models");
    }
}
void Game3D::initSolarSystemScene() {
    std::cout << "Initializing Solar System Scene" << std::endl;

    // Create a single sphere mesh to be shared by all celestial bodies
    auto sphere = std::make_shared<m3D::Sphere>("Sphere", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    sphereMesh = sphere->getMesh();

    // Sun
    auto sun = std::make_shared<m3D::PrimitiveShape>("Sun", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(5.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setMesh(sphereMesh);
    scene.AddObject(sun);
    renderer.pointLight.enabled = true;
    renderer.pointLight.position = sun->position;
    renderer.pointLight.diffuse = glm::vec3(1.0f, 1.0f, 0.8f);
    renderer.pointLight.specular = glm::vec3(1.0f, 1.0f, 0.8f);
    renderer.pointLight.linear = 0.0014f;
    renderer.pointLight.quadratic = 0.000007f;

    // Mercury
    auto mercury = std::make_shared<m3D::PrimitiveShape>("Mercury", glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.3f), glm::vec3(0.5f, 0.5f, 0.5f));
    mercury->setMesh(sphereMesh);
    scene.AddObject(mercury);
    orbitalBodies.push_back({mercury, 8.0f, 0.5f, 1.0f, 0.0f, sun});

    // Venus
    auto venus = std::make_shared<m3D::PrimitiveShape>("Venus", glm::vec3(12.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.7f), glm::vec3(0.8f, 0.4f, 0.0f));
    venus->setMesh(sphereMesh);
    scene.AddObject(venus);
    orbitalBodies.push_back({venus, 12.0f, 0.4f, 0.8f, 0.0f, sun});

    // Earth
    auto earth = std::make_shared<m3D::PrimitiveShape>("Earth", glm::vec3(16.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.8f), glm::vec3(0.0f, 0.0f, 1.0f));
    earth->setMesh(sphereMesh);
    scene.AddObject(earth);
    orbitalBodies.push_back({earth, 16.0f, 0.3f, 0.6f, 0.0f, sun});
    // Earth's Moon
    auto moon = std::make_shared<m3D::PrimitiveShape>("Moon", glm::vec3(16.0f + 2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.2f), glm::vec3(0.7f, 0.7f, 0.7f));
    moon->setMesh(sphereMesh);
    scene.AddObject(moon);
    orbitalBodies.push_back({moon, 2.0f, 1.0f, 1.5f, 0.0f, earth});

    // Mars
    auto mars = std::make_shared<m3D::PrimitiveShape>("Mars", glm::vec3(20.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.6f), glm::vec3(1.0f, 0.0f, 0.0f));
    mars->setMesh(sphereMesh);
    scene.AddObject(mars);
    orbitalBodies.push_back({mars, 20.0f, 0.25f, 0.5f, 0.0f, sun});
    // Mars' Moons (Phobos and Deimos)
    auto phobos = std::make_shared<m3D::PrimitiveShape>("Phobos", glm::vec3(20.0f + 1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.6f, 0.3f, 0.0f));
    phobos->setMesh(sphereMesh);
    scene.AddObject(phobos);
    orbitalBodies.push_back({phobos, 1.0f, 1.5f, 2.0f, 0.0f, mars});
    auto deimos = std::make_shared<m3D::PrimitiveShape>("Deimos", glm::vec3(20.0f + 1.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.08f), glm::vec3(0.5f, 0.2f, 0.0f));
    deimos->setMesh(sphereMesh);
    scene.AddObject(deimos);
    orbitalBodies.push_back({deimos, 1.5f, 1.2f, 2.5f, 0.5f, mars});

    // Jupiter
    auto jupiter = std::make_shared<m3D::PrimitiveShape>("Jupiter", glm::vec3(30.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(2.5f), glm::vec3(0.8f, 0.6f, 0.4f));
    jupiter->setMesh(sphereMesh);
    scene.AddObject(jupiter);
    orbitalBodies.push_back({jupiter, 30.0f, 0.15f, 0.3f, 0.0f, sun});
    // Jupiter's Moons
    auto io = std::make_shared<m3D::PrimitiveShape>("Io", glm::vec3(30.0f + 3.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.3f), glm::vec3(1.0f, 0.8f, 0.0f));
    io->setMesh(sphereMesh);
    scene.AddObject(io);
    orbitalBodies.push_back({io, 3.0f, 0.8f, 1.0f, 0.0f, jupiter});
    auto europa = std::make_shared<m3D::PrimitiveShape>("Europa", glm::vec3(30.0f + 3.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.25f), glm::vec3(0.6f, 0.6f, 0.8f));
    europa->setMesh(sphereMesh);
    scene.AddObject(europa);
    orbitalBodies.push_back({europa, 3.5f, 0.7f, 0.9f, 0.8f, jupiter});
    auto ganymede = std::make_shared<m3D::PrimitiveShape>("Ganymede", glm::vec3(30.0f + 4.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.4f), glm::vec3(0.7f, 0.7f, 0.6f));
    ganymede->setMesh(sphereMesh);
    scene.AddObject(ganymede);
    orbitalBodies.push_back({ganymede, 4.0f, 0.6f, 0.8f, 1.2f, jupiter});
    auto callisto = std::make_shared<m3D::PrimitiveShape>("Callisto", glm::vec3(30.0f + 4.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.35f), glm::vec3(0.5f, 0.4f, 0.3f));
    callisto->setMesh(sphereMesh);
    scene.AddObject(callisto);
    orbitalBodies.push_back({callisto, 4.5f, 0.5f, 0.7f, 1.5f, jupiter});

    // Saturn
    auto saturn = std::make_shared<m3D::PrimitiveShape>("Saturn", glm::vec3(40.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(2.0f), glm::vec3(0.9f, 0.9f, 0.7f));
    saturn->setMesh(sphereMesh);
    scene.AddObject(saturn);
    orbitalBodies.push_back({saturn, 40.0f, 0.1f, 0.2f, 0.0f, sun});

    // Uranus
    auto uranus = std::make_shared<m3D::PrimitiveShape>("Uranus", glm::vec3(48.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.5f), glm::vec3(0.6f, 0.8f, 0.9f));
    uranus->setMesh(sphereMesh);
    scene.AddObject(uranus);
    orbitalBodies.push_back({uranus, 48.0f, 0.07f, 0.15f, 0.0f, sun});

    // Neptune
    auto neptune = std::make_shared<m3D::PrimitiveShape>("Neptune", glm::vec3(55.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.5f), glm::vec3(0.2f, 0.2f, 0.8f));
    neptune->setMesh(sphereMesh);
    scene.AddObject(neptune);
    orbitalBodies.push_back({neptune, 55.0f, 0.05f, 0.1f, 0.0f, sun});

    // Pluto
    auto pluto = std::make_shared<m3D::PrimitiveShape>("Pluto", glm::vec3(60.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.8f, 0.7f, 0.6f));
    pluto->setMesh(sphereMesh);
    scene.AddObject(pluto);
    orbitalBodies.push_back({pluto, 60.0f, 0.04f, 0.08f, 0.0f, sun});

    // Asteroid Belt (between Mars and Jupiter)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> radiusDist(22.0f, 28.0f); // Between Mars (20) and Jupiter (30)
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * glm::pi<float>());
    std::uniform_real_distribution<float> speedDist(0.01f, 0.1f);
    std::uniform_real_distribution<float> sizeDist(0.05f, 0.2f);

    for (int i = 0; i < maxAsteroids; ++i) {
        float orbitR = radiusDist(gen);
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        float size = sizeDist(gen);

        glm::vec3 asteroidPosition = glm::vec3(
            orbitR * cos(angle),
            0.0f, // Asteroids are generally on the ecliptic plane
            orbitR * sin(angle)
        );
        auto asteroid = std::make_shared<m3D::PrimitiveShape>("Asteroid", asteroidPosition, glm::vec3(0.0f), glm::vec3(size), glm::vec3(0.3f, 0.2f, 0.1f));
        asteroid->setMesh(sphereMesh);
        scene.AddObject(asteroid);
        orbitalBodies.push_back({asteroid, orbitR, speed, 0.5f, angle, sun});
    }

    // Kuiper Belt
    std::uniform_real_distribution<float> kuiperRadiusDist(65.0f, 150.0f);
    std::uniform_real_distribution<float> kuiperAngleDist(0.0f, 2.0f * glm::pi<float>());
    std::uniform_real_distribution<float> kuiperSpeedDist(0.005f, 0.05f);
    std::uniform_real_distribution<float> kuiperSizeDist(0.1f, 0.5f);

    for (int i = 0; i < 200; ++i) { // Add 200 Kuiper belt objects
        float orbitR = kuiperRadiusDist(gen);
        float angle = kuiperAngleDist(gen);
        float speed = kuiperSpeedDist(gen);
        float size = kuiperSizeDist(gen);

        glm::vec3 kuiperPosition = glm::vec3(
            orbitR * cos(angle),
            0.0f, 
            orbitR * sin(angle)
        );
        auto kuiperObject = std::make_shared<m3D::PrimitiveShape>("KuiperObject", kuiperPosition, glm::vec3(0.0f), glm::vec3(size), glm::vec3(0.5f, 0.3f, 0.2f));
        kuiperObject->setMesh(sphereMesh);
        scene.AddObject(kuiperObject);
        orbitalBodies.push_back({kuiperObject, orbitR, speed, 0.2f, angle, sun});
    }


    // Stars (static background)
    std::uniform_real_distribution<float> starPosDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> starSizeDist(0.05f, 0.15f);
    float starFieldRadius = 1000.0f;

    for (int i = 0; i < 2000; ++i) {
        glm::vec3 starPosition = glm::normalize(glm::vec3(starPosDist(gen), starPosDist(gen), starPosDist(gen))) * starFieldRadius;
        auto star = std::make_shared<m3D::PrimitiveShape>("Star", starPosition, glm::vec3(0.0f), glm::vec3(starSizeDist(gen)), glm::vec3(1.0f, 1.0f, 1.0f));
        star->setMesh(sphereMesh);
        scene.AddObject(star);
    }
}
void Game3D::run() {
    while (!glfwWindowShouldClose(window)) {
        // Timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Timers::tick();
        processInput();

        // Update orbital mechanics
        for (auto& orbitalData : orbitalBodies) {
            orbitalData.currentAngle += orbitalData.orbitSpeed * deltaTime;
            glm::vec3 centerOfOrbit = orbitalData.parentBody ? orbitalData.parentBody->position : glm::vec3(0.0f);
            
            float x = centerOfOrbit.x + orbitalData.orbitRadius * cos(orbitalData.currentAngle);
            float z = centerOfOrbit.z + orbitalData.orbitRadius * sin(orbitalData.currentAngle);
            orbitalData.body->position = glm::vec3(x, centerOfOrbit.y, z);
            orbitalData.body->rotation.y += orbitalData.rotationSpeed * deltaTime;
        }
        scene.update(deltaTime);

        // MIRROR PASS: Render rear-view to mirror framebuffer (if enabled)
        if (m_rearViewMirror && m_showMirror) {
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                                  (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                                  0.1f, 1000.0f);
            
            m_rearViewMirror->renderMirrorView(camera.GetViewMatrix(), projection, 
                [&](const glm::mat4& mirrorView, const glm::mat4& proj) {
                    // Create temporary camera for mirror rendering
                    Camera tempCamera = camera; // Copy current camera
                    // Override the view matrix for mirror rendering
                    renderer.renderWithCustomView(scene, tempCamera, mirrorView, proj);
                });
        }

        // MAIN PASS: Render scene to main framebuffer
        m_framebuffer->bind();
        glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
        m_framebuffer->clear(0.05f, 0.05f, 0.1f);
        glEnable(GL_DEPTH_TEST);
        
        renderer.render(scene, camera);

        // SCREEN PASS: Render framebuffer to screen with post-processing
        m_framebuffer->unbind();
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        
        // Draw the main scene
        m_postProcessShader.Use();
        m_postProcessShader.SetInteger("screenTexture", 0);
        m_framebuffer->bindColorTexture(0);
        m_screenQuad->draw();

        // Draw mirror overlay on top
        if (m_rearViewMirror && m_showMirror) {
            m_rearViewMirror->drawMirror(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // GUI
        Gui::Start();
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
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        m_framebuffer->screenshot("screenshot_" + std::to_string(ms) + ".png");
    }
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