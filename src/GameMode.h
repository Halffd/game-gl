// gamemode.h
#ifndef GAMEMODE_H
#define GAMEMODE_H

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
    GAME3D
};

int game2d(int argc, char *argv[], std::string type);
int game3d(int argc, char *argv[], std::string type);

#endif //GAMEMODE_H