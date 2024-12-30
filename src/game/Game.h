#ifndef GAME_H
#define GAME_H
#include "setup.h"
#include "Area.h"
#include "CircleObject.h"
#include "Collider.h"
#include "Area.h"
#include "effects/Particle.h"
#include "ui/Battle.h"
#include <glm/glm.hpp>
#include <tuple>
#include "irrKlang/irrKlang.h"
#include <memory>

class Game {
public:
    std::shared_ptr<Area> currentArea;

    // game state
    GameState    State;
    bool         Keys[1024];
    unsigned int Width, Height;

    std::unique_ptr<SpriteRenderer> Renderer = nullptr;
    std::unique_ptr<Player> player = nullptr;   
    std::unique_ptr<Collider> Collision = nullptr;
    std::shared_ptr<DialogueSystem> Dialogue = nullptr;
    std::unique_ptr<ParticleGenerator> Particles = nullptr;
    std::unique_ptr<Battle> battleSystem = nullptr;
    std::unique_ptr<irrklang::ISoundEngine> audio = nullptr;
    std::vector<std::shared_ptr<GameObject>> monsters;

    unsigned int area; 
    
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
    void Center();
    
private:
    double lastTime;
    int frameCount;
    float fps;
    bool battle = false;
};



#endif //GAME_H
