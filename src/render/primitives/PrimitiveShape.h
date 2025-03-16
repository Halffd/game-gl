#ifndef PRIMITIVE_SHAPE_H
#define PRIMITIVE_SHAPE_H

#include "../../Mesh.hpp"
#include "../SceneObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

namespace m3D {

// Helper function to create a simple color texture
inline unsigned int createColorTexture(float r, float g, float b) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Create a simple 1x1 color texture
    unsigned char data[] = {
        static_cast<unsigned char>(r * 255),
        static_cast<unsigned char>(g * 255),
        static_cast<unsigned char>(b * 255),
        255 // Alpha = 1
    };
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    std::cout << "Created color texture with ID: " << textureID << std::endl;
    return textureID;
}

// Base class for primitive shapes
class PrimitiveShape : public SceneObject {
protected:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 color;
    std::map<std::string, float> materialProperties;
    
public:
    PrimitiveShape(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f),
                  const glm::vec3& color = glm::vec3(1.0f))
        : SceneObject(name, position, rotation, scale), color(color) {
        // Initialize default material properties
        materialProperties["ambient"] = 0.2f;
        materialProperties["diffuse"] = 0.8f;
        materialProperties["specular"] = 0.5f;
        materialProperties["shininess"] = 32.0f;
        std::cout << "Creating PrimitiveShape: " << name << std::endl;
    }
    
    virtual ~PrimitiveShape() = default;
    
    virtual void Draw(Shader& shader) override {
        if (!visible || !mesh) {
            std::cout << "Not drawing " << name << ": visible=" << visible << ", mesh=" << (mesh ? "valid" : "null") << std::endl;
            return;
        }
        
        // Set model matrix
        shader.SetMatrix4("model", GetModelMatrix());
        
        // Set material properties
        shader.SetVector3f("material.color", color);
        shader.SetFloat("material.ambient", GetMaterialProperty("ambient"));
        shader.SetFloat("material.diffuse", GetMaterialProperty("diffuse"));
        shader.SetFloat("material.specular", GetMaterialProperty("specular"));
        shader.SetFloat("material.shininess", GetMaterialProperty("shininess"));
        
        // Draw the mesh
        mesh->Draw(shader);
    }
    
    // Create a texture with the specified color
    std::vector<Texture> createColorTextures() {
        std::vector<Texture> textures;
        Texture texture;
        texture.id = createColorTexture(color.r, color.g, color.b);
        texture.type = "texture_diffuse";
        texture.path = "generated_color";
        textures.push_back(texture);
        return textures;
    }
    
    // Helper function to add a vertex to the mesh
    void addVertex(std::vector<Vertex>& vertices, const glm::vec3& position, 
                  const glm::vec3& normal, const glm::vec2& texCoords) {
        Vertex vertex;
        vertex.Position = position;
        vertex.Normal = normal;
        vertex.TexCoords = texCoords;
        
        // Calculate tangent and bitangent
        glm::vec3 tangent;
        if (std::abs(normal.y) < 0.999f) {
            tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
        } else {
            tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), normal));
        }
        vertex.Tangent = tangent;
        vertex.Bitangent = glm::normalize(glm::cross(normal, tangent));
        
        // Initialize bone weights to 0
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = 0;
            vertex.m_Weights[i] = 0.0f;
        }
        
        vertices.push_back(vertex);
    }
    
    // Helper function to add a face (triangle) to the mesh
    void addFace(std::vector<unsigned int>& indices, unsigned int v1, unsigned int v2, unsigned int v3) {
        indices.push_back(v1);
        indices.push_back(v2);
        indices.push_back(v3);
    }
    
    // Set a material property
    void SetMaterialProperty(const std::string& property, float value) {
        materialProperties[property] = value;
    }
    
    // Get a material property
    float GetMaterialProperty(const std::string& property) const {
        auto it = materialProperties.find(property);
        if (it != materialProperties.end()) {
            return it->second;
        }
        return 0.0f; // Default value if property not found
    }
    
    // Getter and setter for color
    glm::vec3 GetColor() const { return color; }
    void SetColor(const glm::vec3& newColor) { color = newColor; }
};

} // namespace m3D

#endif // PRIMITIVE_SHAPE_H 