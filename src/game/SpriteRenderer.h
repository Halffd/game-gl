/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Texture2D.h"
#include "../Shader.h"
#include <vector>

// constexpr function to convert glm::mat2 to glm::mat4
glm::mat4 mat2To4(const glm::mat2& mat2);

class SpriteRenderer
{
public:
    // Constructor (inits shaders/shapes)

    SpriteRenderer(const Shader &shader, const std::vector<float> &vertices);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawSprite(const Texture2D &texture, glm::mat4 model, glm::vec3 color);

    static glm::mat4 Transform(glm::vec2 position, glm::vec2 size, float rotate);

    void Bind();
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData(const std::vector<float> &vertices);
};

#endif