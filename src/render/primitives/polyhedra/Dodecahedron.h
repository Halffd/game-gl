#ifndef DODECAHEDRON_SHAPE_H
#define DODECAHEDRON_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Dodecahedron shape (12 pentagonal faces, 20 vertices, 30 edges)
class Dodecahedron : public PrimitiveShape {
public:
    Dodecahedron(const std::string& name, 
                const glm::vec3& position = glm::vec3(0.0f),
                const glm::vec3& rotation = glm::vec3(0.0f),
                const glm::vec3& scale = glm::vec3(1.0f),
                const glm::vec3& color = glm::vec3(0.9f, 0.6f, 0.3f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Dodecahedron: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the vertices of a dodecahedron
        const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f; // Golden ratio
        const float invPhi = 1.0f / phi;
        const float radius = 0.5f; // Radius of the circumscribed sphere
        
        std::vector<glm::vec3> baseVertices = {
            // Vertices based on (±1, ±1, ±1)
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            
            // Vertices based on (0, ±phi, ±invPhi)
            glm::vec3(0.0f, phi, invPhi),
            glm::vec3(0.0f, -phi, invPhi),
            glm::vec3(0.0f, phi, -invPhi),
            glm::vec3(0.0f, -phi, -invPhi),
            
            // Vertices based on (±invPhi, 0, ±phi)
            glm::vec3(invPhi, 0.0f, phi),
            glm::vec3(-invPhi, 0.0f, phi),
            glm::vec3(invPhi, 0.0f, -phi),
            glm::vec3(-invPhi, 0.0f, -phi),
            
            // Vertices based on (±phi, ±invPhi, 0)
            glm::vec3(phi, invPhi, 0.0f),
            glm::vec3(-phi, invPhi, 0.0f),
            glm::vec3(phi, -invPhi, 0.0f),
            glm::vec3(-phi, -invPhi, 0.0f)
        };
        
        // Normalize vertices to fit within the specified radius
        float maxLength = 0.0f;
        for (const auto& v : baseVertices) {
            maxLength = std::max(maxLength, glm::length(v));
        }
        
        std::vector<glm::vec3> normalizedVertices;
        for (const auto& v : baseVertices) {
            normalizedVertices.push_back(v * (radius / maxLength));
        }
        
        // Define the 12 pentagonal faces (indices into normalizedVertices)
        std::vector<std::vector<int>> faces = {
            {0, 8, 1, 13, 12},    // Face 1
            {0, 16, 4, 10, 8},    // Face 2
            {0, 12, 2, 18, 16},   // Face 3
            {1, 8, 10, 5, 17},    // Face 4
            {1, 17, 19, 3, 13},   // Face 5
            {2, 12, 13, 3, 9},    // Face 6
            {2, 9, 11, 6, 18},    // Face 7
            {3, 19, 7, 11, 9},    // Face 8
            {4, 16, 18, 6, 14},   // Face 9
            {4, 14, 15, 5, 10},   // Face 10
            {5, 15, 7, 19, 17},   // Face 11
            {6, 11, 7, 15, 14}    // Face 12
        };
        
        // For each face, calculate its normal and add vertices
        for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
            const auto& face = faces[faceIdx];
            
            // Calculate face center
            glm::vec3 faceCenter(0.0f);
            for (int vertIdx : face) {
                faceCenter += normalizedVertices[vertIdx];
            }
            faceCenter /= static_cast<float>(face.size());
            
            // Calculate face normal (using first three vertices)
            glm::vec3 v1 = normalizedVertices[face[0]];
            glm::vec3 v2 = normalizedVertices[face[1]];
            glm::vec3 v3 = normalizedVertices[face[2]];
            
            glm::vec3 edge1 = v2 - v1;
            glm::vec3 edge2 = v3 - v1;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            // Ensure normal points outward
            if (glm::dot(normal, faceCenter) < 0.0f) {
                normal = -normal;
            }
            
            // Add vertices for this face
            unsigned int baseIndex = vertices.size();
            
            // Add center vertex
            addVertex(vertices, faceCenter, normal, glm::vec2(0.5f, 0.5f));
            
            // Add vertices for the pentagon
            for (size_t i = 0; i < face.size(); ++i) {
                float angle = 2.0f * glm::pi<float>() * i / face.size();
                glm::vec2 texCoord(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle));
                
                addVertex(vertices, normalizedVertices[face[i]], normal, texCoord);
            }
            
            // Add indices for triangulating the pentagon (fan triangulation from center)
            for (size_t i = 0; i < face.size(); ++i) {
                indices.push_back(baseIndex); // Center
                indices.push_back(baseIndex + 1 + i); // Current vertex
                indices.push_back(baseIndex + 1 + ((i + 1) % face.size())); // Next vertex
            }
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating dodecahedron mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Dodecahedron mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // DODECAHEDRON_SHAPE_H 