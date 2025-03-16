#ifndef PRIMITIVE_SHAPES_H
#define PRIMITIVE_SHAPES_H

#include "../Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>
#include <iostream>

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

// Base class for primitive shapes
class PrimitiveShape : public SceneObject {
protected:
    std::shared_ptr<Mesh> mesh;
    
public:
    PrimitiveShape(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f))
        : SceneObject(name, position, rotation, scale) {
        std::cout << "Creating PrimitiveShape: " << name << std::endl;
    }
    
    virtual void Draw(Shader& shader) override {
        if (!visible || !mesh) {
            std::cout << "Not drawing " << name << ": visible=" << visible << ", mesh=" << (mesh ? "valid" : "null") << std::endl;
            return;
        }
        
        // Set model matrix
        shader.SetMatrix4("model", GetModelMatrix());
        
        // Draw the mesh
        mesh->Draw(shader);
    }
};

// Cube shape
class Cube : public PrimitiveShape {
public:
    Cube(const std::string& name, 
         const glm::vec3& position = glm::vec3(0.0f),
         const glm::vec3& rotation = glm::vec3(0.0f),
         const glm::vec3& scale = glm::vec3(1.0f),
         const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f)) 
        : PrimitiveShape(name, position, rotation, scale) {
        
        std::cout << "Creating Cube: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        // Create cube vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 8 vertices of a cube
        Vertex v1, v2, v3, v4, v5, v6, v7, v8;
        
        // Front face vertices (z = 0.5)
        v1.Position = glm::vec3(-0.5f, -0.5f, 0.5f);
        v2.Position = glm::vec3(0.5f, -0.5f, 0.5f);
        v3.Position = glm::vec3(0.5f, 0.5f, 0.5f);
        v4.Position = glm::vec3(-0.5f, 0.5f, 0.5f);
        
        // Back face vertices (z = -0.5)
        v5.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
        v6.Position = glm::vec3(0.5f, -0.5f, -0.5f);
        v7.Position = glm::vec3(0.5f, 0.5f, -0.5f);
        v8.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
        
        // Set normals, texture coordinates, and other attributes for each vertex
        // Front face (normal: 0, 0, 1)
        v1.Normal = v2.Normal = v3.Normal = v4.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v1.TexCoords = glm::vec2(0.0f, 0.0f);
        v2.TexCoords = glm::vec2(1.0f, 0.0f);
        v3.TexCoords = glm::vec2(1.0f, 1.0f);
        v4.TexCoords = glm::vec2(0.0f, 1.0f);
        
        // Back face (normal: 0, 0, -1)
        v5.Normal = v6.Normal = v7.Normal = v8.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
        v5.TexCoords = glm::vec2(1.0f, 0.0f);
        v6.TexCoords = glm::vec2(0.0f, 0.0f);
        v7.TexCoords = glm::vec2(0.0f, 1.0f);
        v8.TexCoords = glm::vec2(1.0f, 1.0f);
        
        // Set tangent and bitangent for all vertices
        v1.Tangent = v2.Tangent = v3.Tangent = v4.Tangent = 
        v5.Tangent = v6.Tangent = v7.Tangent = v8.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        
        v1.Bitangent = v2.Bitangent = v3.Bitangent = v4.Bitangent = 
        v5.Bitangent = v6.Bitangent = v7.Bitangent = v8.Bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
        
        // Initialize bone weights to 0
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            v1.m_BoneIDs[i] = v2.m_BoneIDs[i] = v3.m_BoneIDs[i] = v4.m_BoneIDs[i] = 
            v5.m_BoneIDs[i] = v6.m_BoneIDs[i] = v7.m_BoneIDs[i] = v8.m_BoneIDs[i] = 0;
            
            v1.m_Weights[i] = v2.m_Weights[i] = v3.m_Weights[i] = v4.m_Weights[i] = 
            v5.m_Weights[i] = v6.m_Weights[i] = v7.m_Weights[i] = v8.m_Weights[i] = 0.0f;
        }
        
        // Add vertices to the vector
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v5);
        vertices.push_back(v6);
        vertices.push_back(v7);
        vertices.push_back(v8);
        
        // Define the 12 triangles (6 faces, 2 triangles each)
        // Front face
        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        indices.push_back(0); indices.push_back(2); indices.push_back(3);
        
        // Back face
        indices.push_back(4); indices.push_back(7); indices.push_back(6);
        indices.push_back(4); indices.push_back(6); indices.push_back(5);
        
        // Left face
        indices.push_back(0); indices.push_back(3); indices.push_back(7);
        indices.push_back(0); indices.push_back(7); indices.push_back(4);
        
        // Right face
        indices.push_back(1); indices.push_back(5); indices.push_back(6);
        indices.push_back(1); indices.push_back(6); indices.push_back(2);
        
        // Top face
        indices.push_back(3); indices.push_back(2); indices.push_back(6);
        indices.push_back(3); indices.push_back(6); indices.push_back(7);
        
        // Bottom face
        indices.push_back(0); indices.push_back(4); indices.push_back(5);
        indices.push_back(0); indices.push_back(5); indices.push_back(1);
        
        // Create a texture with the specified color
        std::vector<Texture> textures;
        Texture texture;
        texture.id = createColorTexture(color.r, color.g, color.b);
        texture.type = "texture_diffuse";
        texture.path = "generated_color";
        textures.push_back(texture);
        
        std::cout << "Creating cube mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Cube mesh created successfully" << std::endl;
    }
};

