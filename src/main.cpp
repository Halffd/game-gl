#include "init.h"
#include "types.h"
#include "game/Game.h"

// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH;
const unsigned int SCREEN_HEIGHT = HEIGHT;

GameMode gameMode = MENU;  // Start in menu mode
std::string gameTypeStr = "MainMenu";
bool debug;
Log o;

Game* NeuroMonsters = nullptr;
bool isPaused = false;
double lastTime = 0.0;

// Add key state tracking
bool keys[1024] = {false};
bool keysProcessed[1024] = {false};

// At the top with other globals
struct {
    bool current[1024] = {false};
    bool previous[1024] = {false};
} KeyState;

// Add these functions at the top
struct Resolution {
    int width;
    int height;
    float aspectRatio;
};

Resolution getOptimalResolution() {
    Resolution res;
    
    // Get primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        return {1280, 720, 16.0f/9.0f};
    }

    // Get all video modes for the monitor
    int count;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    if (!modes || count == 0) {
        return {1280, 720, 16.0f/9.0f};
    }

    // Find mode with highest resolution
    const GLFWvidmode* bestMode = &modes[0];
    for (int i = 1; i < count; i++) {
        if (modes[i].width * modes[i].height > bestMode->width * bestMode->height) {
            bestMode = &modes[i];
        }
    }
    
    res.width = bestMode->width;
    res.height = bestMode->height;
    res.aspectRatio = static_cast<float>(res.width) / static_cast<float>(res.height);
    
    if (debug) {
        std::cout << "Selected resolution: " << res.width << "x" << res.height 
                  << " (" << res.aspectRatio << ")" << std::endl;
    }
    
    return res;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        lower(argv[1]);

        // Check the first argument for game type
        if (includes(argv[1], "d"))
        {
            debug = true;
        }
    }
    std::string root;
    root = ResourceManager::getExecutableDir(); 
    if(argc > 2 && debug){
        root = argv[2];
    }
    ResourceManager::root = root;
    
    o.setDir(root);
    o << root << " / " << __FILE__ << "\n";
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Get optimal resolution
    Resolution res = getOptimalResolution();
    
    // Create window with detected resolution
    GLFWwindow* window = glfwCreateWindow(res.width, res.height, "NeuroMonsters", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Update viewport with actual framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    
    // Create game with actual resolution
    NeuroMonsters = new Game(fbWidth, fbHeight);
    NeuroMonsters->Init();
    
    // Initialize collision system
    if (NeuroMonsters->currentArea) {
        NeuroMonsters->InitializeCollision();
    } else {
        std::cerr << "Warning: Area not initialized!" << std::endl;
    }

    // Reset all key states
    std::fill(std::begin(keys), std::end(keys), false);
    std::fill(std::begin(keysProcessed), std::end(keysProcessed), false);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // OpenGL configuration
    // --------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Gui::Init(window);

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // Skip updates if paused
        if (!isPaused) {
            NeuroMonsters->ProcessInput(deltaTime);
            NeuroMonsters->Update(deltaTime);
        }

        // Always render
        glClearColor(0.2f, 0.4f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        NeuroMonsters->Render();

        // Render pause menu if paused
        if (isPaused) {
            Gui::RenderPauseMenu(isPaused, gameMode);
        }

        glfwSwapBuffers(window);
    }

    // Clean up
    delete NeuroMonsters;
    ResourceManager::Clear();
    Gui::Clean();
    glfwTerminate();
    return 0;
}

void key_callback([[maybe_unused]] GLFWwindow* window, int key, 
                 [[maybe_unused]] int scancode, int action, 
                 [[maybe_unused]] int mode)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return;
    }

    if (key >= 0 && key < 1024) {
        // Update key state
        KeyState.previous[key] = KeyState.current[key];
        KeyState.current[key] = (action != GLFW_RELEASE);

        // Handle escape key for pause menu
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !keysProcessed[key]) {
            isPaused = !isPaused;
            keysProcessed[key] = true;
            if (isPaused) {
                lastTime = glfwGetTime();
            }
            return;
        }

        // Update game key state
        NeuroMonsters->Keys[key] = KeyState.current[key];
        
        // Handle key release
        if (action == GLFW_RELEASE) {
            keysProcessed[key] = false;
        }
    }
}

void framebufferSizeCallback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glCheckError(__FILE__, __LINE__);
}

