//
// Created by halff on 25/09/2024.
//

#include "Gui.h"
void Gui::Init(GLFWwindow *window){
    // Setup ImGui context with docking and multi-window support
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