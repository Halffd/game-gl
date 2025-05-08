#include "CelestialBody.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define PI 3.14159265359f

CelestialBody::CelestialBody(float mass, float radius, float rotationPeriod, float axialTilt)
    : mass(mass), radius(radius), rotationPeriod(rotationPeriod), axialTilt(axialTilt) {
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    currentRotation = 0.0f;
    color = glm::vec3(1.0f); // Default white
    texture = 0;

    // Initialize sphere model for rendering
    sphereModel = nullptr;
    SetupSphere();
}

CelestialBody::~CelestialBody() {
    // Clean up OpenGL resources
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }

    // Clean up model if it was created internally
    if (sphereModel) {
        delete sphereModel;
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
    if (sphereModel) {
        sphereModel->Draw(shader);
    } else {
        // Fallback - bind VAO and draw directly
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 2880); // Assuming 960 triangles for a sphere (can be adjusted)
        glBindVertexArray(0);
    }
}

void CelestialBody::SetupSphere() {
    // This would ideally load a sphere model or create a UV sphere mesh procedurally
    // For simplicity, we'll assume there's an existing sphere model or a utility to create one

    // Option 1: Load model from file
    // sphereModel = new Model("resources/models/sphere/sphere.obj");

    // Option 2: Create procedural sphere (stub - in a real implementation this would create actual geometry)
    // or use a primitive shape helper from your engine

    // For now, we'll leave this as a placeholder to be implemented
    // based on how your engine handles primitive shapes
    std::cout << "Note: Sphere generation needs to be implemented based on your engine's model system\n";
}