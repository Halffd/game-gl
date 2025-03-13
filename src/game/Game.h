#ifndef GAME_H
#define GAME_H
#include "Level.h"
#include "BallObject.h"
#include "Particle.h"
#include <glm/glm.hpp>
#include <tuple>
#include <memory>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_PAUSED,
    GAME_CREDITS
};

class Game
{
public:
    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };    
    typedef std::tuple<bool, Direction, glm::vec2> Collision;    
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
    void ResetPlayer();
    void ResetLevel();
    Direction VectorDirection(glm::vec2 target);
    void Collisions();
    bool CheckCollision(GameObject &one, GameObject &two);
    Collision CheckCollision(BallObject &one, GameObject &two);
    
protected:
    // Performance monitoring
    double lastTime;
    int frameCount;
    float fps;

    // Core systems
    std::unique_ptr<SpriteRenderer> Renderer;
    std::unique_ptr<SpriteRenderer> Renderer2;
    std::unique_ptr<ParticleGenerator> Particles;
};

#endif // GAME_H
