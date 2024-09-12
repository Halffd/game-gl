#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "../ResourceManager.h"
#include "SpriteRenderer.h"
#include "../Util.hpp"
#include "root.h"
#include <iostream>

SpriteRenderer  *Renderer;

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader(fs.shader("sprite/vertex.glsl"), fs.shader("sprite/fragment.glsl"), nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture2D(fs.texture("awesomeface.png"), "face");
}

void Game::Render()
{
    Renderer->DrawSprite(ResourceManager::GetTexture2D("face"),
        glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}