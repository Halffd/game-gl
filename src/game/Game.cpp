#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "asset/ResourceManager.h"
#include "render/SpriteRenderer.h"
#include "GameObject.h"
#include "CircleObject.h"
#include "util/Util.h"
#include "root.h"
#include <iostream>
#include "ui/Gui.h"
#include "gamemode.h"
#include "types.h"
#include "effects/Particle.h"
#include "Collider.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
bool gameOver = false;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), 
    Width(width), 
    Height(height)
{
    Renderer = std::make_unique<SpriteRenderer>();
    Collision = std::make_unique<Collider>();
    audio = std::unique_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());
}
Game::~Game() { }
void Game::Init()
{
    // Timing variables
    lastTime = glfwGetTime();
    frameCount = 0;
    fps = 0.0f;
    // load shaders
    ResourceManager::LoadShader("sprite/vertex.glsl", "sprite/fragment.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("particle.vs", "particle.fs", nullptr, "particle");

    // configure shaders
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
    // load textures
    ResourceManager::LoadTexture2D("misc/particle.png",  "particle"); 
    Particles = std::make_unique<ParticleGenerator>(
        ResourceManager::GetShader("particle"), 
        ResourceManager::GetTexture2D("particle"), 
        500
    );
    
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, 
        this->Height - PLAYER_SIZE.y
    );
    Player = std::make_unique<GameObject>(
        playerPos, PLAYER_SIZE, 
        ResourceManager::GetTexture2D("paddle")
    );

    // load levels
    Level one; 
    one.Load("levels/one.lvl", this->Width, this->Height / 2);
    Level two; 
    two.Load("levels/two.lvl", this->Width, this->Height / 2);
    Level three; 
    three.Load("levels/three.lvl", this->Width, this->Height / 2);
    Level four; 
    four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
    
    audio->play2D((ResourceManager::root + "/audio/breakout.wav").c_str(), true); // Play the sound in a non-blocking way
}

void Game::Render()
{
    Gui::Start();
    // Calculate FPS
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) { // Update every second
        fps = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
    if(debug){
        // Display FPS text at the top left
        ImGui::SetNextWindowBgAlpha(0.0f); // Make background transparent
        ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetCursorPos(ImVec2(10, 10)); // Position the text
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow color
        //ImGui::PushFont(io.Fonts->Fonts[0]); // Use the default font
        ImGui::Text("FPS: %.1f", fps);
        //ImGui::PopFont();

        ImGui::PopStyleColor();
        ImGui::End();
    }
    if(this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture2D("bg/bg2.png"), 
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
        );
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        Player->Draw(*Renderer);  
        // draw particles	
        Particles->Draw();
    }
    Gui::Render();
}
void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
            }
        }
    }
}

void Game::Update(float dt) {
    // Implementation here
    Collision->Collisions();

    if (gameOver) // did ball reach bottom edge?
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
    // update particles
    Particles->Update(dt, *Player, 4, glm::vec2(Ball->Radius / 2.0f));
}


void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
}