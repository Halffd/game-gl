#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cassert>

#include "Game.h"
#include "asset/ResourceManager.h"
#include "render/SpriteRenderer.h"
#include "Camera.h"
#include "Player.h"
#include "CircleObject.h"
#include "util/Util.h"
#include "root.h"
#include <iostream>
#include "ui/Gui.h"
#include "gamemode.h"
#include "types.h"
#include "effects/Particle.h"
#include "Collider.h"
#include "input/Input.h"
#include "Area.h"
#include "ui/Battle.h"
#include "asset/TilemapManager.h"
#include "util/Random.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(300.0f, 300.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(12500.0f);
bool gameOver = false;
// Add this in your Game constructor or Init method
std::random_device rd;
std::mt19937 gen = std::mt19937(rd());  // Seeded generator
std::uniform_real_distribution<float> dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

// Add debug variable declaration if not already present
extern bool debug;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), 
    Width(width), 
    Height(height),
    area(0)
{
    Camera::Instance = std::make_shared<Camera>(glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height));
    Dialogue = std::make_shared<DialogueSystem>();

    // Initialize essential resources with proper parameters
    glm::vec2 playerPos = glm::vec2(120.0f, Height / 3.0f);
    player = std::make_shared<Player>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("player.png"), 
        glm::vec3(1.0f, 1.0f, 1.0f), 5, 5
    );

    currentArea = std::make_shared<Area>(Width, Height);
    Collision = std::make_unique<Collider>(Dialogue, currentArea->tilemapManager);
}

Game::~Game() { }

