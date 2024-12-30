#include "init.h"

// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH; // Example value
const unsigned int SCREEN_HEIGHT = HEIGHT; // Example value

GameMode gameMode = MENU;
std::string gameTypeStr = "MainMenu"; // Default game type string
bool debug = false;
Log o;
Game NeuroMonsters(SCREEN_WIDTH, SCREEN_HEIGHT);
float lastTime = 9;
bool isPaused = false;

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
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroMonsters", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Gui::Init(window);
    // initialize game
    // ---------------
    NeuroMonsters.Init();

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
        NeuroMonsters.ProcessInput(deltaTime);

        // update game state
        // -----------------
        NeuroMonsters.Update(deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.4f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        NeuroMonsters.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();
    Gui::Clean();
    glfwTerminate();
    return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    ImGuiIO &io = ImGui::GetIO();
    // Check for escape key to toggle pause state
    if (key == GLFW_KEY_ESCAPE) {
        if (action == GLFW_PRESS) {
            // Toggle pause state
            isPaused = !isPaused;

            if (isPaused) {
                lastTime = glfwGetTime();
                std::cout << "Program paused. " << lastTime << std::endl;
            } else {
                std::cout << "Program resumed." << std::endl;
            }
        } 
        NeuroMonsters.State = isPaused ? GAME_PAUSED : GAME_ACTIVE;
    }
    if (!io.WantCaptureKeyboard) {
        // Handle F4 key to close the application
        if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                NeuroMonsters.Keys[key] = true;
            else if (action == GLFW_RELEASE)
            {
                NeuroMonsters.Keys[key] = false;
                //NeuroMonsters.KeysProcessed[key] = false;
            }
        }
    }
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glCheckError(__FILE__, __LINE__);
}

