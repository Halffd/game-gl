#ifndef HEXAGONAL_PRISM_H
#define HEXAGONAL_PRISM_H

#include "../PrimitiveShape.h"

namespace m3D {

// Hexagonal prism shape
class HexagonalPrism : public PrimitiveShape {
public:
    HexagonalPrism(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f),
                  const glm::vec3& color = glm::vec3(0.9f, 0.5f, 0.7f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Hexagonal Prism: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        // Create prism vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 12 vertices of a hexagonal prism (6 for bottom face, 6 for top face)
        std::vector<Vertex> hexagonVertices(12);
        
        // Calculate the coordinates of the hexagon vertices
        const float radius = 0.5f;
        const float height = 1.0f;
        const float halfHeight = height * 0.5f;
        const int numSides = 6;
        
        // Generate the hexagon vertices
        for (int i = 0; i < numSides; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / numSides;
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            
            // Bottom face vertex
            hexagonVertices[i].Position = glm::vec3(x, -halfHeight, z);
            hexagonVertices[i].Normal = glm::vec3(0.0f, -1.0f, 0.0f);
            hexagonVertices[i].TexCoords = glm::vec2((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
            
            // Top face vertex
            hexagonVertices[i + numSides].Position = glm::vec3(x, halfHeight, z);
            hexagonVertices[i + numSides].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            hexagonVertices[i + numSides].TexCoords = glm::vec2((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
        }
        
        // Calculate tangent and bitangent for each vertex
        for (Vertex& v : hexagonVertices) {
            glm::vec3 tangent;
            if (std::abs(v.Normal.y) < 0.999f) {
                tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), v.Normal));
            } else {
                tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v.Normal));
            }
            v.Tangent = tangent;
            v.Bitangent = glm::normalize(glm::cross(v.Normal, tangent));
            
            // Initialize bone weights to 0
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                v.m_BoneIDs[i] = 0;
                v.m_Weights[i] = 0.0f;
            }
        }
        
        // Add vertices to the vector
        vertices.insert(vertices.end(), hexagonVertices.begin(), hexagonVertices.end());
        
        // Add indices for the bottom face (as a fan)
        for (int i = 1; i < numSides - 1; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        
        // Add indices for the top face (as a fan, but in reverse order)
        for (int i = 1; i < numSides - 1; ++i) {
            indices.push_back(numSides);
            indices.push_back(numSides + i + 1);
            indices.push_back(numSides + i);
        }
        
        // Add indices for the side faces (rectangles split into triangles)
        for (int i = 0; i < numSides; ++i) {
            int nextI = (i + 1) % numSides;
            
            // First triangle of the side face
            indices.push_back(i);
            indices.push_back(nextI);
            indices.push_back(numSides + nextI);
            
            // Second triangle of the side face
            indices.push_back(i);
            indices.push_back(numSides + nextI);
            indices.push_back(numSides + i);
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating hexagonal prism mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Hexagonal prism mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // HEXAGONAL_PRISM_H 