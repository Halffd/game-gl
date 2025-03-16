#ifndef OCTAHEDRON_SHAPE_H
#define OCTAHEDRON_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Octahedron shape (8 faces, 6 vertices, 12 edges)
class Octahedron : public PrimitiveShape {
public:
    Octahedron(const std::string& name, 
              const glm::vec3& position = glm::vec3(0.0f),
              const glm::vec3& rotation = glm::vec3(0.0f),
              const glm::vec3& scale = glm::vec3(1.0f),
              const glm::vec3& color = glm::vec3(0.5f, 0.8f, 0.5f)) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Octahedron: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 6 vertices of an octahedron
        const float radius = 0.5f; // Radius of the circumscribed sphere
        
        // Vertices along the coordinate axes
        glm::vec3 v0(0.0f, radius, 0.0f);  // Top vertex
        glm::vec3 v1(radius, 0.0f, 0.0f);  // Right vertex
        glm::vec3 v2(0.0f, 0.0f, radius);  // Front vertex
        glm::vec3 v3(-radius, 0.0f, 0.0f); // Left vertex
        glm::vec3 v4(0.0f, 0.0f, -radius); // Back vertex
        glm::vec3 v5(0.0f, -radius, 0.0f); // Bottom vertex
        
        // Calculate face normals for each triangular face
        // Top faces
        glm::vec3 n0 = glm::normalize(glm::cross(v2 - v0, v1 - v0)); // Top-Right-Front
        glm::vec3 n1 = glm::normalize(glm::cross(v4 - v0, v1 - v0)); // Top-Right-Back
        glm::vec3 n2 = glm::normalize(glm::cross(v3 - v0, v4 - v0)); // Top-Left-Back
        glm::vec3 n3 = glm::normalize(glm::cross(v2 - v0, v3 - v0)); // Top-Left-Front
        
        // Bottom faces
        glm::vec3 n4 = glm::normalize(glm::cross(v1 - v5, v2 - v5)); // Bottom-Right-Front
        glm::vec3 n5 = glm::normalize(glm::cross(v4 - v5, v1 - v5)); // Bottom-Right-Back
        glm::vec3 n6 = glm::normalize(glm::cross(v4 - v5, v3 - v5)); // Bottom-Left-Back
        glm::vec3 n7 = glm::normalize(glm::cross(v3 - v5, v2 - v5)); // Bottom-Left-Front
        
        // Add vertices for each face with proper normals
        // Top-Right-Front face
        addVertex(vertices, v0, n0, glm::vec2(0.5f, 1.0f));
        addVertex(vertices, v1, n0, glm::vec2(1.0f, 0.5f));
        addVertex(vertices, v2, n0, glm::vec2(0.5f, 0.5f));
        
        // Top-Right-Back face
        addVertex(vertices, v0, n1, glm::vec2(0.5f, 1.0f));
        addVertex(vertices, v1, n1, glm::vec2(1.0f, 0.5f));
        addVertex(vertices, v4, n1, glm::vec2(0.5f, 0.5f));
        
        // Top-Left-Back face
        addVertex(vertices, v0, n2, glm::vec2(0.5f, 1.0f));
        addVertex(vertices, v3, n2, glm::vec2(0.0f, 0.5f));
        addVertex(vertices, v4, n2, glm::vec2(0.5f, 0.5f));
        
        // Top-Left-Front face
        addVertex(vertices, v0, n3, glm::vec2(0.5f, 1.0f));
        addVertex(vertices, v2, n3, glm::vec2(0.5f, 0.5f));
        addVertex(vertices, v3, n3, glm::vec2(0.0f, 0.5f));
        
        // Bottom-Right-Front face
        addVertex(vertices, v5, n4, glm::vec2(0.5f, 0.0f));
        addVertex(vertices, v2, n4, glm::vec2(0.5f, 0.5f));
        addVertex(vertices, v1, n4, glm::vec2(1.0f, 0.5f));
        
        // Bottom-Right-Back face
        addVertex(vertices, v5, n5, glm::vec2(0.5f, 0.0f));
        addVertex(vertices, v1, n5, glm::vec2(1.0f, 0.5f));
        addVertex(vertices, v4, n5, glm::vec2(0.5f, 0.5f));
        
        // Bottom-Left-Back face
        addVertex(vertices, v5, n6, glm::vec2(0.5f, 0.0f));
        addVertex(vertices, v4, n6, glm::vec2(0.5f, 0.5f));
        addVertex(vertices, v3, n6, glm::vec2(0.0f, 0.5f));
        
        // Bottom-Left-Front face
        addVertex(vertices, v5, n7, glm::vec2(0.5f, 0.0f));
        addVertex(vertices, v3, n7, glm::vec2(0.0f, 0.5f));
        addVertex(vertices, v2, n7, glm::vec2(0.5f, 0.5f));
        
        // Add indices for each face (one triangle per face)
        for (unsigned int i = 0; i < 8; ++i) {
            indices.push_back(i * 3);
            indices.push_back(i * 3 + 1);
            indices.push_back(i * 3 + 2);
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating octahedron mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Octahedron mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // OCTAHEDRON_SHAPE_H 