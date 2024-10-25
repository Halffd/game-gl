#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "../ResourceManager.h"
#include "SpriteRenderer.h"
#include "../Util.hpp"
#include "root.h"
#include <iostream>
#include "../Gui.h"

SpriteRenderer  *Renderer;
SpriteRenderer  *Renderer2;

Game::Game(unsigned int width, unsigned int height)
{
    this->Width = width;
    this->Height = height;
}
Game::~Game() {
    delete Renderer;
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
    ResourceManager::LoadTexture2D("copper.png");
    ResourceManager::LoadTexture2D("yellowstone.jpg");
    ResourceManager::LoadTexture2D("dbricks.png");
    ResourceManager::LoadTexture2D("container.jpg", "main");
    ResourceManager::LoadTexture2D("rect.jpg", "rect");
    ResourceManager::LoadTexture2D("glowstone.jpg", "light");
    ResourceManager::LoadTexture2D("pumpkin.png");
    ResourceManager::LoadTexture2D("sky.png"), "sky";
    ResourceManager::LoadTexture2D("maps/diffuse_container.png", "diffuse");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-01-18-39-42_cf_m_face_00_Texture2.png", "diffusex");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-18-18-28-39_cf_m_tang_DetailMask.png", "diffusez");
    ResourceManager::LoadTexture2D("maps/specular_container.png", "speculars");
    ResourceManager::LoadTexture2D("maps/_Export_2024-01-01-18-40-01_cf_m_face_00_NormalMask.png", "specularf");
    ResourceManager::LoadTexture2D("maps/WaterBottle_specularGlossiness.png", "specular");
    ResourceManager::LoadTexture2D("white.png", "white");
}

void Game::Render()
{
    Gui::Start();
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
    Gui::Render();
}
void Game::ProcessInput(float dt) {

}

void Game::Update(float dt) {
    // Implementation here
}