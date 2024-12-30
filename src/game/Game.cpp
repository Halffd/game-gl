#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), 
    Width(width), 
    Height(height),
    area(0)
{
    Camera::Instance = std::make_shared<Camera>(glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height));
    Dialogue = std::make_shared<DialogueSystem>();
    audio = std::unique_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());
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

    // Initialize player object
    glm::vec2 playerPos = glm::vec2(
        120.0f, 
        this->Height / 3.0f
    );
    player = std::make_unique<Player>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("player.png"), glm::vec3(1.0f, 1.0f, 1.0f), 5, 5
    );
    player->tile = 0;
    player->form = 0;
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("frog.png")
        )
    );
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("turtle.png")
        )
    );
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("Scorpio.png")
        )
    );
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("wolf.png")
        )
    );
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("insect.png")
        )
    );
    monsters.push_back(
        std::make_shared<GameObject>(
            glm::vec2(0.0f, 0.0f),
            glm::vec2(200.0f, 400.0f),
            ResourceManager::GetTexture2D("turtle.png")
        )
    );
    monsters[0]->name = "Froggy";
    monsters[1]->name = "Tartoise";
    monsters[2]->name = "Scorpio";
    monsters[3]->name = "Roawer";
    monsters[4]->name = "Inesctus";
    // Initialize the area manager
    currentArea = std::make_shared<Area>(Width, Height);
    currentArea->State = State;
    currentArea->LoadTilemap("levels/main.lvl", "tiles.png", "bg.png", 7, 7);
    currentArea->enemies = monsters;
    Collision = std::make_unique<Collider>(Dialogue, currentArea->tilemapManager);
    Collision->SetBoundingBoxOffset(glm::vec2(50.0,25.0f));
    Collision->SetBoundingBoxSize(glm::vec2(60.0,120.0f));
    audio->play2D((ResourceManager::root + "/audio/breakout.wav").c_str(), true);
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
        if ((State == GAME_PAUSED || State == GAME_ACTIVE) && currentArea) {
            player->Draw(*Renderer);
            Particles->Draw();
        } 
    }
    if(State != GAME_ACTIVE) {
        // Set custom style for ImGui
        ImGui::StyleColorsDark(); // Use dark theme
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0); // Black background
        style.Colors[ImGuiCol_Border] = ImColor(0, 100, 0); // Dark green borders
        style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255); // White text
        style.Colors[ImGuiCol_Button] = ImColor(0, 100, 0); // Dark green button background
        style.Colors[ImGuiCol_ButtonHovered] = ImColor(0, 150, 0); // Lighter green on hover
        style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 80, 0); // Even darker green when pressed

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
void Game::ProcessInput(float dt)
{
    static bool paused = false;
    static float stepCounter = 0.0f;  // Tracks distance moved for battle checks
    static const float STEP_THRESHOLD = 12.0f;  // Adjust based on your tile size
    static const float BATTLE_CHANCE = 0.4f;    // 10% chance of battle when threshold reached
    
    if (State == GAME_ACTIVE) {
        bool moved = false;
        glm::vec2 oldPos = player->Position;

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
        } else {
            battle = true;
        }
        // Check for battle initiation only if player moved
        if (moved && battle) {
            Camera::Instance->SetPosition(glm::vec2(0.0f, 0.0f));

            // Calculate distance moved this frame
            glm::vec2 newPos = player->Position;
            float distanceMoved = 0.1f; //glm::length(newPos - oldPos);
            
            // Add to step counter
            stepCounter += distanceMoved;

            // Check for battle when step threshold is reached
            if (stepCounter >= STEP_THRESHOLD) {
                // Reset step counter
                stepCounter = 0.0f;

                // Random battle check
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

                if (dis(gen) < BATTLE_CHANCE) {
                    // Initialize and start battle
                    auto enemy = currentArea->GetRandomEnemy();
                    if (enemy) {
                        battleSystem = std::make_unique<Battle>(monsters[player->form], enemy);
                        battleSystem->Start();
                        player->Stop(); // Stop player movement during battle
                        Camera::Instance->SetPosition(glm::vec2(0.0f, 0.0f));
                    }
                } else {
                    battle = false;
                }
            } else {
                battle = false;
            }
        } else {
            battle = false;
        }
    }
}
void Game::Update(float dt)
{
    if (State == GAME_ACTIVE && currentArea) {
        // Process input first
        ProcessInput(dt);
        
        // Then update everything else
        if (gameOver) {
            this->ResetLevel();
            this->ResetPlayer();
        }
        if(!battle){
            player->Update(dt);  // Update player position
            Collision->Update(player, dt);  // Then handle collisions
            currentArea->Update(dt);
            Particles->Update(dt, *player, 4, glm::vec2(60.0f, 135.0f));
            
            Center();
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
    player->Size = PLAYER_SIZE;
    player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
}
void Game::ResetLevel(){
    
}
