#ifndef ICOSPHERE_SHAPE_H
#define ICOSPHERE_SHAPE_H

#include "../PrimitiveShape.h"
#include <map>

namespace m3D {

// Icosphere shape - a sphere based on subdivided icosahedron
class IcosphereShape : public PrimitiveShape {
public:
    IcosphereShape(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f),
                  const glm::vec3& color = glm::vec3(0.2f, 0.6f, 1.0f),
                  unsigned int subdivisions = 3) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Icosphere: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::map<std::pair<unsigned int, unsigned int>, unsigned int> middlePointIndexCache;
        
        // Create 12 vertices of an icosahedron
        const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
        const float radius = 0.5f; // Radius of 0.5 to match other shapes
        
        // Add vertices for icosahedron
        addVertex(vertices, glm::normalize(glm::vec3(-1.0f, t, 0.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(1.0f, t, 0.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(-1.0f, -t, 0.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(1.0f, -t, 0.0f)) * radius);
        
        addVertex(vertices, glm::normalize(glm::vec3(0.0f, -1.0f, t)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(0.0f, 1.0f, t)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(0.0f, -1.0f, -t)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(0.0f, 1.0f, -t)) * radius);
        
        addVertex(vertices, glm::normalize(glm::vec3(t, 0.0f, -1.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(t, 0.0f, 1.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(-t, 0.0f, -1.0f)) * radius);
        addVertex(vertices, glm::normalize(glm::vec3(-t, 0.0f, 1.0f)) * radius);
        
        // Add faces (5 faces around each vertex)
        // 5 faces around point 0
        addFace(indices, 0, 11, 5);
        addFace(indices, 0, 5, 1);
        addFace(indices, 0, 1, 7);
        addFace(indices, 0, 7, 10);
        addFace(indices, 0, 10, 11);
        
        // 5 faces around point 1
        addFace(indices, 1, 5, 9);
        addFace(indices, 1, 9, 8);
        addFace(indices, 1, 8, 7);
        
        // 5 faces around point 2
        addFace(indices, 2, 4, 11);
        addFace(indices, 2, 11, 10);
        addFace(indices, 2, 10, 6);
        addFace(indices, 2, 6, 3);
        addFace(indices, 2, 3, 4);
        
        // 5 faces around point 3
        addFace(indices, 3, 6, 8);
        addFace(indices, 3, 8, 9);
        addFace(indices, 3, 9, 4);
        
        // 5 faces around point 4
        addFace(indices, 4, 9, 5);
        addFace(indices, 4, 5, 11);
        
        // 5 faces around point 5
        
        // 5 faces around point 6
        addFace(indices, 6, 10, 7);
        addFace(indices, 6, 7, 8);
        
        // 5 faces around point 7
        
        // 5 faces around point 8
        
        // 5 faces around point 9
        
        // 5 faces around point 10
        
        // 5 faces around point 11
        
        // Subdivide the faces
        for (unsigned int i = 0; i < subdivisions; i++) {
            std::vector<unsigned int> newIndices;
            
            // Iterate over all triangles
            for (size_t j = 0; j < indices.size(); j += 3) {
                unsigned int v1 = indices[j];
                unsigned int v2 = indices[j + 1];
                unsigned int v3 = indices[j + 2];
                
                // Get the midpoints of each edge
                unsigned int a = getMiddlePoint(v1, v2, vertices, middlePointIndexCache, radius);
                unsigned int b = getMiddlePoint(v2, v3, vertices, middlePointIndexCache, radius);
                unsigned int c = getMiddlePoint(v3, v1, vertices, middlePointIndexCache, radius);
                
                // Create 4 new triangles
                addFace(newIndices, v1, a, c);
                addFace(newIndices, v2, b, a);
                addFace(newIndices, v3, c, b);
                addFace(newIndices, a, b, c);
            }
            
            // Replace old indices with new ones
            indices = newIndices;
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating icosphere mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Icosphere mesh created successfully" << std::endl;
    }
    
private:
    // Helper function to add a vertex to the mesh
    void addVertex(std::vector<Vertex>& vertices, const glm::vec3& position) {
        Vertex vertex;
        vertex.Position = position;
        
        // For a sphere, the normal is the normalized position
        vertex.Normal = glm::normalize(position);
        
        // Calculate texture coordinates using spherical mapping
        float u = 0.5f + std::atan2(position.z, position.x) / (2.0f * glm::pi<float>());
        float v = 0.5f - std::asin(position.y) / glm::pi<float>();
        vertex.TexCoords = glm::vec2(u, v);
        
        // Calculate tangent and bitangent
        glm::vec3 tangent;
        if (std::abs(vertex.Normal.y) < 0.999f) {
            tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), vertex.Normal));
        } else {
            tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), vertex.Normal));
        }
        vertex.Tangent = tangent;
        vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, tangent));
        
        // Initialize bone weights to 0
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = 0;
            vertex.m_Weights[i] = 0.0f;
        }
        
        vertices.push_back(vertex);
    }
    
    // Helper function to get or create a vertex at the middle of an edge
    unsigned int getMiddlePoint(unsigned int v1, unsigned int v2, 
                               std::vector<Vertex>& vertices, 
                               std::map<std::pair<unsigned int, unsigned int>, unsigned int>& cache,
                               float radius) {
        // Check if we already have this edge's middle point
        bool firstIsSmaller = v1 < v2;
        std::pair<unsigned int, unsigned int> key = firstIsSmaller ? 
            std::make_pair(v1, v2) : std::make_pair(v2, v1);
            
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
        
        // Not in cache, calculate the middle point
        const glm::vec3& p1 = vertices[v1].Position;
        const glm::vec3& p2 = vertices[v2].Position;
        glm::vec3 middle = (p1 + p2) * 0.5f;
        
        // Normalize to the sphere's radius
        middle = glm::normalize(middle) * radius;
        
        // Add the new vertex
        unsigned int newIndex = vertices.size();
        addVertex(vertices, middle);
        
        // Add to cache
        cache[key] = newIndex;
        
        return newIndex;
    }
};

} // namespace m3D

#endif // ICOSPHERE_SHAPE_H 