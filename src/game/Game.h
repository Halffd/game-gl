#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include <glm/glm.hpp>
#include "render/SpriteRenderer.h"
#include "effects/Particle.h"
#include "game/GameObject.h"
#include "game/Level.h"

// Forward declarations
class GameObject;
class BallObject;
class Particle;
class Level;

enum GameMode {
    PLANE,
    GAME
};

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
public:
    // Game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    std::vector<GameLevel> Levels;
    unsigned int Level;

    // Constructor/Destructor
    Game(unsigned int width, unsigned int height);
    ~Game();

    // Initialize game state (load all shaders/textures/levels)
    void Init();

    // Game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    // Reset
    void ResetLevel();
    void ResetPlayer();

    // Collision
    bool CheckCollision(GameObject &one, GameObject &two);
    Collision CheckCollision(BallObject &one, GameObject &two);
    Direction VectorDirection(glm::vec2 target);
    void Collisions();

private:
    std::unique_ptr<SpriteRenderer> Renderer;
    std::unique_ptr<SpriteRenderer> Renderer2;
    std::unique_ptr<ParticleGenerator> Particles;

    // Timing variables for FPS calculation
    double lastTime;
    int frameCount;
    float fps;
};
