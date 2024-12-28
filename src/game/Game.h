#ifndef GAME_H
#define GAME_H
#include "Level.h"
#include "CircleObject.h"
#include <glm/glm.hpp>
#include <tuple>
#include "irrKlang/irrKlang.h"
#include <memory>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    std::vector<Level> Levels;
    unsigned int           Level;

    // game state
    GameState    State;
    bool         Keys[1024];
    unsigned int Width, Height;

    irrklang::ISoundEngine* audio;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void ResetPlayer();
    void ResetLevel();
    
private:
    double lastTime;
    int frameCount;
    float fps;
};



#endif //GAME_H
