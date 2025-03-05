#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include <chrono>
#include <array>

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

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(300.0f, 300.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(12500.0f);
bool gameOver = false;
// Add this in your Game constructor or Init method
std::random_device rd;
std::mt19937 gen = std::mt19937(rd());  // Seeded generator
std::uniform_real_distribution<float> dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), 
    Width(width), 
    Height(height),
    area(0),
    stepCounter(0.0f),
    lastRandomSeed(std::chrono::steady_clock::now())
{
    // Initialize RNG with multiple entropy sources for better cross-platform reliability
    std::array<std::uint32_t, 8> seed_data{
        static_cast<std::uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count()),
        static_cast<std::uint32_t>(rd()),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(std::random_device{}()),
        static_cast<std::uint32_t>(std::hash<std::string>{}("NeuroMonsters")),
        0xdeadbeef,  // Additional entropy
        0x8badf00d   // Additional entropy
    };
    
    std::seed_seq seq(seed_data.begin(), seed_data.end());
    rng = std::mt19937(seq);
    
    Camera::Instance = std::make_shared<Camera>(glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height));
    Dialogue = std::make_shared<DialogueSystem>();
    //audio = std::unique_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());
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
    // set render-specific controls
    const std::vector<float>  vertices = {
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
    ResetPlayer();
    ResetLevel();
    // Initialize the area manager
    currentArea = std::make_shared<Area>(Width, Height);
    // currentArea->State = State;
    currentArea->LoadTilemap("levels/main.lvl", "tiles.png", "bg.png", 7, 7);
    currentArea->enemies = monsters;
    Collision = std::make_unique<Collider>(Dialogue, currentArea->tilemapManager);
    Collision->SetBoundingBoxOffset(glm::vec2(50.0,25.0f));
    Collision->SetBoundingBoxSize(glm::vec2(60.0,120.0f));
    //audio->play2D((ResourceManager::root + "/audio/music.wav").c_str(), true);
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

            float buttonWidth = windowSize.x - 10.0f;
            float buttonHeight = windowSize.y * 0.2f;

            if (ImGui::Button("Resume", ImVec2(buttonWidth, buttonHeight))) {
                State = GAME_ACTIVE;
            }

            if (ImGui::Button("Main Menu", ImVec2(buttonWidth, buttonHeight))) {
                State = GAME_MENU;
                if (battleSystem) {
                    battleSystem->End();
                    battleSystem.reset();
                }
                // Reset everything when returning to main menu
                ResetLevel();
                ResetPlayer();
                gameCompleted = false;
                showCongrats = false;
                stepCounter = 0.0f;
                battle = false;
            }

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

        // New debug window for position
        ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("Position Debug", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        if (player) {
            float tileX = player->Position.x / 64.0f;
            float tileY = player->Position.y / 64.0f;
            ImGui::Text("Pos: (%.1f, %.1f)", player->Position.x, player->Position.y);
            ImGui::Text("Tile: (%d, %d)", static_cast<int>(tileX), static_cast<int>(tileY));
        }
        ImGui::PopStyleColor();
        ImGui::End();
    }
    // Show congratulations message if game is completed
    if (showCongrats) {
        const ImVec2 windowSize(500, 300);
        ImVec2 windowPos = ImVec2((Width - windowSize.x) * 0.5f, (Height - windowSize.y) * 0.5f);
        
        ImGui::SetNextWindowPos(windowPos);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::Begin("Congratulations!", nullptr, 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::GetFont()->Scale = 1.8f;
        
        ImGui::TextWrapped("Congratulations! You've completed your journey!");
        ImGui::Spacing();
        ImGui::TextWrapped("You've mastered the art of monster training and reached the end of your adventure.");
        
        ImGui::GetFont()->Scale = 1.2f;
        ImGui::Spacing();
        ImGui::Spacing();
        
        float buttonWidth = windowSize.x - 40.0f;
        float buttonHeight = 50.0f;
        
        if (ImGui::Button("Return to Main Menu", ImVec2(buttonWidth, buttonHeight))) {
            showCongrats = false;
            State = GAME_MENU;
            ResetLevel();
            ResetPlayer();
            gameCompleted = false;
        }
        
        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopFont();
        ImGui::End();
    }
    // Render the GUI
    Gui::Render();
}
void Game::ProcessInput(float dt)
{
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

        // Check if we can start a battle
        if (moved && !battleSystem && player->stats.health > 0) {
            stepCounter += dt * 60.0f;

            if (stepCounter >= STEP_THRESHOLD) {
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                float roll = dist(rng);

                // Reseed periodically using multiple entropy sources
                auto currentTime = std::chrono::steady_clock::now();
                if (currentTime - lastRandomSeed > std::chrono::seconds(5)) {
                    std::array<std::uint32_t, 8> entropy_data{
                        static_cast<std::uint32_t>(currentTime.time_since_epoch().count()),
                        static_cast<std::uint32_t>(player->Position.x * 1000),
                        static_cast<std::uint32_t>(player->Position.y * 1000),
                        static_cast<std::uint32_t>(rd()),
                        static_cast<std::uint32_t>(stepCounter * 1000),
                        static_cast<std::uint32_t>(dt * 1000000),
                        static_cast<std::uint32_t>(roll * 1000000),
                        static_cast<std::uint32_t>(std::hash<std::string>{}(player->name))
                    };
                    
                    std::seed_seq seq(entropy_data.begin(), entropy_data.end());
                    rng.seed(seq);
                    lastRandomSeed = currentTime;
                }

                if (roll < BATTLE_CHANCE) {
                    auto enemy = currentArea->GetRandomEnemy();
                    if (enemy) {
                        player->Stop();
                        Camera::Instance->SetPosition(glm::vec2(0.0f));
                        Camera::Instance->SetSize(glm::vec2(Width, Height));
                        battleSystem = std::make_unique<Battle>(monsters[player->form], enemy, Width, Height);
                        battleSystem->Start();
                        stepCounter = 0.0f; // Reset counter after battle starts
                    }
                }
                stepCounter = 0.0f; // Reset counter after check
            }
        }
    }
}
void Game::Update(float dt)
{
    if (State == GAME_ACTIVE && currentArea) {
        // Add debug position info at the start of update
        if (debug && player) {
            // Calculate current tile position
            float tileX = player->Position.x / 64.0f;  // Assuming 64 is tile width
            float tileY = player->Position.y / 64.0f;  // Assuming 64 is tile height
            
            std::cout << "Player Debug Info:" << std::endl;
            std::cout << "Position: (" << player->Position.x << ", " << player->Position.y << ")" << std::endl;
            std::cout << "Current Tile: (" << static_cast<int>(tileX) << ", " << static_cast<int>(tileY) << ")" << std::endl;
            std::cout << "------------------------" << std::endl;
        }

        // Process input first
        ProcessInput(dt);
        
        // Then update everything else
        if (gameOver) {
            this->ResetLevel();
            this->ResetPlayer();
        }

        if(battleSystem){
            battle = battleSystem->IsActive();
            if(battle){
                battleSystem->Update(dt);
                for (auto& key : Keys) {
                    key = false; // Reset key states
                }
            } else {
                // Store enemy's health before resetting battle system
                int enemyHealth = battleSystem->GetEnemyHealth();
                
                battleSystem.reset();
                stepCounter = 0.0f;

                if (monsters[player->form]->won && enemyHealth > 0) {
                    int healthRestored = static_cast<int>(enemyHealth * 0.5f);
                    int newHealth = monsters[player->form]->stats.health + healthRestored;
                    if (newHealth > monsters[player->form]->stats.maxHealth) {
                        newHealth = monsters[player->form]->stats.maxHealth;
                    }
                    monsters[player->form]->stats.health = newHealth;
                }
            }
        }

        if(!battle){
            if(monsters[player->form]->battleEnd){
                // Check if player lost (health <= 0)
                if(monsters[player->form]->stats.health <= 0) {
                    ResetLevel();
                    for(auto& monster : monsters) {
                        monster->stats.health = monster->stats.maxHealth;
                    }
                    monsters[player->form]->battleEnd = false;
                    player->battleEnd = false;
                    ResetPlayer();  // Reset player position
                    if(monsters[player->form]->lost){
                        monsters[player->form]->lost = false;
                    }
                } else {
                    // Player won or escaped, just reset battle flags
                    monsters[player->form]->battleEnd = false;
                    player->battleEnd = false;
                    if(monsters[player->form]->lost){
                        monsters[player->form]->lost = false;
                    }
                }
                stepCounter = 0.0f; // Reset step counter after battle ends
            }
            if(monsters[player->form]->won){
                monsters[player->form]->won = false;
                stepCounter = 0.0f; // Reset step counter after victory
            }

            // Check for game over due to 0 health outside of battle
            if (monsters[player->form]->stats.health <= 0) {
                ResetLevel();
                ResetPlayer();
            } else {
                player->Update(dt);
                Collision->Update(player, dt);
                currentArea->Update(dt);
                Particles->Update(dt, *player, 4, glm::vec2(60.0f, 135.0f));
                Center();
            }
            
            // Check for reaching the end of the game
            if (!gameCompleted && CheckEndGame()) {
                gameCompleted = true;
                showCongrats = true;
                State = GAME_PAUSED; // Pause the game when showing congratulations
            }
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
    player.reset();
    // Initialize player object
    glm::vec2 playerPos = glm::vec2(
        120.0f, 
        this->Height / 3.0f
    );
    player = std::make_unique<Player>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("player.png"), glm::vec3(1.0f, 1.0f, 1.0f), 5, 5
    );
    player->tile = 23;
    player->form = 0;
}
void Game::ResetLevel(){
    stepCounter = 0.0f; // Reset step counter when level resets
    // Clear the vector
    monsters.clear();

    // Add Froggy
    auto froggy = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("frog.png")
    );
    froggy->name = "Froggy";
    froggy->stats = {
        150,    // health
        80,     // attack
        70,     // defense
        50,     // speed
        50,     // maxHealth
        "Water",// type
        "Froggy"// name
    };
    froggy->moves = {
        {"Water Gun", "Water", 50, 95.0f, 30, "A basic water attack"},
        {"Aqua Wave", "Water", 100, 70.0f, 10, "A powerful water wave that may lower enemy speed"},
        {"Bubble Beam", "Water", 65, 90.0f, 20, "An attack that may lower enemy speed"},
        {"Pound", "Normal", 30, 100.0f, 40, "A basic normal attack"}
    };
    monsters.push_back(froggy);

    // Add Tortoise
    auto tortoise = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("turtle.png")
    );
    tortoise->name = "Tortoise";
    tortoise->stats = {
        180,    // health
        80,     // attack
        75,     // defense
        95,     // speed
        30,     // maxHealth
        "Water",// type
        "Tortoise" // name
    };
    tortoise->moves = {
        {"Shell Shield", "Water", 0, 100.0f, 10, "A defense move to increase defense for a few turns"},
        {"Bite", "Normal", 60, 90.0f, 15, "A normal biting attack"},
        {"Water Pulse", "Water", 40, 100.0f, 20, "A water attack that may confuse the enemy"},
        {"Defense Curl", "Normal", 0, 100.0f, 10, "A move that increases defense for a few turns"}
    };
    monsters.push_back(tortoise);

    // Add Scorpio
    auto scorpio = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("scorpion.png")
    );
    scorpio->name = "Scorpio";
    scorpio->stats = {
        120,    // health
        120,    // attack
        65,     // defense
        55,     // speed
        50,     // maxHealth
        "Ground",// type
        "Scorpio" // name
    };
    scorpio->moves = {
        {"Sandstorm", "Ground", 0, 85.0f, 15, "A powerful sandstorm that affects visibility and damages over time"},
        {"Poison Sting", "Ground", 40, 95.0f, 20, "A stinging attack that may poison the opponent"},
        {"Tail Whip", "Normal", 0, 100.0f, 30, "A move that lowers the enemy's defense"},
        {"Rock Slide", "Rock", 75, 90.0f, 10, "An attack that may cause the enemy to flinch"}
    };
    monsters.push_back(scorpio);

    // Add Roawer
    auto roawer = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("wolf.png")
    );
    roawer->name = "Roawer";
    roawer->stats = {
        150,    // health
        150,    // attack
        80,     // defense
        60,     // speed
        60,     // maxHealth
        "Ground",// type
        "Roawer" // name
    };
    roawer->moves = {
        {"Ground Slam", "Ground", 80, 85.0f, 15, "A ground-shaking attack that lowers enemy speed"},
        {"Howl", "Normal", 0, 100.0f, 10, "A move that boosts the user's attack"},
        {"Fang Strike", "Normal", 70, 95.0f, 15, "A biting attack that deals significant damage"},
        {"Earthquake", "Ground", 100, 75.0f, 5, "A powerful attack that hits all opponents"}
    };
    monsters.push_back(roawer);

    // Add Insectus
    auto insectus = std::make_shared<GameObject>(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(200.0f, 400.0f),
        ResourceManager::GetTexture2D("insect.png")
    );
    insectus->name = "Insectus";
    insectus->stats = {
        90,     // health
        90,     // attack
        50,     // defense
        35,     // speed
        40,     // maxHealth
        "Insect",// type
        "Insectus" // name
    };
    insectus->moves = {
        {"Bug Bite", "Insect", 50, 90.0f, 20, "A bite attack with a chance to confuse the enemy"},
        {"Toxic Powder", "Poison", 0, 90.0f, 10, "A powder that poisons the opponent over time"},
        {"Quick Attack", "Normal", 40, 100.0f, 20, "A fast attack that strikes first"},
        {"Leech Life", "Bug", 20, 100.0f, 15, "A move that restores some health based on damage dealt"}
    };
    monsters.push_back(insectus);
}

bool Game::CheckEndGame() {
    // The end zone is around tile 137 (x position ~8810)
    const float END_ZONE_X = 8810.0f;
    const float TRIGGER_ZONE_WIDTH = 128.0f; // Two tiles width for reliable detection
    
    // Create a detection zone around the end point
    bool inEndZone = (player->Position.x >= END_ZONE_X && 
                     player->Position.x <= (END_ZONE_X + TRIGGER_ZONE_WIDTH));

    // Debug output to help verify position
    if (debug) {
        // Calculate current tile position
        float tileX = player->Position.x / 64.0f;
        
        // More detailed debug output
        std::cout << "End Game Check:" << std::endl;
        std::cout << "Current X: " << player->Position.x << std::endl;
        std::cout << "Current Tile: " << static_cast<int>(tileX) << std::endl;
        std::cout << "End Zone X: " << END_ZONE_X << " to " << (END_ZONE_X + TRIGGER_ZONE_WIDTH) << std::endl;
        if (inEndZone) {
            std::cout << "*** PLAYER REACHED END ZONE! ***" << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }

    return inEndZone;
}
