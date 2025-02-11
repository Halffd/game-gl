#ifndef GUI_H
#define GUI_H
#include <iostream>
#include <imgui/imgui.h>              // ImGui core header
#include <imgui/backends/imgui_impl_glfw.h>    // ImGui GLFW implementation
#include <imgui/backends/imgui_impl_opengl3.h> // ImGui OpenGL3 implementation
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../gamemode.h"  // For GameMode enum
//#include <ft2build.h>
//#include FT_FREETYPE_H 

class Gui {
    public:
    //GLFWwindow *Window;
    static void Init(GLFWwindow *window);
    static void Start();
    static void Render();
    static void Clean();
    static void RenderPauseMenu(bool& isPaused, GameMode& gameMode);

private:
    static const int MENU_WIDTH = 200;
    static const int MENU_HEIGHT = 200;
};



#endif //GUI_H
