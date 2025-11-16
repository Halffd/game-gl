#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "render/Shader.h"
#include "render/Model.h"

class CelestialBody {
protected:
    // Position and movement
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    float radius;
    
    // Visual properties
    glm::vec3 color;
    unsigned int texture;
    
    // Rotation
    float rotationPeriod;    // Time for one rotation (in days)
    float axialTilt;         // Tilt of rotation axis (in radians)
    float currentRotation;   // Current rotation angle
    
    // OpenGL rendering data
    unsigned int VAO, VBO;
    std::shared_ptr<m3D::Mesh> sphereMesh; // Shared mesh for rendering

public:
    CelestialBody(float mass, float radius, float rotationPeriod, float axialTilt, std::shared_ptr<m3D::Mesh> mesh);
    virtual ~CelestialBody();

    virtual void Update(float deltaTime);
    virtual void Draw(Shader &shader);

    virtual void SetupMaterial(Shader &shader) = 0;

    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetVelocity() const { return velocity; }
    float GetMass() const { return mass; }
    float GetRadius() const { return radius; }
    float GetSchwarzschildRadius() const { return (2.0f * 6.67430e-11f * mass) / (3.0e8f * 3.0e8f); }  // 2GM/c²
    glm::vec3 GetColor() const { return color; }

    // Setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetColor(const glm::vec3& col) { color = col; }

protected: 
    void SetupSphere();
};

#endif // CELESTIAL_BODY_H