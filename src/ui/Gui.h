#ifndef GUI_H
#define GUI_H
#include <iostream>
#include <imgui/imgui.h>              // ImGui core header
#include <imgui/backends/imgui_impl_glfw.h>    // ImGui GLFW implementation
#include <imgui/backends/imgui_impl_opengl3.h> // ImGui OpenGL3 implementation
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <ft2build.h>
//#include FT_FREETYPE_H 

class Gui {
    public:
    //GLFWwindow *Window;
    static void Init(GLFWwindow *window);
    static void Start();
    static void Render();
    static void Clean();
};



#endif //GUI_H
