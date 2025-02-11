#ifndef GAMEMODE_H
#define GAMEMODE_H

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

#endif // GAMEMODE_H 