#ifndef CYLINDER_SHAPE_H
#define CYLINDER_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Cylinder shape
class Cylinder : public PrimitiveShape {
public:
    Cylinder(const std::string& name, 
            const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f),
            const glm::vec3& color = glm::vec3(0.3f, 0.8f, 0.6f),
            unsigned int segments = 32) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Cylinder: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        const float radius = 0.5f;
        const float height = 1.0f;
        const float halfHeight = height * 0.5f;
        
        // Create top and bottom center vertices
        Vertex topCenter, bottomCenter;
        
        topCenter.Position = glm::vec3(0.0f, halfHeight, 0.0f);
        topCenter.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        topCenter.TexCoords = glm::vec2(0.5f, 0.5f);
        
        bottomCenter.Position = glm::vec3(0.0f, -halfHeight, 0.0f);
        bottomCenter.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomCenter.TexCoords = glm::vec2(0.5f, 0.5f);
        
        // Calculate tangent and bitangent for center vertices
        for (Vertex* v : {&topCenter, &bottomCenter}) {
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
        vertices.push_back(topCenter);     // Index 1
        
        // Create vertices for the sides of the cylinder
        for (unsigned int i = 0; i <= segments; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / segments;
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            
            // Bottom rim vertex
            Vertex bottomRim;
            bottomRim.Position = glm::vec3(x, -halfHeight, z);
            bottomRim.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
            bottomRim.TexCoords = glm::vec2((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
            
            // Top rim vertex
            Vertex topRim;
            topRim.Position = glm::vec3(x, halfHeight, z);
            topRim.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            topRim.TexCoords = glm::vec2((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
            
            // Side vertex (same position as top rim)
            Vertex side;
            side.Position = glm::vec3(x, halfHeight, z);
            side.Normal = glm::normalize(glm::vec3(x, 0.0f, z));
            side.TexCoords = glm::vec2(static_cast<float>(i) / segments, 1.0f);
            
            // Side vertex (same position as bottom rim)
            Vertex side2;
            side2.Position = glm::vec3(x, -halfHeight, z);
            side2.Normal = glm::normalize(glm::vec3(x, 0.0f, z));
            side2.TexCoords = glm::vec2(static_cast<float>(i) / segments, 0.0f);
            
            // Calculate tangent and bitangent for each vertex
            for (Vertex* v : {&bottomRim, &topRim, &side, &side2}) {
                glm::vec3 tangent;
                if (std::abs(v->Normal.y) < 0.999f) {
                    // For side vertices, tangent is along the circle
                    tangent = glm::normalize(glm::vec3(-std::sin(angle), 0.0f, std::cos(angle)));
                } else {
                    // For top/bottom vertices, tangent is along X axis
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
            
            vertices.push_back(bottomRim);  // Index 2 + 4*i
            vertices.push_back(topRim);     // Index 3 + 4*i
            vertices.push_back(side);       // Index 4 + 4*i
            vertices.push_back(side2);      // Index 5 + 4*i
        }
        
        // Add indices for bottom face
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(0);  // Bottom center
            indices.push_back(2 + 4*i);  // Current bottom rim
            indices.push_back(2 + 4*((i+1) % segments));  // Next bottom rim
        }
        
        // Add indices for top face
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(1);  // Top center
            indices.push_back(3 + 4*((i+1) % segments));  // Next top rim
            indices.push_back(3 + 4*i);  // Current top rim
        }
        
        // Add indices for side faces
        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int current = 4 + 4*i;  // Current side top
            unsigned int next = 4 + 4*((i+1) % segments);  // Next side top
            unsigned int currentBottom = 5 + 4*i;  // Current side bottom
            unsigned int nextBottom = 5 + 4*((i+1) % segments);  // Next side bottom
            
            // First triangle
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(nextBottom);
            
            // Second triangle
            indices.push_back(current);
            indices.push_back(nextBottom);
            indices.push_back(currentBottom);
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating cylinder mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Cylinder mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // CYLINDER_SHAPE_H 