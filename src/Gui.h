#ifndef GUI_H
#define GUI_H
#include "imgui.h"              // ImGui core header
#include "backends/imgui_impl_glfw.h"    // ImGui GLFW implementation
#include "backends/imgui_impl_opengl3.h" // ImGui OpenGL3 implementation
#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Gui {
    public:
    //GLFWwindow *Window;
    static void Init(GLFWwindow *window);
    static void Start();
    static void Render();
    static void Clean();
};



#endif //GUI_H
