#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <random>

// Include game states
#include "../gamemode.h"  // Adjust path if needed

// Include required headers
#include "../ui/DialogueSystem.h"
#include "../effects/Particle.h"
#include "../render/SpriteRenderer.h"
#include "GameObject.h"
#include "Player.h"

// Forward declarations for pointers only
class Area;
class Battle;
class Collider;

/**
 * @brief Main game class that handles game states, rendering, and updates
 * 
 * The Game class is responsible for:
 * - Managing game states (menu, active, paused)
 * - Handling resource initialization and cleanup
 * - Processing input and game logic
 * - Coordinating rendering of game elements
 */
class Game {
public:
    /**
     * @brief Game state enumeration
     * Controls the current state of the game and affects behavior/rendering
     */
    GameState State;
    
    /**
     * @brief Input state array
     * Tracks the state of keyboard keys
     */
    bool Keys[1024];

    /**
     * @brief Window dimensions
     */
    unsigned int Width, Height;
    
    /**
     * @brief Area index
     */
    int area;

    /**
     * @brief Constructor
     * @param width Window width
     * @param height Window height
     */
    Game(unsigned int width, unsigned int height);
    
    /**
     * @brief Destructor
     */
    ~Game();

    /**
     * @brief Initialize game resources and systems
     * Sets up shaders, textures, and basic game components
     */
    void Init();

    /**
     * @brief Process input based on current game state
     * @param dt Delta time for frame-rate independent input
     */
    void ProcessInput(float dt);

    /**
     * @brief Update game logic
     * @param dt Delta time for frame-rate independent updates
     */
    void Update(float dt);

    /**
     * @brief Render the game
     * Handles rendering of all game elements based on current state
     */
    void Render();

    // Game state management
    bool battle = false;
    std::vector<std::shared_ptr<GameObject>> monsters;
    std::shared_ptr<Player> player;
    std::shared_ptr<Area> currentArea;
    std::unique_ptr<Battle> battleSystem;

    void StartBattle();
    void InitializeCollision();

protected:
    /**
     * @brief Initialize game-specific resources
     * Called when transitioning to active game state
     */
    void InitializeGameResources();

    /**
     * @brief Reset the player to initial state
     */
    void ResetPlayer();

    /**
     * @brief Reset the game level
     * Reinitializes monsters and game objects
     */
    void ResetLevel();

    /**
     * @brief Center the camera on the player
     */
    void Center();

    // Add getCurrentEnemy declaration
    std::shared_ptr<GameObject> getCurrentEnemy();

    // Core systems
    std::unique_ptr<SpriteRenderer> Renderer;
    std::unique_ptr<ParticleGenerator> Particles;
    std::unique_ptr<Collider> Collision;
    std::shared_ptr<DialogueSystem> Dialogue;

    // Performance monitoring
    double lastTime;
    int frameCount;
    float fps;
};

#endif // GAME_H
