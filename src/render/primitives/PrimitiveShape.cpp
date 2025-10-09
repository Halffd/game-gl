#include <glad/glad.h>
#include <iostream>

#include "PrimitiveShape.h"
#include "../Shader.h"

namespace m3D {

// Helper function to create a simple color texture
unsigned int createColorTexture(float r, float g, float b) {
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

// Constructor implementation
PrimitiveShape::PrimitiveShape(const std::string& name, 
                             const glm::vec3& position,
                             const glm::vec3& rotation,
                             const glm::vec3& scale,
                             const glm::vec3& color)
    : SceneObject(name, position, rotation, scale), color(color) {
    // Initialize default material properties
    materialProperties["ambient"] = 0.2f;
    materialProperties["diffuse"] = 0.8f;
    materialProperties["specular"] = 0.5f;
    materialProperties["shininess"] = 32.0f;
    std::cout << "Creating PrimitiveShape: " << name << std::endl;
}

void PrimitiveShape::Draw(Shader& shader) {
    if (!visible || !mesh) {
        return;
    }
    
    shader.SetMatrix4("model", GetModelMatrix());
    
    // Only set custom parameters if the shader is the custom shader
    if (customShader && shader.ID == customShader->ID) {
        for (const auto& pair : shaderVec3Params) {
            shader.SetVector3f(pair.first.c_str(), pair.second);
        }
        for (const auto& pair : shaderFloatParams) {
            shader.SetFloat(pair.first.c_str(), pair.second);
        }
    }
    
    mesh->Draw(shader);
}

std::vector<Texture> PrimitiveShape::createColorTextures() {
    std::vector<Texture> textures;
    Texture texture;
    texture.id = createColorTexture(color.r, color.g, color.b);
    texture.type = "texture_diffuse";
    texture.path = "generated_color";
    textures.push_back(texture);
    return textures;
}

void PrimitiveShape::addVertex(std::vector<Vertex>& vertices, const glm::vec3& position, 
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

void PrimitiveShape::addFace(std::vector<unsigned int>& indices, 
                            unsigned int v1, unsigned int v2, unsigned int v3) {
    indices.push_back(v1);
    indices.push_back(v2);
    indices.push_back(v3);
}

float PrimitiveShape::GetMaterialProperty(const std::string& property) const {
    auto it = materialProperties.find(property);
    if (it != materialProperties.end()) {
        return it->second;
    }
    return 0.0f; // Default value if property not found
}

glm::vec3 PrimitiveShape::GetColor() const { 
    return color; 
}

void PrimitiveShape::SetColor(const glm::vec3& newColor) { 
    color = newColor; 
}

std::shared_ptr<Mesh> PrimitiveShape::getMesh() const { 
    return mesh; 
}

void PrimitiveShape::setMesh(std::shared_ptr<Mesh> newMesh) { 
    mesh = newMesh; 
}

void PrimitiveShape::setMaterialProperty(const std::string& name, float value) {
    materialProperties[name] = value;
}

void PrimitiveShape::setCustomShader(Shader* shader) {
    customShader = shader;
}

void PrimitiveShape::setShaderVec3(const std::string& name, const glm::vec3& value) {
    shaderVec3Params[name] = value;
}

void PrimitiveShape::setShaderFloat(const std::string& name, float value) {
    shaderFloatParams[name] = value;
}

Shader* PrimitiveShape::getShader() const {
    return customShader;
}

} // namespace m3D
