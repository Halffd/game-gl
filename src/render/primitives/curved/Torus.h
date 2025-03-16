#ifndef TORUS_SHAPE_H
#define TORUS_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Torus shape
class Torus : public PrimitiveShape {
public:
    Torus(const std::string& name, 
         const glm::vec3& position = glm::vec3(0.0f),
         const glm::vec3& rotation = glm::vec3(0.0f),
         const glm::vec3& scale = glm::vec3(1.0f),
         const glm::vec3& color = glm::vec3(0.4f, 0.7f, 0.9f),
         float majorRadius = 0.35f,
         float minorRadius = 0.15f,
         unsigned int majorSegments = 32,
         unsigned int minorSegments = 24) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Torus: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Generate torus vertices
        for (unsigned int i = 0; i <= majorSegments; ++i) {
            float majorAngle = 2.0f * glm::pi<float>() * i / majorSegments;
            float majorX = std::cos(majorAngle);
            float majorZ = std::sin(majorAngle);
            
            // Center of the current minor circle
            glm::vec3 center(majorX * majorRadius, 0.0f, majorZ * majorRadius);
            
            // Direction from center of torus to center of minor circle
            glm::vec3 toCenter = glm::normalize(center);
            
            for (unsigned int j = 0; j <= minorSegments; ++j) {
                float minorAngle = 2.0f * glm::pi<float>() * j / minorSegments;
                
                // Calculate the position on the minor circle
                // We need to create a local coordinate system for the minor circle
                glm::vec3 normal = toCenter; // Normal points from center of torus to center of minor circle
                
                // Create a tangent vector (perpendicular to normal and up)
                glm::vec3 tangent;
                if (std::abs(normal.y) < 0.999f) {
                    tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
                } else {
                    tangent = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), normal));
                }
                
                // Create a bitangent to complete the coordinate system
                glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));
                
                // Calculate position on the minor circle
                float minorX = std::cos(minorAngle);
                float minorY = std::sin(minorAngle);
                
                // Position is center of minor circle plus offset in the local coordinate system
                glm::vec3 position = center + (normal * minorX + bitangent * minorY) * minorRadius;
                
                // Normal is the direction from the center of the minor circle to the point
                glm::vec3 vertexNormal = glm::normalize(position - center);
                
                // Texture coordinates
                float u = static_cast<float>(i) / majorSegments;
                float v = static_cast<float>(j) / minorSegments;
                
                // Add the vertex
                Vertex vertex;
                vertex.Position = position;
                vertex.Normal = vertexNormal;
                vertex.TexCoords = glm::vec2(u, v);
                
                // Calculate tangent and bitangent for the vertex
                // Tangent is along the major circle
                glm::vec3 vertexTangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), vertexNormal));
                if (glm::length(vertexTangent) < 0.001f) {
                    vertexTangent = tangent; // Fallback if cross product is too small
                }
                
                vertex.Tangent = vertexTangent;
                vertex.Bitangent = glm::normalize(glm::cross(vertexNormal, vertexTangent));
                
                // Initialize bone weights to 0
                for (int k = 0; k < MAX_BONE_INFLUENCE; k++) {
                    vertex.m_BoneIDs[k] = 0;
                    vertex.m_Weights[k] = 0.0f;
                }
                
                vertices.push_back(vertex);
            }
        }
        
        // Generate indices
        for (unsigned int i = 0; i < majorSegments; ++i) {
            for (unsigned int j = 0; j < minorSegments; ++j) {
                unsigned int current = i * (minorSegments + 1) + j;
                unsigned int next = current + (minorSegments + 1);
                
                // First triangle
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);
                
                // Second triangle
                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating torus mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Torus mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // TORUS_SHAPE_H 