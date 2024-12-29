#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.h"
#include "asset/ResourceManager.h"
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
    
    void DrawSprite(const Texture2D& texture, 
                    glm::vec2 position, 
                    glm::vec2 size = glm::vec2(10.0f, 10.0f), 
                    float rotate = 0.0f, 
                    glm::vec3 color = glm::vec3(1.0f), 
                    glm::vec2 textureOffset = glm::vec2(0.0f, 0.0f), 
                    glm::vec2 textureSize = glm::vec2(1.0f, 1.0f));
    void DrawSprite(const Texture2D& texture, 
                    glm::mat4 model, 
                    glm::vec3 color = glm::vec3(1.0f), 
                    glm::vec2 textureOffset = glm::vec2(0.0f, 0.0f), 
                    glm::vec2 textureSize = glm::vec2(9.0f, 1.0f));
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