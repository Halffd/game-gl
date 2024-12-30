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
    ResourceManager::LoadTexture2D("misc/particle.png", "particle");

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
    // Initialize the area manager
    currentArea = std::make_shared<Area>(Width, Height);
    currentArea->State = State;
    currentArea->LoadTilemap("levels/main.lvl", "tiles.png", "bg.png", 7, 7);
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

    if(currentArea){
        currentArea->Draw(*Renderer); 
    }
    // Render based on the current game state
    if ((State == GAME_PAUSED || State == GAME_ACTIVE) && currentArea) {
        player->Draw(*Renderer);
        Particles->Draw();
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

    if (State == GAME_ACTIVE) {
        // Player movement
        if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT]) {
            player->Move(Direction::LEFT);
        } 
        else if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT]) {
            player->Move(Direction::RIGHT);
        } 
        else if (this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_UP]) {
            player->Move(Direction::UP);
        } 
        else if (this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_DOWN]) {
            player->Move(Direction::DOWN);
        } 
        else {
            player->Stop();
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

        player->Update(dt);  // Update player position
        Collision->Update(player, dt);  // Then handle collisions
        currentArea->Update(dt);
        Particles->Update(dt, *player, 4, glm::vec2(60.0f, 135.0f));
        
        // Camera update last
        float camX = player->Position.x - (Width / 2.0f);
        float camY = player->Position.y - (Height / 2.0f);
        Camera::Instance->FollowPlayer(glm::vec2(camX, camY));
    }
}

void Game::ResetPlayer()
{
    player->Size = PLAYER_SIZE;
    player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
}
void Game::ResetLevel(){
    
}
