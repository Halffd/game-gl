#ifndef CONE_SHAPE_H
#define CONE_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Cone shape
class Cone : public PrimitiveShape {
public:
    Cone(const std::string& name, 
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f),
        const glm::vec3& color = glm::vec3(0.9f, 0.3f, 0.3f),
        unsigned int segments = 32) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Cone: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        const float radius = 0.5f;
        const float height = 1.0f;
        const float halfHeight = height * 0.5f;
        
        // Create top vertex (apex of the cone)
        Vertex apex;
        apex.Position = glm::vec3(0.0f, halfHeight, 0.0f);
        apex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        apex.TexCoords = glm::vec2(0.5f, 0.5f);
        
        // Create bottom center vertex
        Vertex bottomCenter;
        bottomCenter.Position = glm::vec3(0.0f, -halfHeight, 0.0f);
        bottomCenter.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomCenter.TexCoords = glm::vec2(0.5f, 0.5f);
        
        // Calculate tangent and bitangent for center vertices
        for (Vertex* v : {&apex, &bottomCenter}) {
            glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v->Normal));
            v->Tangent = tangent;
            v->Bitangent = glm::normalize(glm::cross(v->Normal, tangent));
            
            // Initialize bone weights to 0
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                v->m_BoneIDs[i] = 0;
                v->m_Weights[i] = 0.0f;
            }
        }
        
        // Add center vertices
        vertices.push_back(bottomCenter);  // Index 0
        vertices.push_back(apex);          // Index 1
        
        // Create vertices for the sides of the cone
        for (unsigned int i = 0; i <= segments; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / segments;
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            
            // Bottom rim vertex
            Vertex bottomRim;
            bottomRim.Position = glm::vec3(x, -halfHeight, z);
            bottomRim.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
            bottomRim.TexCoords = glm::vec2((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
            
            // Side vertex (same position as bottom rim but with different normal)
            Vertex side;
            side.Position = glm::vec3(x, -halfHeight, z);
            
            // Calculate normal for the side (pointing outward and upward)
            glm::vec3 toApex = apex.Position - side.Position;
            glm::vec3 tangential = glm::normalize(glm::vec3(-std::sin(angle), 0.0f, std::cos(angle)));
            side.Normal = glm::normalize(glm::cross(glm::cross(toApex, tangential), toApex));
            
            side.TexCoords = glm::vec2(static_cast<float>(i) / segments, 0.0f);
            
            // Calculate tangent and bitangent for each vertex
            for (Vertex* v : {&bottomRim, &side}) {
                glm::vec3 tangent;
                if (std::abs(v->Normal.y) < 0.999f) {
                    // For side vertices, tangent is along the circle
                    tangent = glm::normalize(glm::vec3(-std::sin(angle), 0.0f, std::cos(angle)));
                } else {
                    // For bottom vertex, tangent is along X axis
                    tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v->Normal));
                }
                v->Tangent = tangent;
                v->Bitangent = glm::normalize(glm::cross(v->Normal, tangent));
                
                // Initialize bone weights to 0
                for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
                    v->m_BoneIDs[j] = 0;
                    v->m_Weights[j] = 0.0f;
                }
            }
            
            vertices.push_back(bottomRim);  // Index 2 + 2*i
            vertices.push_back(side);       // Index 3 + 2*i
        }
        
        // Add indices for bottom face
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(0);  // Bottom center
            indices.push_back(2 + 2*i);  // Current bottom rim
            indices.push_back(2 + 2*((i+1) % segments));  // Next bottom rim
        }
        
        // Add indices for side faces
        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int current = 3 + 2*i;  // Current side
            unsigned int next = 3 + 2*((i+1) % segments);  // Next side
            
            // Triangle connecting current side, next side, and apex
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(1);  // Apex
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating cone mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Cone mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // CONE_SHAPE_H 