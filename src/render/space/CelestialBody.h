#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
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
    Model* sphereModel;      // Using model class for sphere rendering

public:
    CelestialBody(float mass, float radius, float rotationPeriod, float axialTilt);
    virtual ~CelestialBody();
    
    virtual void Update(float deltaTime);
    virtual void Draw(Shader &shader);
    
    virtual void SetupMaterial(Shader &shader) = 0;
    
    // Getters
    glm::vec3 GetPosition() const { return position; }
    float GetMass() const { return mass; }
    float GetRadius() const { return radius; }
    
protected:
    void SetupSphere();
};

#endif // CELESTIAL_BODY_H