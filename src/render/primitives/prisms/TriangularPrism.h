#ifndef TRIANGULAR_PRISM_H
#define TRIANGULAR_PRISM_H

#include "../PrimitiveShape.h"

namespace m3D {

// Triangular prism shape
class TriangularPrism : public PrimitiveShape {
public:
    TriangularPrism(const std::string& name, 
                   const glm::vec3& position = glm::vec3(0.0f),
                   const glm::vec3& rotation = glm::vec3(0.0f),
                   const glm::vec3& scale = glm::vec3(1.0f),
                   const glm::vec3& color = glm::vec3(0.8f, 0.4f, 0.3f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Triangular Prism: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        // Create prism vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 6 vertices of a triangular prism
        Vertex v1, v2, v3, v4, v5, v6;
        
        // Bottom triangle
        v1.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
        v1.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        v1.TexCoords = glm::vec2(0.0f, 0.0f);
        
        v2.Position = glm::vec3(0.5f, -0.5f, -0.5f);
        v2.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        v2.TexCoords = glm::vec2(1.0f, 0.0f);
        
        v3.Position = glm::vec3(0.0f, -0.5f, 0.5f);
        v3.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        v3.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Top triangle
        v4.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
        v4.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v4.TexCoords = glm::vec2(0.0f, 0.0f);
        
        v5.Position = glm::vec3(0.5f, 0.5f, -0.5f);
        v5.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v5.TexCoords = glm::vec2(1.0f, 0.0f);
        
        v6.Position = glm::vec3(0.0f, 0.5f, 0.5f);
        v6.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v6.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Calculate tangent and bitangent for each vertex
        for (Vertex* v : {&v1, &v2, &v3, &v4, &v5, &v6}) {
            glm::vec3 tangent;
            if (std::abs(v->Normal.y) < 0.999f) {
                tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), v->Normal));
            } else {
                tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v->Normal));
            }
            v->Tangent = tangent;
            v->Bitangent = glm::normalize(glm::cross(v->Normal, tangent));
            
            // Initialize bone weights to 0
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                v->m_BoneIDs[i] = 0;
                v->m_Weights[i] = 0.0f;
            }
        }
        
        // Add vertices to the vector
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v5);
        vertices.push_back(v6);
        
        // Add indices for each face
        // Bottom triangle
        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        
        // Top triangle
        indices.push_back(5); indices.push_back(4); indices.push_back(3);
        
        // Side faces (rectangles split into triangles)
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
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating triangular prism mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Triangular prism mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // TRIANGULAR_PRISM_H 