#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "../PrimitiveShape.h"

namespace m3D {

// Rectangular prism (cuboid) shape
class RectangularPrism : public PrimitiveShape {
public:
    RectangularPrism(const std::string& name, 
                    const glm::vec3& position = glm::vec3(0.0f),
                    const glm::vec3& rotation = glm::vec3(0.0f),
                    const glm::vec3& scale = glm::vec3(1.0f),
                    const glm::vec3& color = glm::vec3(0.6f, 0.8f, 0.3f),
                    const glm::vec3& dimensions = glm::vec3(1.0f, 1.0f, 1.0f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Rectangular Prism: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        // Create prism vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 8 vertices of a rectangular prism
        Vertex v1, v2, v3, v4, v5, v6, v7, v8;
        
        // Calculate half dimensions
        float halfWidth = dimensions.x * 0.5f;
        float halfHeight = dimensions.y * 0.5f;
        float halfDepth = dimensions.z * 0.5f;
        
        // Front face vertices (z = halfDepth)
        v1.Position = glm::vec3(-halfWidth, -halfHeight, halfDepth);
        v1.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v1.TexCoords = glm::vec2(0.0f, 0.0f);
        
        v2.Position = glm::vec3(halfWidth, -halfHeight, halfDepth);
        v2.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v2.TexCoords = glm::vec2(1.0f, 0.0f);
        
        v3.Position = glm::vec3(halfWidth, halfHeight, halfDepth);
        v3.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v3.TexCoords = glm::vec2(1.0f, 1.0f);
        
        v4.Position = glm::vec3(-halfWidth, halfHeight, halfDepth);
        v4.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v4.TexCoords = glm::vec2(0.0f, 1.0f);
        
        // Back face vertices (z = -halfDepth)
        v5.Position = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
        v5.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
        v5.TexCoords = glm::vec2(1.0f, 0.0f);
        
        v6.Position = glm::vec3(halfWidth, -halfHeight, -halfDepth);
        v6.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
        v6.TexCoords = glm::vec2(0.0f, 0.0f);
        
        v7.Position = glm::vec3(halfWidth, halfHeight, -halfDepth);
        v7.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
        v7.TexCoords = glm::vec2(0.0f, 1.0f);
        
        v8.Position = glm::vec3(-halfWidth, halfHeight, -halfDepth);
        v8.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
        v8.TexCoords = glm::vec2(1.0f, 1.0f);
        
        // Calculate tangent and bitangent for each vertex
        for (Vertex* v : {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}) {
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
        vertices.push_back(v7);
        vertices.push_back(v8);
        
        // Add indices for each face (2 triangles per face, 6 faces)
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
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating rectangular prism mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Rectangular prism mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // RECTANGULAR_PRISM_H 