// Sphere shape
class Sphere : public PrimitiveShape {
public:
    Sphere(const std::string& name, 
           const glm::vec3& position = glm::vec3(0.0f),
           const glm::vec3& rotation = glm::vec3(0.0f),
           const glm::vec3& scale = glm::vec3(1.0f),
           const glm::vec3& color = glm::vec3(0.2f, 0.6f, 1.0f),
           unsigned int segments = 32,
           unsigned int rings = 32) 
        : PrimitiveShape(name, position, rotation, scale) {
        
        std::cout << "Creating Sphere: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Generate sphere vertices
        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)rings;
                
                // Calculate the spherical coordinates
                float theta = xSegment * 2.0f * glm::pi<float>();  // Azimuthal angle (around y-axis)
                float phi = ySegment * glm::pi<float>();           // Polar angle (from top to bottom)
                
                // Convert spherical coordinates to Cartesian coordinates
                float xPos = std::sin(phi) * std::cos(theta);
                float yPos = std::cos(phi);
                float zPos = std::sin(phi) * std::sin(theta);
                
                Vertex vertex;
                vertex.Position = glm::vec3(xPos * 0.5f, yPos * 0.5f, zPos * 0.5f);
                
                // For a sphere, the normal is simply the normalized position vector
                vertex.Normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
                
                // Texture coordinates
                vertex.TexCoords = glm::vec2(xSegment, ySegment);
                
                // Calculate tangent and bitangent
                // Tangent is perpendicular to normal and points along increasing theta
                glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));
                
                // If we're at the poles, we need a different tangent
                if (std::abs(yPos) > 0.999f) {
                    tangent = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                
                vertex.Tangent = tangent;
                
                // Bitangent is perpendicular to both normal and tangent
                vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, tangent));
                
                // Initialize bone weights to 0
                for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                    vertex.m_BoneIDs[i] = 0;
                    vertex.m_Weights[i] = 0.0f;
                }
                
                vertices.push_back(vertex);
            }
        }
        
        // Generate sphere indices
        for (unsigned int y = 0; y < rings; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
                // Get the indices of the quad's vertices
                unsigned int topLeft = y * (segments + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * (segments + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;
                
                // First triangle (top-right, bottom-right, bottom-left)
                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
                
                // Second triangle (top-right, bottom-left, top-left)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(topLeft);
            }
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures;
        Texture texture;
        texture.id = createColorTexture(color.r, color.g, color.b);
        texture.type = "texture_diffuse";
        texture.path = "generated_color";
        textures.push_back(texture);
        
        std::cout << "Creating sphere mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Sphere mesh created successfully" << std::endl;
    }
};

// Prism shape (triangular prism)
class Prism : public PrimitiveShape {
public:
    Prism(const std::string& name, 
          const glm::vec3& position = glm::vec3(0.0f),
          const glm::vec3& rotation = glm::vec3(0.0f),
          const glm::vec3& scale = glm::vec3(1.0f),
          const glm::vec3& color = glm::vec3(0.8f, 0.2f, 0.8f)) 
        : PrimitiveShape(name, position, rotation, scale) {
        
        std::cout << "Creating Prism: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 6 vertices of a triangular prism
        Vertex v1, v2, v3, v4, v5, v6;
        
        // Base triangle (bottom, y = -0.5)
        v1.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
        v2.Position = glm::vec3(0.5f, -0.5f, -0.5f);
        v3.Position = glm::vec3(0.0f, -0.5f, 0.5f);
        
        // Top triangle (y = 0.5)
        v4.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
        v5.Position = glm::vec3(0.5f, 0.5f, -0.5f);
        v6.Position = glm::vec3(0.0f, 0.5f, 0.5f);
        
        // Set normals for each face
        // Bottom face (normal: 0, -1, 0)
        v1.Normal = v2.Normal = v3.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        
        // Top face (normal: 0, 1, 0)
        v4.Normal = v5.Normal = v6.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        
        // Set texture coordinates
        v1.TexCoords = glm::vec2(0.0f, 0.0f);
        v2.TexCoords = glm::vec2(1.0f, 0.0f);
        v3.TexCoords = glm::vec2(0.5f, 1.0f);
        v4.TexCoords = glm::vec2(0.0f, 0.0f);
        v5.TexCoords = glm::vec2(1.0f, 0.0f);
        v6.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Set tangent and bitangent for all vertices
        v1.Tangent = v2.Tangent = v3.Tangent = v4.Tangent = v5.Tangent = v6.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        v1.Bitangent = v2.Bitangent = v3.Bitangent = v4.Bitangent = v5.Bitangent = v6.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        
        // Initialize bone weights to 0
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            v1.m_BoneIDs[i] = v2.m_BoneIDs[i] = v3.m_BoneIDs[i] = v4.m_BoneIDs[i] = v5.m_BoneIDs[i] = v6.m_BoneIDs[i] = 0;
            v1.m_Weights[i] = v2.m_Weights[i] = v3.m_Weights[i] = v4.m_Weights[i] = v5.m_Weights[i] = v6.m_Weights[i] = 0.0f;
        }
        
