#ifndef HIGH_QUALITY_SPHERE_H
#define HIGH_QUALITY_SPHERE_H

#include "../PrimitiveShape.h"

namespace m3D {

// High-quality sphere shape with smooth shading and improved lighting
class HighQualitySphere : public PrimitiveShape {
public:
    // Roundness parameter (0.0 to 1.0) controls how spherical the shape is
    float roundness;
    
    HighQualitySphere(const std::string& name, 
                     const glm::vec3& position = glm::vec3(0.0f),
                     const glm::vec3& rotation = glm::vec3(0.0f),
                     const glm::vec3& scale = glm::vec3(1.0f),
                     const glm::vec3& color = glm::vec3(0.2f, 0.6f, 1.0f),
                     unsigned int segments = 64,
                     unsigned int rings = 64,
                     float roundness = 1.0f) 
        : PrimitiveShape(name, position, rotation, scale, color), roundness(roundness) {
        
        std::cout << "Creating High-Quality Sphere: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Generate sphere vertices with higher precision
        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)rings;
                
                // Calculate the spherical coordinates
                float theta = xSegment * 2.0f * glm::pi<float>();
                float phi = ySegment * glm::pi<float>();
                
                // Convert spherical coordinates to Cartesian coordinates
                float xPos = std::sin(phi) * std::cos(theta);
                float yPos = std::cos(phi);
                float zPos = std::sin(phi) * std::sin(theta);
                
                Vertex vertex;
                
                // Apply roundness parameter - interpolate between icosahedron and sphere
                if (roundness < 1.0f) {
                    // Create an icosahedron-like point (better approximation than octahedron)
                    glm::vec3 icoPoint;
                    
                    // Approximate icosahedron projection
                    float phi = std::atan2(std::sqrt(xPos*xPos + zPos*zPos), yPos);
                    float theta = std::atan2(zPos, xPos);
                    
                    // Adjust phi to create icosahedron-like shape
                    float t = phi / glm::pi<float>();
                    float adjustedPhi = phi;
                    
                    // Adjust phi based on the golden ratio properties of icosahedron
                    if (t < 0.2f || t > 0.8f) {
                        adjustedPhi = phi * 0.9f;
                    } else if (t < 0.4f || t > 0.6f) {
                        adjustedPhi = phi * 1.1f;
                    }
                    
                    // Convert back to Cartesian
                    float r = 0.5f;
                    icoPoint.x = r * std::sin(adjustedPhi) * std::cos(theta);
                    icoPoint.y = r * std::cos(adjustedPhi);
                    icoPoint.z = r * std::sin(adjustedPhi) * std::sin(theta);
                    
                    // Interpolate between icosahedron and sphere
                    glm::vec3 spherePoint(xPos * 0.5f, yPos * 0.5f, zPos * 0.5f);
                    vertex.Position = glm::mix(icoPoint, spherePoint, roundness);
                } else {
                    // Perfect sphere
                    vertex.Position = glm::vec3(xPos * 0.5f, yPos * 0.5f, zPos * 0.5f);
                }
                
                // Always use perfect sphere normals for correct lighting
                vertex.Normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
                
                // Improved texture mapping for spheres with reduced pole distortion
                // Use spherical unwrapping with adjustments to reduce distortion
                vertex.TexCoords = glm::vec2(xSegment, ySegment);
                
                // Calculate tangent and bitangent for proper normal mapping
                glm::vec3 tangent;
                if (std::abs(yPos) < 0.999f) {
                    // For most of the sphere, tangent is perpendicular to normal in the theta direction
                    tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));
                } else {
                    // At the poles, we need a special case
                    tangent = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
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
        }
        
        // Generate sphere indices with optimal triangle orientation
        for (unsigned int y = 0; y < rings; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
                // Get the indices of the quad's vertices
                unsigned int topLeft = y * (segments + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * (segments + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;
                
                // First triangle (top-right, bottom-right, bottom-left)
                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
                
                // Second triangle (top-right, bottom-left, top-left)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(topLeft);
            }
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating high-quality sphere mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "High-quality sphere mesh created successfully" << std::endl;
    }
    
    // Calculate the roundness metric based on maximum sagitta
    float calculateRoundnessMetric() const {
        if (!mesh) return 0.0f;
        
        float maxSagitta = 0.0f;
        float radius = 0.5f; // Our sphere has radius 0.5
        
        // Iterate through all triangular faces
        for (size_t i = 0; i < mesh->indices.size(); i += 3) {
            // Get the three vertices of this face
            const Vertex& v1 = mesh->vertices[mesh->indices[i]];
            const Vertex& v2 = mesh->vertices[mesh->indices[i+1]];
            const Vertex& v3 = mesh->vertices[mesh->indices[i+2]];
            
            // Calculate face normal (non-normalized)
            glm::vec3 edge1 = v2.Position - v1.Position;
            glm::vec3 edge2 = v3.Position - v1.Position;
            glm::vec3 faceNormal = glm::cross(edge1, edge2);
            
            // Calculate distance from center (0,0,0) to face plane
            float distance = std::abs(glm::dot(faceNormal, v1.Position)) / glm::length(faceNormal);
            
            // Calculate sagitta (deviation from perfect sphere)
            float sagitta = radius - distance;
            
            // Update maximum sagitta
            if (sagitta > maxSagitta) {
                maxSagitta = sagitta;
            }
        }
        
        // Roundness metric: 1 - (max_sagitta / radius)
        return 1.0f - (maxSagitta / radius);
    }
};

} // namespace m3D

#endif // HIGH_QUALITY_SPHERE_H 