void Game::Init()
{
    // Timing variables
    lastTime = glfwGetTime();
    frameCount = 0;
    fps = 0.0f;

    // Load shaders
    ResourceManager::LoadShader("sprite/vertex.glsl", "sprite/fragment.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("particle.vs", "particle.fs", nullptr, "particle");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);

    // Initialize renderer
    const std::vector<float> vertices = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    Renderer = std::make_unique<SpriteRenderer>(
        ResourceManager::GetShader("sprite"), vertices
    );

    // Load textures
    ResourceManager::LoadTexture2D("particle.png", "particle");

    // Initialize particle generator
    Particles = std::make_unique<ParticleGenerator>(
        ResourceManager::GetShader("particle"), 
        ResourceManager::GetTexture2D("particle"), 
        1500
    );

    // Initialize game resources
    InitializeGameResources();

    // Initialize battle system
    battleSystem = std::make_unique<Battle>(player, nullptr, Width, Height);
    
    if (debug) {
        std::cout << "Game initialized with resolution: " << Width << "x" << Height << std::endl;
    }
}

// New method to separate game resource initialization
void Game::InitializeGameResources()
{
    ResetPlayer();
    ResetLevel();

    // Initialize area
    if (!currentArea) {
        currentArea = std::make_shared<Area>(Width, Height);
    }
    currentArea->LoadTilemap("levels/main.lvl", "tiles.png", "bg.png", 7, 7);
    currentArea->enemies = monsters;

    // Initialize collision system
    if (!Collision) {
        Collision = std::make_unique<Collider>(Dialogue, currentArea->tilemapManager);
    }
    Collision->SetBoundingBoxOffset(glm::vec2(50.0,25.0f));
    Collision->SetBoundingBoxSize(glm::vec2(60.0,120.0f));
}

void Game::Render()
{
    Gui::Start();

    // Calculate FPS
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) {
        fps = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
    if (battleSystem && battleSystem->IsActive()) {
        battleSystem->Render(*Renderer);
        battleSystem->RenderUI();
    } else {    
        if(currentArea){
            currentArea->Draw(*Renderer); 
        }
        // Render based on the current game state
        //if ((State == GAME_PAUSED || State == GAME_ACTIVE) && currentArea) {
            player->Draw(*Renderer);
            Particles->Draw();
        //} 
    }
    if(State != GAME_ACTIVE) {
        // Define the size of the window
        const ImVec2 windowSize(400, 300); // Adjust dimensions as needed
        ImVec2 windowPos = ImVec2((Width - windowSize.x) * 0.5f, (Height - windowSize.y) * 0.5f); // Centered position

        // Set a larger font size (bold if you have a bold font)
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Assumes the first font is the one used (adjust if needed)
        ImGui::GetFont()->Scale = 1.8f; // Increase scale (adjust as necessary)

        // Pause Menu
        if (State == GAME_PAUSED) {
            ImGui::SetNextWindowPos(windowPos);
            ImGui::SetNextWindowSize(windowSize);
            ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            ImGui::Text("Game Paused");

            // Calculate button size
            float buttonWidth = windowSize.x - 10.0f;
            float buttonHeight = windowSize.y * 0.2f;

            if (ImGui::Button("Resume", ImVec2(buttonWidth, buttonHeight))) {
                State = GAME_ACTIVE;
            }
            //ImGui::SameLine(); // Maintain horizontal spacing for centering

            if (ImGui::Button("Main Menu", ImVec2(buttonWidth, buttonHeight))) {
                State = GAME_MENU;
                battleSystem->End();
            }
            //ImGui::SameLine(); // Maintain horizontal spacing for centering

            if (ImGui::Button("Exit Game", ImVec2(buttonWidth, buttonHeight))) {
                exit(0);
            }
            ImGui::End();
        } 
        // Main Menu
        else if (State == GAME_MENU) {
            ImGui::SetNextWindowPos(windowPos);
            ImGui::SetNextWindowSize(windowSize);
            ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            ImGui::Text("NeuroMonsters");

            // Calculate button size
            float buttonWidth = windowSize.x - 10.0f;
            float buttonHeight = windowSize.y * 0.2f;

            if (ImGui::Button("Start Game", ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Game Start Selected" << std::endl;
                State = GAME_ACTIVE;
            }

            if (ImGui::Button("Credits", ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Credits Selected" << std::endl;
                State = GAME_CREDITS;
            }

            if (ImGui::Button("Exit Game", ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Quit" << std::endl;
                exit(0);
            }
            ImGui::End();
        } 
        // Credits
        else if (State == GAME_CREDITS) {
            ImGui::SetNextWindowPos(windowPos);
            ImGui::SetNextWindowSize(windowSize);
            ImGui::Begin("Credits", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            ImGui::Text("Created by Half");
            
            // Calculate button size
            float buttonWidth = windowSize.x - 10.0f;
            float buttonHeight = windowSize.y * 0.2f;

            if (ImGui::Button("Back to Menu", ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Returning to Menu" << std::endl;
                State = GAME_MENU;  // Update this line to actually change state
            }
            ImGui::End();
        }

        // Restore original font settings
        ImGui::PopFont();
    }
    if (debug) {
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("FPS: %.1f", fps);
        ImGui::PopStyleColor();
        ImGui::End();
    }
    // Render the GUI
    Gui::Render();
}
void Game::ProcessInput([[maybe_unused]] float dt)
{
    static float stepCounter = 0.0f;
    static const float STEP_THRESHOLD = 29.0f;
    static const float BATTLE_CHANCE = 0.45f;
    
    if (State == GAME_ACTIVE) {
        bool moved = false;

        // Player movement
        if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT]) {
            player->Move(Direction::LEFT);
            moved = true;
        } 
        else if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT]) {
            player->Move(Direction::RIGHT);
            moved = true;
        } 
        else if (this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_UP]) {
            player->Move(Direction::UP);
            moved = true;
        } 
        else if (this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_DOWN]) {
            player->Move(Direction::DOWN);
            moved = true;
        } 
        else {
            player->Stop();
        }
        if(battleSystem){
            battle = !battleSystem->IsActive();
            if (debug) std::cout << "Battle system exists, battle=" << battle << std::endl;
        } else {
            battle = true;
            if (debug) std::cout << "No battle system, battle=true" << std::endl;
        }
        
        if(player->stats.health <= 0){
            if (debug) std::cout << "Player health <= 0, returning" << std::endl;
            return;
        }

        // Check for battle initiation only if player moved and battle is possible
        if (moved && battle) {
            if (debug) {
                std::cout << "Movement detected, checking for battle" << std::endl;
                std::cout << "Current step counter: " << stepCounter << std::endl;
            }

            float distanceMoved = 0.1f;
            stepCounter += distanceMoved;

            if (stepCounter >= STEP_THRESHOLD) {
                if (debug) std::cout << "Step threshold reached (" << STEP_THRESHOLD << ")" << std::endl;
                
                stepCounter = 0.0f;
                std::uniform_real_distribution<float> battleChance(0.0f, 1.0f);
                float roll = battleChance(gen);

                if (debug) {
                    std::cout << "Battle roll: " << roll << " (needs < " << BATTLE_CHANCE << ")" << std::endl;
                }

                if (roll < BATTLE_CHANCE) {
                    if (debug) std::cout << "Battle triggered, getting random enemy..." << std::endl;
                    
                    auto enemy = currentArea->GetRandomEnemy();
                    
                    if (enemy) {
                        if (debug) {
                            std::cout << "Enemy found: " << enemy->name << std::endl;
                            std::cout << "Starting battle..." << std::endl;
                        }

                        player->Stop();
                        Camera::Instance->SetPosition(glm::vec2(0.0f));
                        Camera::Instance->SetSize(glm::vec2(Width, Height));
                        
                        battleSystem = std::make_unique<Battle>(player, enemy, Width, Height);
                        if (battleSystem) {
                            battleSystem->Start();
                            battle = false;
                            if (debug) std::cout << "Battle system created and started successfully" << std::endl;
                        } else {
                            if (debug) std::cout << "Failed to create battle system!" << std::endl;
                        }
                    } else {
                        if (debug) std::cout << "No enemy found!" << std::endl;
                    }
                } else {
                    if (debug) std::cout << "Battle check failed, continuing normal gameplay" << std::endl;
                }
            }
        } else {
            if (debug && !moved) std::cout << "No movement detected" << std::endl;
            if (debug && !battle) std::cout << "Battle not possible at this time" << std::endl;
            battle = false;
        }
    }
}
void Game::Update(float dt)
{
    if (State == GAME_ACTIVE) {
        // Add null checks
        if (!player) {
            std::cerr << "Player is null!" << std::endl;
            return;
        }
        if (!Collision) {
            std::cerr << "Collision system is null!" << std::endl;
            return;
        }
        if (!currentArea) {
            std::cerr << "Current area is null!" << std::endl;
            return;
        }

        // Store previous position to detect movement
        glm::vec2 oldPosition = player->Position;

        // Update game systems
        player->Update(dt);
        Collision->Update(player, dt);
        currentArea->Update(dt);
        
        if (Particles) {
            Particles->Update(dt, *player, 4, glm::vec2(60.0f, 135.0f));
        }

        // Check if player has moved
        bool hasMoved = glm::length(player->Position - oldPosition) > 0.1f;
        
        // Random battle check after movement
        if (hasMoved && !battleSystem->IsActive()) {
            static std::uniform_real_distribution<> dis(0.0, 1.0);
            
            // 5% chance of battle per movement
            if (dis(Random::getGenerator()) < 0.05) {
                if (debug) std::cout << "Random battle triggered after movement" << std::endl;
                StartBattle();
            }
        }

        Center();

        // Check if battle should start
        if (battle && !battleSystem->IsActive()) {
            if (debug) std::cout << "Starting battle..." << std::endl;
            
            // Make sure we have an enemy selected
            if (auto enemy = getCurrentEnemy()) {
                battleSystem = std::make_unique<Battle>(player, enemy.get(), Width, Height);
                battleSystem->Start();
            } else {
                std::cerr << "Warning: No enemy available for battle" << std::endl;
                battle = false;
            }
        }

        // Update battle if active
        if (battleSystem && battleSystem->IsActive()) {
            battleSystem->Update(dt);
        }
    }
}
void Game::Center(){
    // Camera update last
    float camX = player->Position.x - (Width / 2.0f);
    float camY = player->Position.y - (Height / 2.0f);
    Camera::Instance->FollowPlayer(glm::vec2(camX, camY));
}
void Game::ResetPlayer()
{
    if (debug) std::cout << "\n=== Resetting Player ===" << std::endl;
    
    if (debug && player) {
        std::cout << "Current player state before reset:" << std::endl;
        std::cout << "- Position: " << player->Position.x << ", " << player->Position.y << std::endl;
        std::cout << "- Form: " << player->form << std::endl;
    }
    
    player.reset();
    if (debug) std::cout << "Player object reset" << std::endl;

    glm::vec2 playerPos = glm::vec2(120.0f, this->Height / 3.0f);
    player = std::make_shared<Player>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("player.png"), 
        glm::vec3(1.0f, 1.0f, 1.0f), 5, 5
    );
    
    player->tile = 23;
    player->form = 0;
    
    if (debug) {
        std::cout << "New player created:" << std::endl;
        std::cout << "- Position: " << player->Position.x << ", " << player->Position.y << std::endl;
        std::cout << "- Size: " << PLAYER_SIZE.x << ", " << PLAYER_SIZE.y << std::endl;
        std::cout << "- Tile: " << player->tile << std::endl;
        std::cout << "- Form: " << player->form << std::endl;
        std::cout << "=== Player Reset Complete ===\n" << std::endl;
    }
}
void Game::ResetLevel()
{
    if (debug) std::cout << "\n=== Resetting Level ===" << std::endl;
    
    monsters.clear();
    if (debug) std::cout << "Cleared existing monsters" << std::endl;

    // Add Froggy with complete stats initialization
    auto froggy = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("frog.png")
    );
    froggy->name = "Froggy";
    froggy->stats = {150, 80, 70, 50, 50, "Water", "Froggy"};
    monsters.push_back(froggy);

    // Add Tortoise with complete stats
    auto tortoise = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("turtle.png")
    );
    tortoise->name = "Tortoise";
    tortoise->stats = {180, 80, 75, 95, 30, "Water", "Tortoise"};
    monsters.push_back(tortoise);

    // Add Scorpio with complete stats
    auto scorpio = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("scorpion.png")
    );
    scorpio->name = "Scorpio";
    scorpio->stats = {120, 120, 65, 55, 50, "Ground", "Scorpio"};
    monsters.push_back(scorpio);

    // Add Roawer with complete stats
    auto roawer = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("wolf.png")
    );
    roawer->name = "Roawer";
    roawer->stats = {150, 150, 80, 60, 60, "Ground", "Roawer"};
    monsters.push_back(roawer);

    // Add Insectus with complete stats
    auto insectus = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("insect.png")
    );
    insectus->name = "Insectus";
    insectus->stats = {90, 90, 50, 35, 40, "Insect", "Insectus"};
    monsters.push_back(insectus);

    if (debug) {
        std::cout << "Total monsters after reset: " << monsters.size() << std::endl;
        std::cout << "=== Level Reset Complete ===\n" << std::endl;
    }
}

void Game::InitializeCollision() {
    if (currentArea && currentArea->tilemapManager) {
        Collision->SetTilemapManager(currentArea->tilemapManager);
    }
}

// Helper function to get current enemy
std::shared_ptr<GameObject> Game::getCurrentEnemy() {
    if (monsters.empty()) return nullptr;
    
    // Get a random enemy from the available monsters
    std::uniform_int_distribution<size_t> dis(0, monsters.size() - 1);
    return monsters[dis(Random::getGenerator())];
}

void Game::StartBattle() {
    battle = true;
    if (debug) std::cout << "Battle flag set" << std::endl;
}
