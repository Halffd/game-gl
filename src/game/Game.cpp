#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "asset/ResourceManager.h"
#include "render/SpriteRenderer.h"
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
#include "Area.h"
#include "asset/TilemapManager.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
bool gameOver = false;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), 
    Width(width), 
    Height(height),
    area(0)
{
    Dialogue = std::make_shared<DialogueSystem>();
    Collision = std::make_unique<Collider>(Dialogue);
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
    ResourceManager::LoadTexture2D("bg/bg2.png", "background");

    // Initialize particle generator
    Particles = std::make_unique<ParticleGenerator>(
        ResourceManager::GetShader("particle"), 
        ResourceManager::GetTexture2D("particle"), 
        500
    );

    // Initialize player object
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, 
        this->Height - PLAYER_SIZE.y
    );
    player = std::make_unique<Player>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("char/Froggo 1_0/Froggo/Spritesheets/Fall.png")
    );

    // Initialize the area manager
    currentArea = std::make_shared<Area>(GAME);
    currentArea->LoadTilemap("levels/main.lvl", this->Width, this->Height);

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

    if (debug) {
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("FPS: %.1f", fps);
        ImGui::PopStyleColor();
        ImGui::End();
    }

    if (State == GAME_ACTIVE && currentArea) {
        Renderer->DrawSprite(ResourceManager::GetTexture2D("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        currentArea->Draw(*Renderer);
        player->Draw(*Renderer);
        Particles->Draw();
    } else if (State == GAME_PAUSED) {
        Gui::Start();
        ImGui::Begin("Pause Menu");
        ImGui::Text("Game Paused");
        if (ImGui::Button("Resume")) State = GAME_ACTIVE;
        if (ImGui::Button("Quit")) State = GAME_MENU;
        ImGui::End();
    }

    Gui::Render();
}

void Game::ProcessInput(float dt)
{
    if (State == GAME_ACTIVE) {
        if (this->Keys[GLFW_KEY_A]) {
            player->Move(Player::Direction::LEFT);
        }
        if (this->Keys[GLFW_KEY_D]) {
            player->Move(Player::Direction::RIGHT);
        }
        if (this->Keys[GLFW_KEY_W]) {
            player->Move(Player::Direction::UP);
        }
        if (this->Keys[GLFW_KEY_S]) {
            player->Move(Player::Direction::DOWN);
        }
        if (this->Keys[GLFW_KEY_P]) {
            State = GAME_PAUSED;
        }
    } else if (State == GAME_PAUSED) {
        if (this->Keys[GLFW_KEY_P]) {
            State = GAME_ACTIVE;
        }
    }
}

void Game::Update(float dt)
{
    if (State == GAME_ACTIVE && currentArea) {
        Collision->Update(player, dt);

        if (gameOver) {
            this->ResetLevel();
            this->ResetPlayer();
        }

        Particles->Update(dt, *player, 4, glm::vec2(10.0f));
        currentArea->Update(dt);
        player->Update(dt);
    }
}

void Game::ResetPlayer()
{
    player->Size = PLAYER_SIZE;
    player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
}
void Game::ResetLevel(){
    currentArea->LoadTilemap("levels/main.lvl", this->Width, this->Height);
}
