// gamemode.h
#ifndef GAMEMODE_H
#define GAMEMODE_H
#include <string>

enum GameType {
    ACTION,        // Action games
    ADVENTURE,     // Adventure games
    PUZZLE,        // Puzzle games
    STRATEGY,      // Strategy games
    RPG,           // Role-playing games
    SPORTS,        // Sports games
    SIMULATION,    // Simulation games
    MULTIPLAYER,   // Multiplayer games
    VN,            // Visual Novel games
    GAME2D,
    GAME3D,
    TEST,
    PLANE
};

/**
 * @brief Represents the current state of the game
 */
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_PAUSED,
    GAME_CREDITS,
    GAME_WIN
};

/**
 * @brief Represents the game mode
 */
enum GameMode {
    MENU,
    GAME,
    CREDITS
};


int game2d(int argc, char *argv[], std::string type);
int game3d(int argc, char *argv[], std::string type);

#endif //GAMEMODE_H