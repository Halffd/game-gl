#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "../ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObject.h"
#include "../Util.hpp"
#include "root.h"
#include <iostream>
#include "../Gui.h"
#include "../GameMode.h"

GameType mode = GAME2D;
SpriteRenderer  *Renderer;
SpriteRenderer  *Renderer2;
// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

GameObject      *Player;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;
  
BallObject     *Ball; 

Game::Game(unsigned int width, unsigned int height)
{
    this->Width = width;
    this->Height = height;
}
Game::~Game() {
    delete Renderer;
    delete Renderer2;
}
void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("sprite/vertex.glsl", "sprite/fragment.glsl", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
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
    // Define triangle vertices
    const std::vector<float> verticesTriangle = {
        // pos        // tex
        0.5f,  0.5f,  0.5f, 1.0f,  // Top vertex
        0.0f,  0.0f,  0.0f, 0.0f,  // Bottom left vertex
        1.0f, 0.0f,  1.0f, 0.0f   // Bottom right vertex
    };

    // Create SpriteRenderers
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"), vertices);
    Renderer2 = new SpriteRenderer(ResourceManager::GetShader("sprite"), verticesTriangle);

    // load textures
    ResourceManager::LoadTexture2D("awesomeface.png", "face");
    ResourceManager::LoadTexture2D("bookshelf.jpg", "bookshelf");
    ResourceManager::LoadTexture2D("copper.png", "copper");
    ResourceManager::LoadTexture2D("yellowstone.jpg");
    ResourceManager::LoadTexture2D("dbricks.png");
    ResourceManager::LoadTexture2D("dbrick.png", "dbrick");
    ResourceManager::LoadTexture2D("container.jpg", "main");
    ResourceManager::LoadTexture2D("rect.jpg", "rect");
    ResourceManager::LoadTexture2D("pumpkin.png");
    ResourceManager::LoadTexture2D("sky.png", "sky");
    ResourceManager::LoadTexture2D("maps/diffuse_container.png", "diffuse");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-01-18-39-42_cf_m_face_00_Texture2.png", "diffusex");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-18-18-28-39_cf_m_tang_DetailMask.png", "diffusez");
    ResourceManager::LoadTexture2D("maps/specular_container.png", "speculars");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-01-18-40-01_cf_m_face_00_NormalMask.png", "specularf");
    ResourceManager::LoadTexture2D("maps/WaterBottle_specularGlossiness.png", "specular");
    ResourceManager::LoadTexture2D("white.png", "white");
    ResourceManager::LoadTexture2D("block.png", "block");
    ResourceManager::LoadTexture2D("block_solid.png", "block_solid");
    ResourceManager::LoadTexture2D("bg/wallhaven-1p6dj9.jpg", "mountain");
    ResourceManager::LoadTexture2D("bg/wallhaven-qz7okl.png", "bg1");
    ResourceManager::LoadTexture2D("glasspaddle2_1.png", "paddle");
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, 
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture2D("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, 
                                              -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture2D("face"));
    // load levels
    GameLevel one; 
    one.Load("levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; 
    two.Load("levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; 
    three.Load("levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; 
    four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
}

void Game::Render()
{
    Gui::Start();
    if(mode == PLANE){
        float middleX = (float) this->Width / 2.0f;
        float middleY = (float) this->Height / 2.0f;
        float arrowW = 60.0f;
        float arrowH = 90.0f;

        glm::mat2 trans = {
            glm::vec2(16.0f / 17.0f, 4.0f / 17.0f),
            glm::vec2(4.0f / 17.0f, 1.0f / 17.0f)
        };
        glm::mat4 model = SpriteRenderer::Transform(glm::vec2(200.0f, 200.0f), glm::vec2(100.0f, 150.0f), 24.0f);
        glm::mat4 transformtion = model * mat2To4(trans);
        logger.log("Model", model);
        logger.log("Trana", transformtion);
        //transformtion *= glm::vec4(4.0f,0.0f,0.0f,2.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture2D("face"),
                            model, glm::vec3(0.0f, 1.0f, 0.0f));
        Renderer->DrawSprite(ResourceManager::GetTexture2D("face"),
                            transformtion, glm::vec3(0.0f, 0.0f, 1.0f));
        Renderer->DrawSprite(ResourceManager::GetTexture2D("white"),
                            glm::vec2(middleX, 0.0f), glm::vec2(2.0f, (float) this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Renderer->DrawSprite(ResourceManager::GetTexture2D("white"),
                            glm::vec2(0.0f, middleY), glm::vec2((float) this->Width, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Renderer2->DrawSprite(ResourceManager::GetTexture2D("white"),
                            glm::vec2(this->Width - arrowW / 2.0f, middleY - arrowW / 2.0f), glm::vec2(arrowW, arrowW), -90.0f, glm::vec3(1.0f, 0.8f, 1.0f));
        Renderer2->DrawSprite(ResourceManager::GetTexture2D("white"),
                            glm::vec2(middleX - arrowW / 2.0f, -30.0f), glm::vec2(arrowW, arrowW), -180.0f, glm::vec3(1.0f, 0.8f, 1.0f));
    } else {
        if(this->State == GAME_ACTIVE)
        {
            // draw background
            Renderer->DrawSprite(ResourceManager::GetTexture2D("bg/bg2.png"), 
                glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
            );
            // draw level
            this->Levels[this->Level].Draw(*Renderer);
            Player->Draw(*Renderer);  
            Ball->Draw(*Renderer);
        }
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
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Update(float dt) {
    // Implementation here
    Ball->Move(dt, this->Width);

    this->Collision();
}

bool Game::CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}  

void Game::Collision()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            if (CheckCollision(*Ball, box))
            {
                if (!box.IsSolid)
                    box.Destroyed = true;
            }
        }
    }
}  