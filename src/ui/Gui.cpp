#include "Gui.h"

void Gui::Init(GLFWwindow *window){
    // Setup ImGui context with docking and multi-window support
    /*FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        //return;
    }*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Set ImGui configuration flags for docking
    ImGuiIO &io = ImGui::GetIO();
    #ifdef WINDOWS
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-window support
    io.ConfigDebugHighlightIdConflicts = true;
    #endif
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Scale the entire GUI (default is 1.0f)
    io.FontGlobalScale = 2.0f; // Scale text and UI elements by 2x

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Adjust according to your OpenGL version

    // Load a larger font
    // ImFont* font = io.Fonts->AddFontFromFileTTF("path/to/your/font.ttf", 18.0f); // Specify path to your font file
    // io.FontDefault = font;
}
void Gui::Start(){
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Create a docking space
    //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // Use the main viewport
// Set custom style for ImGui
    ImGui::StyleColorsDark(); // Use dark theme
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0); // Black background
    style.Colors[ImGuiCol_Border] = ImColor(0, 100, 0); // Dark ` borders
    style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255); // White text
    style.Colors[ImGuiCol_Button] = ImColor(0, 100, 0); // Dark green button background
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(0, 150, 0); // Lighter green on hover
    style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 80, 0); // Even darker green when pressed

    //ImGui::ShowDemoWindow();
}
void Gui::Render() {
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #ifdef WINDOWS
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // Update and Render additional Platform Windows
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    #endif
}
void Gui::Clean() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
