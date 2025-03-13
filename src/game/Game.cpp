#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./Game.h"
#include "asset/ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObject.h"
#include "../util/Util.h"
#include "root.h"
#include <iostream>
#include "ui/Gui.h"
#include "GameMode.h"
#include "effects/Particle.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

GameObject *Player;
GameMode mode = GAME; // Add the mode variable with default value

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

BallObject *Ball;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), 
      Width(width), 
      Height(height),
      Level(0),
      lastTime(0.0),
      frameCount(0),
      fps(0.0f)
{
}

Game::~Game() {
    delete Player;
    delete Ball;
}

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
    // Define triangle vertices
    const std::vector<float> verticesTriangle = {
        // pos        // tex
        0.5f,  0.5f,  0.5f, 1.0f,  // Top vertex
        0.0f,  0.0f,  0.0f, 0.0f,  // Bottom left vertex
        1.0f, 0.0f,  1.0f, 0.0f   // Bottom right vertex
    };

    // Create SpriteRenderers
    Renderer = std::make_unique<SpriteRenderer>(ResourceManager::GetShader("sprite"), vertices);
    Renderer2 = std::make_unique<SpriteRenderer>(ResourceManager::GetShader("sprite"), verticesTriangle);

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
    ResourceManager::LoadTexture2D("particle.png",  "particle"); 
    Particles = std::make_unique<ParticleGenerator>(
        ResourceManager::GetShader("particle"), 
        ResourceManager::GetTexture2D("particle"), 
        500
    );
    
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

     // Display FPS text at the top left
    ImGui::SetNextWindowBgAlpha(0.0f); // Make background transparent
    ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetCursorPos(ImVec2(10, 10)); // Position the text
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow color
    ImGui::Text("FPS: %.1f", fps);
    ImGui::PopStyleColor();
    ImGui::End();
    
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
            // draw particles	
            Particles->Draw();
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

    // update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
    this->Collisions();

    if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
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
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}
Direction Game::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // up
        glm::vec2(1.0f, 0.0f),  // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f)  // left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return static_cast<Direction>(best_match);
}

bool Game::CheckCollision(GameObject &one, GameObject &two)
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

std::tuple<bool, Direction, glm::vec2> Game::CheckCollision(BallObject &one, GameObject &two)
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::Collisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);  

                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball bback up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }               
            }
        }    
    }
    // check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}  
