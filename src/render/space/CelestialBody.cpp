#include "CelestialBody.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define PI 3.14159265359f

CelestialBody::CelestialBody(float mass, float radius, float rotationPeriod, float axialTilt, std::shared_ptr<m3D::Mesh> mesh)
    : mass(mass), radius(radius), rotationPeriod(rotationPeriod), axialTilt(axialTilt), VAO(0), VBO(0), texture(0), sphereMesh(mesh) {
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    currentRotation = 0.0f;
    color = glm::vec3(1.0f); // Default white
    texture = 0;
}

CelestialBody::~CelestialBody() {
    // Clean up OpenGL resources
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
}

void CelestialBody::Update(float deltaTime) {
    // Update rotation
    currentRotation += (deltaTime / rotationPeriod) * 2.0f * PI;

    // Keep angle in 0-2π range
    while (currentRotation > 2.0f * PI) {
        currentRotation -= 2.0f * PI;
    }
}

void CelestialBody::Draw(Shader &shader) {
    // Save previous state
    shader.Use();

    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);

    // Apply position
    model = glm::translate(model, position);

    // Apply axial tilt and rotation
    model = glm::rotate(model, currentRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y
    model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));       // Apply axial tilt

    // Apply scale for radius
    model = glm::scale(model, glm::vec3(radius));

    // Set model matrix in shader
    shader.SetMatrix4("model", model);

    // Apply material properties
    SetupMaterial(shader);

    // Draw the sphere
    if (sphereMesh)
        sphereMesh->Draw(shader);
    else {
        // Fallback: Draw using VAO/VBO if no model is available
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, 0); // This needs proper index count
        glBindVertexArray(0);
    }
}
