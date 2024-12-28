#include "init.h"

// Define the dimensions
const unsigned SCREEN_WIDTH = WIDTH; // Example value
const unsigned int SCREEN_HEIGHT = HEIGHT; // Example value

GameMode gameMode = MENU;
std::string gameTypeStr = "MainMenu"; // Default game type string
bool debug = true;
bool isPaused = false;
Game NeuroJam(SCREEN_WIDTH, SCREEN_HEIGHT);

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
        root = argv[2]
    }
    o << root << " / " << __FILE__ << endl;
    ResourceManager::root = root;
    
    logger.setDir(root);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroJam", nullptr, nullptr);
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
    NeuroJam.Init();

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
        NeuroJam.ProcessInput(deltaTime);

        // update game state
        // -----------------
        NeuroJam.Update(deltaTime);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        NeuroJam.Render();

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
    if (!io.WantCaptureKeyboard) {
        // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                NeuroJam.Keys[key] = true;
            else if (action == GLFW_RELEASE)
                NeuroJam.Keys[key] = false;
        }
    }
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glCheckError(__FILE__, __LINE__);
}

