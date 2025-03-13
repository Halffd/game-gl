// gamemode.h
#pragma once

#include <string>
#include "game/Game.h"

enum GameType {
    GAME2D,
    GAME3D
};

// Function declarations
int game2d(int argc, char *argv[], const std::string& type);
int game3d(int argc, char *argv[], const std::string& type);