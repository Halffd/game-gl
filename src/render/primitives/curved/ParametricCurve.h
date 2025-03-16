#ifndef PARAMETRIC_CURVE_SHAPE_H
#define PARAMETRIC_CURVE_SHAPE_H

#include "../PrimitiveShape.h"
#include <functional>

namespace m3D {

// Parametric curve shape
class ParametricCurve : public PrimitiveShape {
public:
    // Function type for parametric curves
    using ParametricFunction = std::function<glm::vec3(float)>;
    
    ParametricCurve(const std::string& name, 
                   const glm::vec3& position = glm::vec3(0.0f),
                   const glm::vec3& rotation = glm::vec3(0.0f),
                   const glm::vec3& scale = glm::vec3(1.0f),
                   const glm::vec3& color = glm::vec3(1.0f, 0.3f, 0.7f),
                   ParametricFunction func = &ParametricCurve::helix,
                   float thickness = 0.05f,
                   unsigned int segments = 100,
                   unsigned int sides = 8,
                   float tMin = 0.0f,
                   float tMax = 2.0f * glm::pi<float>()) 
        : PrimitiveShape(name, position, rotation, scale, color) {
        
        std::cout << "Creating Parametric Curve: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Generate curve points
        std::vector<glm::vec3> curvePoints;
        std::vector<glm::vec3> tangents;
        
        for (unsigned int i = 0; i <= segments; ++i) {
            float t = tMin + (tMax - tMin) * static_cast<float>(i) / segments;
            
            // Calculate point on curve
            glm::vec3 point = func(t);
            curvePoints.push_back(point);
            
            // Calculate tangent (using finite difference)
            float delta = 0.001f;
            glm::vec3 nextPoint = func(t + delta);
            glm::vec3 tangent = glm::normalize(nextPoint - point);
            tangents.push_back(tangent);
        }
        
        // Generate tube around the curve
        for (unsigned int i = 0; i <= segments; ++i) {
            // Get current point and tangent
            glm::vec3 center = curvePoints[i];
            glm::vec3 tangent = tangents[i];
            
            // Create a coordinate system at this point
            glm::vec3 normal;
            if (std::abs(tangent.y) < 0.999f) {
                normal = glm::normalize(glm::cross(tangent, glm::vec3(0.0f, 1.0f, 0.0f)));
            } else {
                normal = glm::normalize(glm::cross(tangent, glm::vec3(1.0f, 0.0f, 0.0f)));
            }
            
            glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
            
            // Generate circle around the point
            for (unsigned int j = 0; j <= sides; ++j) {
                float angle = 2.0f * glm::pi<float>() * static_cast<float>(j) / sides;
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                
                // Calculate position on the circle
                glm::vec3 circlePoint = center + thickness * (normal * cosA + binormal * sinA);
                
                // Calculate normal (pointing outward from the tube)
                glm::vec3 vertexNormal = glm::normalize(circlePoint - center);
                
                // Texture coordinates
                float u = static_cast<float>(i) / segments;
                float v = static_cast<float>(j) / sides;
                
                // Add vertex
                Vertex vertex;
                vertex.Position = circlePoint;
                vertex.Normal = vertexNormal;
                vertex.TexCoords = glm::vec2(u, v);
                
                // Calculate tangent and bitangent
                vertex.Tangent = tangent;
                vertex.Bitangent = glm::normalize(glm::cross(vertexNormal, tangent));
                
                // Initialize bone weights to 0
                for (int k = 0; k < MAX_BONE_INFLUENCE; k++) {
                    vertex.m_BoneIDs[k] = 0;
                    vertex.m_Weights[k] = 0.0f;
                }
                
                vertices.push_back(vertex);
            }
        }
        
        // Generate indices for the tube
        for (unsigned int i = 0; i < segments; ++i) {
            for (unsigned int j = 0; j < sides; ++j) {
                unsigned int current = i * (sides + 1) + j;
                unsigned int next = current + (sides + 1);
                
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
        
        std::cout << "Creating parametric curve mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Parametric curve mesh created successfully" << std::endl;
    }
    
    // Predefined parametric curves
    
    // Helix curve
    static glm::vec3 helix(float t) {
        float radius = 0.3f;
        float height = 0.1f;
        return glm::vec3(
            radius * std::cos(t),
            height * t,
            radius * std::sin(t)
        );
    }
    
    // Trefoil knot
    static glm::vec3 trefoilKnot(float t) {
        float radius = 0.3f;
        return glm::vec3(
            radius * (std::sin(t) + 2.0f * std::sin(2.0f * t)),
            radius * (std::cos(t) - 2.0f * std::cos(2.0f * t)),
            radius * (-std::sin(3.0f * t))
        );
    }
    
    // Torus knot
    static glm::vec3 torusKnot(float t) {
        float radius = 0.3f;
        float p = 2.0f; // Number of times around the torus
        float q = 3.0f; // Number of times through the hole
        
        float r = radius * (0.5f + 0.1f * std::cos(q * t));
        return glm::vec3(
            r * std::cos(p * t),
            r * std::sin(p * t),
            radius * 0.2f * std::sin(q * t)
        );
    }
    
    // Lissajous curve
    static glm::vec3 lissajous(float t) {
        float radius = 0.3f;
        return glm::vec3(
            radius * std::sin(2.0f * t),
            radius * std::sin(3.0f * t),
            radius * std::sin(5.0f * t)
        );
    }
};

} // namespace m3D

#endif // PARAMETRIC_CURVE_SHAPE_H 