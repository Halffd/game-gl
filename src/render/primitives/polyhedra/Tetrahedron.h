#ifndef TETRAHEDRON_SHAPE_H
#define TETRAHEDRON_SHAPE_H

#include "../PrimitiveShape.h"
#include <algorithm> // Added for std::max
#include <iostream>

namespace m3D {

// Tetrahedron shape
class Tetrahedron : public PrimitiveShape {
public:
    Tetrahedron(const std::string& name, 
               const glm::vec3& position = glm::vec3(0.0f),
               const glm::vec3& rotation = glm::vec3(0.0f),
               const glm::vec3& scale = glm::vec3(1.0f),
               const glm::vec3& color = glm::vec3(0.7f, 0.3f, 0.7f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Tetrahedron: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 4 vertices of a tetrahedron
        const float a = 1.0f / 3.0f;
        const float b = std::sqrt(8.0f / 9.0f);
        const float c = std::sqrt(2.0f / 9.0f);
        const float d = std::sqrt(2.0f / 3.0f);
        
        // Scale to fit within a 0.5 radius sphere
        const float scale_factor = 0.5f / std::max({a, b, c, d});
        
        // Vertex positions
        glm::vec3 v0 = glm::vec3(0, 0, scale_factor * b);
        glm::vec3 v1 = glm::vec3(-scale_factor * d, 0, -scale_factor * c);
        glm::vec3 v2 = glm::vec3(scale_factor * d / 2.0f, scale_factor * d * std::sqrt(3.0f) / 2.0f, -scale_factor * c);
        glm::vec3 v3 = glm::vec3(scale_factor * d / 2.0f, -scale_factor * d * std::sqrt(3.0f) / 2.0f, -scale_factor * c);
        
        // Calculate face normals
        glm::vec3 n0 = glm::normalize(glm::cross(v2 - v0, v1 - v0));
        glm::vec3 n1 = glm::normalize(glm::cross(v3 - v0, v2 - v0));
        glm::vec3 n2 = glm::normalize(glm::cross(v1 - v0, v3 - v0));
        glm::vec3 n3 = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        
        // Create vertices with proper normals for each face
        // Face 0 (v0, v1, v2)
        Vertex vf0_0, vf0_1, vf0_2;
        vf0_0.Position = v0;
        vf0_1.Position = v1;
        vf0_2.Position = v2;
        vf0_0.Normal = vf0_1.Normal = vf0_2.Normal = n0;
        vf0_0.TexCoords = glm::vec2(0.0f, 0.0f);
        vf0_1.TexCoords = glm::vec2(1.0f, 0.0f);
        vf0_2.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Face 1 (v0, v2, v3)
        Vertex vf1_0, vf1_1, vf1_2;
        vf1_0.Position = v0;
        vf1_1.Position = v2;
        vf1_2.Position = v3;
        vf1_0.Normal = vf1_1.Normal = vf1_2.Normal = n1;
        vf1_0.TexCoords = glm::vec2(0.0f, 0.0f);
        vf1_1.TexCoords = glm::vec2(1.0f, 0.0f);
        vf1_2.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Face 2 (v0, v3, v1)
        Vertex vf2_0, vf2_1, vf2_2;
        vf2_0.Position = v0;
        vf2_1.Position = v3;
        vf2_2.Position = v1;
        vf2_0.Normal = vf2_1.Normal = vf2_2.Normal = n2;
        vf2_0.TexCoords = glm::vec2(0.0f, 0.0f);
        vf2_1.TexCoords = glm::vec2(1.0f, 0.0f);
        vf2_2.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Face 3 (v1, v3, v2)
        Vertex vf3_0, vf3_1, vf3_2;
        vf3_0.Position = v1;
        vf3_1.Position = v3;
        vf3_2.Position = v2;
        vf3_0.Normal = vf3_1.Normal = vf3_2.Normal = n3;
        vf3_0.TexCoords = glm::vec2(0.0f, 0.0f);
        vf3_1.TexCoords = glm::vec2(1.0f, 0.0f);
        vf3_2.TexCoords = glm::vec2(0.5f, 1.0f);
        
        // Calculate tangent and bitangent for each vertex
        std::vector<Vertex*> allVertices = {
            &vf0_0, &vf0_1, &vf0_2,
            &vf1_0, &vf1_1, &vf1_2,
            &vf2_0, &vf2_1, &vf2_2,
            &vf3_0, &vf3_1, &vf3_2
        };
        
        for (Vertex* v : allVertices) {
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
        vertices.push_back(vf0_0);
        vertices.push_back(vf0_1);
        vertices.push_back(vf0_2);
        
        vertices.push_back(vf1_0);
        vertices.push_back(vf1_1);
        vertices.push_back(vf1_2);
        
        vertices.push_back(vf2_0);
        vertices.push_back(vf2_1);
        vertices.push_back(vf2_2);
        
        vertices.push_back(vf3_0);
        vertices.push_back(vf3_1);
        vertices.push_back(vf3_2);
        
        // Add indices for each face
        // Face 0
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        
        // Face 1
        indices.push_back(3);
        indices.push_back(4);
        indices.push_back(5);
        
        // Face 2
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(8);
        
        // Face 3
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(11);
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating tetrahedron mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Tetrahedron mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // TETRAHEDRON_SHAPE_H