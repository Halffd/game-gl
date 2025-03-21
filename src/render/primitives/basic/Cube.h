#ifndef CUBE_SHAPE_H
#define CUBE_SHAPE_H

#include "../PrimitiveShape.h"
#include "asset/Texture2D.h"

namespace m3D {

// Cube shape
class Cube : public PrimitiveShape {
public:
    // Add this declaration
    Cube();
    
    // Existing parameterized constructor
    Cube(const std::string& name,
         const glm::vec3& position = glm::vec3(0.0f),
         const glm::vec3& rotation = glm::vec3(0.0f),
         const glm::vec3& scale = glm::vec3(1.0f),
         const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f));

    // Add Draw method override
    void Draw(Shader &shader) override;
    
    // Add missing member variables
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    bool hasTexture;
    Texture2D texture;
    unsigned int VAO;
    
private:
    unsigned int VBO, EBO;
};

} // namespace m3D

#endif // CUBE_SHAPE_H 