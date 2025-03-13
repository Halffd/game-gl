#include "render/SpriteRenderer.h"
#include "util/common.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Transform.hpp"
#include "game/Camera.h"

glm::mat4 mat2To4(const glm::mat2& mat2){
    return glm::mat4(
        mat2[0][0], mat2[0][1], 0.0f, 0.0f,  // First row
        mat2[1][0], mat2[1][1], 0.0f, 0.0f,  // Second row
        0.0f,      0.0f,      1.0f, 0.0f,  // Third row
        0.0f,      0.0f,      0.0f, 1.0f   // Fourth row
    );
}

SpriteRenderer::SpriteRenderer(const Shader &shader, const std::vector<float> &vertices)
{
    this->shader = shader;
    this->initRenderData(vertices);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color, glm::vec2 textureOffset, glm::vec2 textureSize, glm::mat4 view, bool mirror)
{
    // Prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);
    this->shader.SetVector2f("textureOffset", textureOffset);
    this->shader.SetVector2f("textureSize", textureSize);
    this->shader.SetInteger("mirror", mirror ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(const Texture2D &texture, glm::mat4 model, glm::vec3 color, glm::vec2 textureOffset, glm::vec2 textureSize, glm::mat4 view)
{
    this->shader.Use();
    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);
    this->shader.SetVector2f("textureOffset", textureOffset);
    this->shader.SetVector2f("textureSize", textureSize);
    this->shader.SetMatrix4("view", view);
    this->shader.SetInteger("mirror", 0);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData(const std::vector<float> &vertices)
{
    // Configure VAO/VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}