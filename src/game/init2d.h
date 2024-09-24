//
// Created by halff on 12/09/2024.
//

#ifndef INIT_2D_H
#define INIT_2D_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"
#include "../Util.hpp"
#include "../ResourceManager.h"

// The Width of the screen
extern const unsigned int SCREEN_WIDTH;
// The height of the screen
extern const unsigned int SCREEN_HEIGHT;

extern Game Breakout;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int game(int argc, char *argv[]);
#endif