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
static void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height);

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
      window(nullptr),
      m_postProcessShader(nullptr),
      useFramebuffer(false),
      planetShader(),
      skybox(nullptr),
      skyboxCubemap(nullptr)
{
}

Game3D::~Game3D() {
    delete m_postProcessShader;
    delete skybox;
    delete skyboxCubemap;
    
    for (auto* body : celestialBodies) {
        delete body;
    }
    celestialBodies.clear();
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
    //glEnable(GL_FRAMEBUFFER_SRGB); 
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
    ResourceManager::LoadShader("outline.vs", "outline.fs", nullptr, "outline");
    std::cout << "Creating framebuffer" << std::endl;
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    int fbWidth = 1200;
    int fbHeight = fbWidth / aspectRatio;
    m_framebufferSize = glm::vec2(fbWidth, fbHeight);
    m_framebuffer->create(m_framebufferSize.x, m_framebufferSize.y);
    std::cout << "Loading shader: fb" << std::endl;
    ResourceManager::LoadShader("fb.vs", "fb.fs", nullptr, "fb");
    m_postProcessShader = &ResourceManager::GetShader("fb");
    std::cout << "Creating screen quad" << std::endl;
    m_screenQuad->setup();
    std::cout << "Initializing renderer" << std::endl;
    renderer.init();

    // Initialize skybox
    std::cout << "Initializing skybox..." << std::endl;
    skyboxCubemap = new Cubemap();

    // Set up the skybox texture parameters
    skyboxCubemap->Internal_Format = GL_RGB;
    skyboxCubemap->Image_Format = GL_RGB;
    skyboxCubemap->Wrap_S = GL_CLAMP_TO_EDGE;
    skyboxCubemap->Wrap_T = GL_CLAMP_TO_EDGE;
    skyboxCubemap->Wrap_R = GL_CLAMP_TO_EDGE;
    skyboxCubemap->Filter_Min = GL_LINEAR;
    skyboxCubemap->Filter_Max = GL_LINEAR;

    // Load the skybox texture faces
    std::vector<std::string> faces = {
        ResourceManager::GetTexturePath("skybox/right.jpg"),
        ResourceManager::GetTexturePath("skybox/left.jpg"),
        ResourceManager::GetTexturePath("skybox/top.jpg"),
        ResourceManager::GetTexturePath("skybox/bottom.jpg"),
        ResourceManager::GetTexturePath("skybox/front.jpg"),
        ResourceManager::GetTexturePath("skybox/back.jpg")
    };

    if (skyboxCubemap->Load(faces)) {
        std::cout << "Skybox cubemap loaded successfully!" << std::endl;
        skybox = new Skybox();
        skybox->cubemap = skyboxCubemap;
    } else {
        std::cerr << "Failed to load skybox cubemap!" << std::endl;
        delete skyboxCubemap;
        skyboxCubemap = nullptr;
    }

    m_showMirror = false;
    maxAsteroids = 25;
    if (m_showMirror) {
        std::cout << "Creating rear-view mirror" << std::endl;
        m_rearViewMirror = std::make_unique<Mirror>();
        if (!m_rearViewMirror->initialize()) {
            std::cerr << "Failed to initialize rear-view mirror!" << std::endl;
            m_rearViewMirror.reset(); // Clean up on failure
        }
    } else {
        m_rearViewMirror = nullptr;
    }
    std::cout << "Loading models" << std::endl;
    if (useSolarSystemScene) {
        initSolarSystemScene();
    } else {
        loadModels(std::string(cwd) + "/models", std::string(cwd) + "/bin/models");
    }
}
void Game3D::initSolarSystemScene() {
    std::cout << "Initializing Solar System Scene with Physics" << std::endl;

    // Physical constants
    const float AU = 1.496e11f;              // Astronomical Unit (meters)
    const float EARTH_MASS = 5.972e24f;      // Earth mass (kg)
    const float EARTH_RADIUS = 6.371e6f;     // Earth radius (m)
    const float SOLAR_MASS = 1.989e30f;      // Solar mass (kg)
    const float SOLAR_RADIUS = 6.957e8f;     // Solar radius (m)
    const float DAY = 86400.0f;              // Day in seconds
    const float YEAR = 365.25f * DAY;        // Year in seconds
    const float PI = 3.14159265359f;
    
    // Display scale factors (for visualization)
    const float DISTANCE_SCALE = 1.0f / (AU / 50.0f); // 1 AU = 50 units in scene
    const float SIZE_SCALE = 1.0f / 1e7f;              // Scale radii for visibility
    
    // Load shaders
    Shader& starShader = ResourceManager::LoadShader("star.vs", "star.fs", nullptr, "star");
    Shader& planetShader = ResourceManager::LoadShader("planet.vs", "planet.fs", nullptr, "planet");
    Shader& glowShader = ResourceManager::LoadShader("glow.vs", "glow.fs", nullptr, "glow");
    Shader& coronaShader = ResourceManager::LoadShader("corona.vs", "corona.fs", nullptr, "corona");
    Shader& limbDarkeningShader = ResourceManager::LoadShader("limb_darkening.vs", "limb_darkening.fs", nullptr, "limb_darkening");
    // Additional shaders needed for star effects
    ResourceManager::LoadShader("blur.vs", "blur.fs", nullptr, "blur");
    ResourceManager::LoadShader("bloom.vs", "bloom.fs", nullptr, "bloom");
    
    // Create shared sphere mesh
    auto sphere = std::make_shared<m3D::Sphere>("Sphere", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    sphereMesh = sphere->getMesh();
    
    // Setup projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                           (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                           0.1f, 10000.0f);
    
    // ========== THE SUN ==========
    Star* sun = new Star(
        SOLAR_MASS,
        SOLAR_RADIUS,
        25.05f * DAY,           // Rotation period
        0.0f,                   // No axial tilt
        1.0f,                   // Solar luminosity (normalized)
        5778.0f,                // Surface temperature (K) - G-type star
        sphereMesh
    );
    
    sun->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sun->SetColor(glm::vec3(1.0f, 0.9f, 0.7f));
    
    // Store sun reference for lighting
    celestialBodies.push_back(sun);
    
    // Setup renderer lighting from sun
    renderer.pointLight.enabled = true;
    renderer.pointLight.position = sun->GetPosition();
    renderer.pointLight.diffuse = sun->GetColor();
    renderer.pointLight.specular = glm::vec3(1.0f, 1.0f, 0.9f);
    renderer.pointLight.linear = 0.0001f;
    renderer.pointLight.quadratic = 0.000001f;
    
    // ========== MERCURY ==========
    Planet* mercury = new Planet(
        3.3011e23f,             // Mass (kg)
        2.4397e6f,              // Radius (m)
        1407.6f * 3600.0f,      // Rotation period (58.6 Earth days)
        0.034f * PI / 180.0f,   // Axial tilt (0.034°)
        sun,                    // Parent body
        87.969f * DAY,          // Orbital period
        0.387f * AU,            // Semi-major axis
        0.2056f,                // Eccentricity (high!)
        7.005f * PI / 180.0f,   // Inclination
        sphereMesh,
        29.124f * PI / 180.0f,  // Argument of periapsis
        48.331f * PI / 180.0f,  // Longitude of ascending node
        0.0f                    // Mean anomaly at epoch
    );
    
    mercury->SetColor(glm::vec3(0.7f, 0.6f, 0.5f));
    mercury->SetRelativisticFactor(1.0f); // Enable GR precession
    celestialBodies.push_back(mercury);
    
    // ========== VENUS ==========
    Planet* venus = new Planet(
        4.8675e24f,
        6.0518e6f,
        -5832.5f * 3600.0f,     // Retrograde rotation (negative)
        177.36f * PI / 180.0f,  // Nearly upside down
        sun,
        224.701f * DAY,
        0.723f * AU,
        0.0067f,                // Nearly circular
        3.39f * PI / 180.0f,
        sphereMesh,
        54.884f * PI / 180.0f,
        76.680f * PI / 180.0f,
        0.0f
    );
    
    venus->SetColor(glm::vec3(0.9f, 0.7f, 0.5f));
    celestialBodies.push_back(venus);
    
    // ========== EARTH ==========
    Planet* earth = new Planet(
        EARTH_MASS,
        EARTH_RADIUS,
        DAY,                    // 24 hours
        23.44f * PI / 180.0f,   // Axial tilt (23.44°)
        sun,
        YEAR,                   // 365.25 days
        AU,                     // 1 AU
        0.0167f,                // Low eccentricity
        0.0f,                   // Reference plane
        sphereMesh,
        102.94f * PI / 180.0f,
        0.0f,
        0.0f
    );
    
    earth->SetColor(glm::vec3(0.2f, 0.5f, 0.9f));
    celestialBodies.push_back(earth);
    
    // ========== MOON (Earth's satellite) ==========
    Planet* moon = new Planet(
        7.342e22f,
        1.737e6f,
        27.322f * DAY,          // Tidally locked
        1.54f * PI / 180.0f,
        earth,                  // Orbits Earth!
        27.322f * DAY,
        3.844e8f,               // 384,400 km
        0.0549f,
        5.145f * PI / 180.0f,
        sphereMesh,
        0.0f,
        0.0f,
        0.0f
    );
    
    moon->SetColor(glm::vec3(0.7f, 0.7f, 0.7f));
    earth->AddSatellite(moon);
    celestialBodies.push_back(moon);
    
    // ========== MARS ==========
    Planet* mars = new Planet(
        6.4171e23f,
        3.3895e6f,
        24.6229f * 3600.0f,     // ~24.6 hours
        25.19f * PI / 180.0f,
        sun,
        686.98f * DAY,          // ~1.88 Earth years
        1.524f * AU,
        0.0934f,
        1.850f * PI / 180.0f,
        sphereMesh,
        286.502f * PI / 180.0f,
        49.558f * PI / 180.0f,
        0.0f
    );
    
    mars->SetColor(glm::vec3(0.8f, 0.4f, 0.2f));
    celestialBodies.push_back(mars);
    
    // Mars moons
    Planet* phobos = new Planet(
        1.0659e16f, 11.267e3f, 7.65f * 3600.0f, 0.0f,
        mars, 7.65f * 3600.0f, 9.376e6f, 0.0151f, 1.093f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    phobos->SetColor(glm::vec3(0.5f, 0.3f, 0.2f));
    mars->AddSatellite(phobos);
    celestialBodies.push_back(phobos);
    
    Planet* deimos = new Planet(
        1.4762e15f, 6.2e3f, 30.3f * 3600.0f, 0.0f,
        mars, 30.3f * 3600.0f, 2.3459e7f, 0.00033f, 1.793f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    deimos->SetColor(glm::vec3(0.4f, 0.2f, 0.1f));
    mars->AddSatellite(deimos);
    celestialBodies.push_back(deimos);
    
    // ========== JUPITER ==========
    Planet* jupiter = new Planet(
        1.8982e27f,             // Massive!
        6.9911e7f,              // Large radius
        9.925f * 3600.0f,       // Fast rotation (~10 hours)
        3.13f * PI / 180.0f,
        sun,
        11.862f * YEAR,         // ~12 Earth years
        5.204f * AU,
        0.0489f,
        1.303f * PI / 180.0f,
        sphereMesh,
        273.867f * PI / 180.0f,
        100.464f * PI / 180.0f,
        0.0f
    );
    
    jupiter->SetColor(glm::vec3(0.9f, 0.8f, 0.6f));
    celestialBodies.push_back(jupiter);
    
    // Galilean moons
    Planet* io = new Planet(
        8.9319e22f, 1.8216e6f, 42.5f * 3600.0f, 0.0f,
        jupiter, 42.5f * 3600.0f, 4.217e8f, 0.0041f, 0.05f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    io->SetColor(glm::vec3(1.0f, 0.8f, 0.2f));
    jupiter->AddSatellite(io);
    celestialBodies.push_back(io);
    
    Planet* europa = new Planet(
        4.7998e22f, 1.5608e6f, 85.2f * 3600.0f, 0.0f,
        jupiter, 85.2f * 3600.0f, 6.711e8f, 0.009f, 0.47f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    europa->SetColor(glm::vec3(0.6f, 0.6f, 0.8f));
    jupiter->AddSatellite(europa);
    celestialBodies.push_back(europa);
    
    Planet* ganymede = new Planet(
        1.4819e23f, 2.6341e6f, 172.0f * 3600.0f, 0.0f,
        jupiter, 172.0f * 3600.0f, 1.0704e9f, 0.0013f, 0.2f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    ganymede->SetColor(glm::vec3(0.7f, 0.7f, 0.6f));
    jupiter->AddSatellite(ganymede);
    celestialBodies.push_back(ganymede);
    
    Planet* callisto = new Planet(
        1.0759e23f, 2.4103e6f, 400.5f * 3600.0f, 0.0f,
        jupiter, 400.5f * 3600.0f, 1.8827e9f, 0.0074f, 0.19f * PI / 180.0f,
        sphereMesh, 0.0f, 0.0f, 0.0f
    );
    callisto->SetColor(glm::vec3(0.5f, 0.4f, 0.3f));
    jupiter->AddSatellite(callisto);
    celestialBodies.push_back(callisto);
    
    // ========== SATURN ==========
    Planet* saturn = new Planet(
        5.6834e26f,
        5.8232e7f,
        10.656f * 3600.0f,      // ~10.7 hours
        26.73f * PI / 180.0f,
        sun,
        29.457f * YEAR,         // ~29.5 years
        9.537f * AU,
        0.0565f,
        2.485f * PI / 180.0f,
        sphereMesh,
        339.392f * PI / 180.0f,
        113.665f * PI / 180.0f,
        0.0f
    );
    
    saturn->SetColor(glm::vec3(0.9f, 0.9f, 0.7f));
    
    // Add Saturn's iconic rings
    Texture2D ringTexture = ResourceManager::LoadTexture2D("saturn_rings.png", "", true);
    saturn->EnableRings(ringTexture.ID, 7.4e7f * SIZE_SCALE, 1.4e8f * SIZE_SCALE);
    
    celestialBodies.push_back(saturn);
    
    // ========== URANUS ==========
    Planet* uranus = new Planet(
        8.6810e25f,
        2.5362e7f,
        17.24f * 3600.0f,       // Retrograde rotation
        97.77f * PI / 180.0f,   // Extreme tilt (sideways!)
        sun,
        84.011f * YEAR,
        19.191f * AU,
        0.04717f,
        0.773f * PI / 180.0f,
        sphereMesh,
        96.998857f * PI / 180.0f,
        74.006f * PI / 180.0f,
        0.0f
    );
    
    uranus->SetColor(glm::vec3(0.6f, 0.8f, 0.9f));
    celestialBodies.push_back(uranus);
    
    // ========== NEPTUNE ==========
    Planet* neptune = new Planet(
        1.02413e26f,
        2.4622e7f,
        16.11f * 3600.0f,
        28.32f * PI / 180.0f,
        sun,
        164.79f * YEAR,
        30.07f * AU,
        0.008678f,
        1.767f * PI / 180.0f,
        sphereMesh,
        273.187f * PI / 180.0f,
        131.784f * PI / 180.0f,
        0.0f
    );
    
    neptune->SetColor(glm::vec3(0.2f, 0.3f, 0.9f));
    celestialBodies.push_back(neptune);
    
    // ========== PLUTO (Dwarf Planet) ==========
    Planet* pluto = new Planet(
        1.303e22f,
        1.188e6f,
        153.3f * 3600.0f,       // 6.4 Earth days
        122.53f * PI / 180.0f,  // Extreme tilt
        sun,
        248.09f * YEAR,         // ~248 years!
        39.482f * AU,
        0.2488f,                // Very eccentric
        17.16f * PI / 180.0f,   // Highly inclined
        sphereMesh,
        113.834f * PI / 180.0f,
        110.299f * PI / 180.0f,
        0.0f
    );
    
    pluto->SetColor(glm::vec3(0.8f, 0.7f, 0.6f));
    celestialBodies.push_back(pluto);
    
    // ========== ASTEROID BELT ==========
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> asteroidRadiusDist(2.2f * AU, 3.2f * AU); // Between Mars and Jupiter
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    std::uniform_real_distribution<float> inclinationDist(0.0f, 20.0f * PI / 180.0f);
    std::uniform_real_distribution<float> eccentricityDist(0.0f, 0.3f);
    std::uniform_real_distribution<float> asteroidSizeDist(100.0f, 500000.0f); // 100m to 500km
    
    std::cout << "Generating " << maxAsteroids << " asteroids..." << std::endl;
    
    for (int i = 0; i < maxAsteroids; ++i) {
        float semiMajor = asteroidRadiusDist(gen);
        float eccentricity = eccentricityDist(gen);
        float inclination = inclinationDist(gen);
        float size = asteroidSizeDist(gen);
        float meanAnomaly = angleDist(gen);
        
        // Orbital period from Kepler's third law: T² ∝ a³
        float orbitalPeriod = 2.0f * PI * sqrtf(powf(semiMajor, 3.0f) / (6.674e-11f * SOLAR_MASS));
        
        Planet* asteroid = new Planet(
            1e12f,                  // ~1 billion kg (small)
            size,
            12.0f * 3600.0f,       // Random fast rotation
            angleDist(gen),        // Random tilt
            sun,
            orbitalPeriod,
            semiMajor,
            eccentricity,
            inclination,
            sphereMesh,
            angleDist(gen),        // Random periapsis
            angleDist(gen),        // Random ascending node
            meanAnomaly
        );
        
        asteroid->SetColor(glm::vec3(0.3f, 0.25f, 0.2f));
        celestialBodies.push_back(asteroid);
    }
    
    // ========== KUIPER BELT ==========
    std::uniform_real_distribution<float> kuiperRadiusDist(30.0f * AU, 50.0f * AU);
    std::uniform_real_distribution<float> kuiperSizeDist(50000.0f, 1000000.0f); // 50km to 1000km
    std::uniform_real_distribution<float> kuiperInclinationDist(0.0f, 30.0f * PI / 180.0f);
    
    std::cout << "Generating " << maxKuiperBeltObjects << " Kuiper Belt objects..." << std::endl;
    
    for (int i = 0; i < maxKuiperBeltObjects; ++i) {
        float semiMajor = kuiperRadiusDist(gen);
        float eccentricity = eccentricityDist(gen);
        float inclination = kuiperInclinationDist(gen);
        float size = kuiperSizeDist(gen);
        float meanAnomaly = angleDist(gen);
        
        float orbitalPeriod = 2.0f * PI * sqrtf(powf(semiMajor, 3.0f) / (6.674e-11f * SOLAR_MASS));
        
        Planet* kbo = new Planet(
            5e20f,                 // ~500 billion kg
            size,
            18.0f * 3600.0f,
            angleDist(gen),
            sun,
            orbitalPeriod,
            semiMajor,
            eccentricity,
            inclination,
            sphereMesh,
            angleDist(gen),
            angleDist(gen),
            meanAnomaly
        );
        
        kbo->SetColor(glm::vec3(0.4f, 0.35f, 0.3f));
        celestialBodies.push_back(kbo);
    }
    
    // ========== DISTANT STARS (Background) ==========
    std::uniform_real_distribution<float> starPosDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> starTempDist(3000.0f, 40000.0f);
    std::uniform_real_distribution<float> starMassDist(0.1f * SOLAR_MASS, 50.0f * SOLAR_MASS);
    std::uniform_real_distribution<float> starRadiusDist(0.1f * SOLAR_RADIUS, 100.0f * SOLAR_RADIUS);
    float starFieldRadius = 5000.0f * AU; // Very far away
    
    std::cout << "Generating " << maxDistantStars << " background stars..." << std::endl;
    
    for (int i = 0; i < maxDistantStars; ++i) {
        // Random position on celestial sphere
        glm::vec3 starDirection = glm::normalize(glm::vec3(
            starPosDist(gen), 
            starPosDist(gen), 
            starPosDist(gen)
        ));
        glm::vec3 starPosition = starDirection * starFieldRadius;
        
        float temp = starTempDist(gen);
        float mass = starMassDist(gen);
        float radius = starRadiusDist(gen);
        
        // Calculate luminosity based on mass-luminosity relation: L ∝ M^3.5
        float luminosity = powf(mass / SOLAR_MASS, 3.5f);
        
        Star* backgroundStar = new Star(
            mass,
            radius,
            (10.0f + (rand() % 100)) * DAY, // Random rotation 10-110 days
            0.0f,
            luminosity,
            temp,
            sphereMesh
        );
        
        backgroundStar->SetPosition(starPosition);
        
        // Disable expensive effects for distant stars would go here
        // Currently not implemented in Star class
        
        celestialBodies.push_back(backgroundStar);
    }
    
    std::cout << "Solar System scene initialized with " << celestialBodies.size() 
              << " celestial bodies" << std::endl;
}
void Game3D::updateSolarSystem(float deltaTime) {
    // Apply time scale
    float scaledDeltaTime = deltaTime * timeScale;

// Update all celestial bodies
for (auto* body : celestialBodies) {
    body->Update(scaledDeltaTime);
}

// Update sun position for lighting (if sun moved)
if (!celestialBodies.empty()) {
    CelestialBody* sun = celestialBodies[0]; // First body is the Sun
    renderer.pointLight.position = sun->GetPosition();
}

}

void Game3D::renderSolarSystem(Shader& shader) {
    // Setup camera and projection
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f, 10000.0f
    );
    glm::mat4 view = camera.GetViewMatrix();

shader.Use();
shader.SetMatrix4("projection", projection);
shader.SetMatrix4("view", view);
shader.SetVector3f("viewPos", camera.Position);

// Render all celestial bodies
for (auto* body : celestialBodies) {
    // Check if it's a Star or Planet for specialized rendering
    if (Star* star = dynamic_cast<Star*>(body)) {
        star->Draw(shader);
    } else if (Planet* planet = dynamic_cast<Planet*>(body)) {
        planet->Draw(shader);
    } else {
        body->Draw(shader);
    }
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
        if(useSolarSystemScene) {
            planetShader.Use();
            planetShader.SetInteger("useBlinnPhong", static_cast<int>(!usePhong)); // Remove the extra 'true' parameter
            updateSolarSystem(deltaTime);
            renderSolarSystem(planetShader);
        } else {
            auto& modelShader = ResourceManager::GetShader("model");
            modelShader.Use();
            modelShader.SetInteger("useBlinnPhong", static_cast<int>(!usePhong));
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

        // Render skybox first (if enabled)
        if (skybox && useSkybox) {
            // Use the same projection as main camera
            glm::mat4 projection = camera.GetProjectionMatrix();
            skybox->render(camera.GetViewMatrix(), projection);
        }

        renderer.render(scene, camera);

        // SCREEN PASS: Render framebuffer to screen with post-processing
        m_framebuffer->unbind();
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        
        // Draw the main scene
        m_postProcessShader->Use();
        m_postProcessShader->SetInteger("screenTexture", 0);
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
    toggleKey(GLFW_KEY_B, usePhong);
    static bool lastSrgbToggle = false;
    static bool srgbToggle = false;
    toggleKey(GLFW_KEY_R, srgbToggle);
    if (srgbToggle != lastSrgbToggle) {
        lastSrgbToggle = srgbToggle;
        if (srgbToggle) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        } else {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }
    }
}

void Game3D::mouse_callback([[maybe_unused]] double xposIn, double yposIn) {
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

void Game3D::scroll_callback(double /*xoffset*/, double yoffset) {
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

static void framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}