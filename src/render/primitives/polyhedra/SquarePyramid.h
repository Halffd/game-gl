#ifndef SQUARE_PYRAMID_SHAPE_H
#define SQUARE_PYRAMID_SHAPE_H

#include "../PrimitiveShape.h"

namespace m3D {

// Square Pyramid shape
class SquarePyramid : public PrimitiveShape {
public:
    SquarePyramid(const std::string& name, 
                 const glm::vec3& position = glm::vec3(0.0f),
                 const glm::vec3& rotation = glm::vec3(0.0f),
                 const glm::vec3& scale = glm::vec3(1.0f),
                 const glm::vec3& color = glm::vec3(0.8f, 0.6f, 0.2f),
                 float height = 1.0f) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Square Pyramid: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Define the 5 vertices of a square pyramid
        // Base vertices (square)
        const float baseHalfWidth = 0.5f;
        const float pyramidHeight = height * 0.5f; // Half height to keep consistent with other shapes
        
        // Base vertices (z = -pyramidHeight)
        glm::vec3 v0(-baseHalfWidth, -pyramidHeight, -baseHalfWidth); // Bottom left
        glm::vec3 v1(baseHalfWidth, -pyramidHeight, -baseHalfWidth);  // Bottom right
        glm::vec3 v2(baseHalfWidth, -pyramidHeight, baseHalfWidth);   // Top right
        glm::vec3 v3(-baseHalfWidth, -pyramidHeight, baseHalfWidth);  // Top left
        
        // Apex vertex (top of pyramid)
        glm::vec3 apex(0.0f, pyramidHeight, 0.0f);
        
        // Calculate face normals
        // Base face normal (pointing down)
        glm::vec3 baseNormal = glm::vec3(0.0f, -1.0f, 0.0f);
        
        // Side face normals
        glm::vec3 frontNormal = glm::normalize(glm::cross(v3 - apex, v2 - apex));
        glm::vec3 rightNormal = glm::normalize(glm::cross(v2 - apex, v1 - apex));
        glm::vec3 backNormal = glm::normalize(glm::cross(v1 - apex, v0 - apex));
        glm::vec3 leftNormal = glm::normalize(glm::cross(v0 - apex, v3 - apex));
        
        // Add base vertices with base normal
        addVertex(vertices, v0, baseNormal, glm::vec2(0.0f, 0.0f));
        addVertex(vertices, v1, baseNormal, glm::vec2(1.0f, 0.0f));
        addVertex(vertices, v2, baseNormal, glm::vec2(1.0f, 1.0f));
        addVertex(vertices, v3, baseNormal, glm::vec2(0.0f, 1.0f));
        
        // Add apex vertices with different normals for each face
        addVertex(vertices, apex, frontNormal, glm::vec2(0.5f, 0.5f)); // Front face apex
        addVertex(vertices, apex, rightNormal, glm::vec2(0.5f, 0.5f)); // Right face apex
        addVertex(vertices, apex, backNormal, glm::vec2(0.5f, 0.5f));  // Back face apex
        addVertex(vertices, apex, leftNormal, glm::vec2(0.5f, 0.5f));  // Left face apex
        
        // Add base vertices with side normals for the side faces
        // Front face vertices (with front normal)
        addVertex(vertices, v3, frontNormal, glm::vec2(0.0f, 0.0f));
        addVertex(vertices, v2, frontNormal, glm::vec2(1.0f, 0.0f));
        
        // Right face vertices (with right normal)
        addVertex(vertices, v2, rightNormal, glm::vec2(0.0f, 0.0f));
        addVertex(vertices, v1, rightNormal, glm::vec2(1.0f, 0.0f));
        
        // Back face vertices (with back normal)
        addVertex(vertices, v1, backNormal, glm::vec2(0.0f, 0.0f));
        addVertex(vertices, v0, backNormal, glm::vec2(1.0f, 0.0f));
        
        // Left face vertices (with left normal)
        addVertex(vertices, v0, leftNormal, glm::vec2(0.0f, 0.0f));
        addVertex(vertices, v3, leftNormal, glm::vec2(1.0f, 0.0f));
        
        // Add indices for the base (as a quad - two triangles)
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);
        
        // Add indices for the side faces
        // Front face
        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(4);
        
        // Right face
        indices.push_back(10);
        indices.push_back(11);
        indices.push_back(5);
        
        // Back face
        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(6);
        
        // Left face
        indices.push_back(14);
        indices.push_back(15);
        indices.push_back(7);
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating square pyramid mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Square pyramid mesh created successfully" << std::endl;
    }
};

} // namespace m3D

#endif // SQUARE_PYRAMID_SHAPE_H 