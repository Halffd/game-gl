// gamemode.h
#ifndef GAME_MODE_H
#define GAME_MODE_H
#include <string>

enum GameType {
    GAME2D,
    GAME3D
};

/**
 * @brief Represents the game mode
 */
enum GameMode {
    DEFAULT,
    PLANE,
    CUBE,
    SPHERE,
    TORUS,
    RING,
    CIRCLE,
    QUAD,
    ARC,
    MENU,
    CUSTOM
};

extern GameMode mode;

int game2d(int argc, char *argv[], std::string type);
int game3d(int argc, char *argv[], std::string type);

#endif //GAME_MODE_H