        // Add vertices to the vector
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v5);
        vertices.push_back(v6);
        
        // Define the triangles for each face
        // Bottom face
        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        
        // Top face
        indices.push_back(3); indices.push_back(5); indices.push_back(4);
        
        // Side faces
        // Front side
        indices.push_back(2); indices.push_back(5); indices.push_back(3);
        indices.push_back(2); indices.push_back(3); indices.push_back(0);
        
        // Right side
        indices.push_back(1); indices.push_back(4); indices.push_back(5);
        indices.push_back(1); indices.push_back(5); indices.push_back(2);
        
        // Back side
        indices.push_back(0); indices.push_back(3); indices.push_back(4);
        indices.push_back(0); indices.push_back(4); indices.push_back(1);
        
        // Create a texture with the specified color
        std::vector<Texture> textures;
        Texture texture;
        texture.id = createColorTexture(color.r, color.g, color.b);
        texture.type = "texture_diffuse";
        texture.path = "generated_color";
        textures.push_back(texture);
        
        std::cout << "Creating prism mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Prism mesh created successfully" << std::endl;
    }
};

// High-quality sphere shape with smooth shading
class HighQualitySphere : public PrimitiveShape {
public:
    HighQualitySphere(const std::string& name, 
                     const glm::vec3& position = glm::vec3(0.0f),
                     const glm::vec3& rotation = glm::vec3(0.0f),
                     const glm::vec3& scale = glm::vec3(1.0f),
                     const glm::vec3& color = glm::vec3(0.2f, 0.6f, 1.0f),
                     unsigned int segments = 64,
                     unsigned int rings = 64) 
        : PrimitiveShape(name, position, rotation, scale) {
        
        std::cout << "Creating High-Quality Sphere: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Generate sphere vertices with higher precision
        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)rings;
                
                // Calculate the spherical coordinates
                float theta = xSegment * 2.0f * glm::pi<float>();
                float phi = ySegment * glm::pi<float>();
                
                // Convert spherical coordinates to Cartesian coordinates
                float xPos = std::sin(phi) * std::cos(theta);
                float yPos = std::cos(phi);
                float zPos = std::sin(phi) * std::sin(theta);
                
                Vertex vertex;
                vertex.Position = glm::vec3(xPos * 0.5f, yPos * 0.5f, zPos * 0.5f);
                vertex.Normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
                
                // Improved texture mapping for spheres
                // This mapping reduces distortion at the poles
                vertex.TexCoords = glm::vec2(xSegment, ySegment);
                
                // Calculate tangent and bitangent for proper normal mapping
                glm::vec3 tangent;
                if (std::abs(yPos) < 0.999f) {
                    // For most of the sphere, tangent is perpendicular to normal in the theta direction
                    tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));
                } else {
                    // At the poles, we need a special case
                    tangent = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                
                vertex.Tangent = tangent;
                vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, tangent));
                
                // Initialize bone weights to 0
                for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                    vertex.m_BoneIDs[i] = 0;
                    vertex.m_Weights[i] = 0.0f;
                }
                
                vertices.push_back(vertex);
            }
        }
        
        // Generate sphere indices with optimal triangle orientation
        for (unsigned int y = 0; y < rings; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
                // Get the indices of the quad's vertices
                unsigned int topLeft = y * (segments + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * (segments + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;
                
                // First triangle (top-right, bottom-right, bottom-left)
                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
                
                // Second triangle (top-right, bottom-left, top-left)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(topLeft);
            }
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures;
        Texture texture;
        texture.id = createColorTexture(color.r, color.g, color.b);
        texture.type = "texture_diffuse";
        texture.path = "generated_color";
        textures.push_back(texture);
        
        std::cout << "Creating high-quality sphere mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "High-quality sphere mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // PRIMITIVE_SHAPES_H 