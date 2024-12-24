#ifndef GAME_H
#define GAME_H
#include "Level.h"
#include "BallObject.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    std::vector<GameLevel> Levels;
    unsigned int           Level;

    // game state
    GameState    State;
    bool         Keys[1024];
    unsigned int Width, Height;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void Collision();
    bool CheckCollision(GameObject &one, GameObject &two);
    bool CheckCollision(BallObject &one, GameObject &two);
};



#endif //GAME_H
