#ifndef INPUT_H
#define INPUT_H

#include "../types.h"  // Include for debug variable
#include "game/Game.h"
#include <map>

std::map<int, bool> keyWasPressed;
bool cursorEnabled = true;

bool toggleKey(int key, bool &toggleState)
{
    bool keyCurrentlyPressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;

    if (!keyWasPressed[key] && keyCurrentlyPressed)
    {
        // Key was just pressed
        toggleState = !toggleState; // Toggle the state
        keyWasPressed[key] = true;  // Set the flag for this key
        return true;                // Indicate that the state was toggled
    }
    else if (keyWasPressed[key] && !keyCurrentlyPressed)
    {
        // Key was just released
        keyWasPressed[key] = false; // Reset the flag for this key
    }

    return false; // Indicate that the state was not toggled
}
void toggleCursor(GLFWwindow *window)
{
    static bool alt = cursorEnabled;
    if (toggleKey(GLFW_KEY_LEFT_ALT, alt))
    {
        cursorEnabled = !cursorEnabled; // Toggle the cursor state
        glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}
void lineMode(){
    static bool isLineMode = false; // Track the current polygon mode

    static bool spacePressed = false;
    if (toggleKey(GLFW_KEY_SPACE, spacePressed))
    {
        // Toggle polygon mode only once after the space key is pressed
        isLineMode = !isLineMode;
        glPolygonMode(GL_FRONT_AND_BACK, isLineMode ? GL_LINE : GL_FILL);
    }
}
void debugToggle(int key){
    static bool debugToggled = debug;
    if (toggleKey(key, debugToggled))
    {
        debug = !debug;
    }
}   
/*
void exitKey(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}
*/

#endif // INPUT_H