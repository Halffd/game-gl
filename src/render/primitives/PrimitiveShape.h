#ifndef PRIMITIVE_SHAPE_H
#define PRIMITIVE_SHAPE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include <string>

#include "../../Mesh.hpp"
#include "../SceneObject.h"
#include "../Shader.h"

namespace m3D {
// Helper function to create a simple color texture
unsigned int createColorTexture(float r, float g, float b);

// Base class for primitive shapes
// Forward declarations
struct Texture;

class PrimitiveShape : public SceneObject {
protected:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 color;
    std::map<std::string, float> materialProperties;
    Shader* customShader = nullptr;
    std::map<std::string, glm::vec3> shaderVec3Params;
    std::map<std::string, float> shaderFloatParams;
    
    // Shader parameter maps
    friend class Renderer3D;
    
public:
    PrimitiveShape(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f),
                  const glm::vec3& color = glm::vec3(1.0f));
    
    virtual ~PrimitiveShape() = default;
    
    // Drawing and rendering - overrides SceneObject::Draw
    void Draw(Shader& shader) override;
    
    // Shader management
    void SetShader(Shader& shader) { customShader = &shader; }
    
    // Color and material management
    void SetColor(const glm::vec3& newColor);
    glm::vec3 GetColor() const;
    
    // Mesh management
    void setMesh(std::shared_ptr<Mesh> newMesh);
    std::shared_ptr<Mesh> getMesh() const;
    
    // Shader management
    void setCustomShader(Shader* shader);
    Shader* getShader() const override;
    void setShaderVec3(const std::string& name, const glm::vec3& value);
    void setShaderFloat(const std::string& name, float value);
    
    // Material properties
    void setMaterialProperty(const std::string& name, float value);
    float GetMaterialProperty(const std::string& property) const;
    
protected:
    // Helper functions for derived classes
    std::vector<Texture> createColorTextures();
    void addVertex(std::vector<Vertex>& vertices, const glm::vec3& position, 
                  const glm::vec3& normal, const glm::vec2& texCoords);
    void addFace(std::vector<unsigned int>& indices, 
                unsigned int v1, unsigned int v2, unsigned int v3);
};

} // namespace m3D

#endif // PRIMITIVE_SHAPE_H