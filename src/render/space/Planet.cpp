//
// Created by half on 5/8/25.
//

#include "Planet.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>
#define PI 3.14159265359f

Planet::Planet(float mass, float radius, float rotationPeriod, float axialTilt,
               CelestialBody* parent, float orbitalPeriod, float semiMajorAxis,
               float eccentricity, float inclination)
    : CelestialBody(mass, radius, rotationPeriod, axialTilt),
      parent(parent), orbitalPeriod(orbitalPeriod), semiMajorAxis(semiMajorAxis),
      eccentricity(eccentricity), inclination(inclination) {

    // Initialize random seed if needed
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // Set random initial orbital position
    currentOrbitalAngle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * PI;

    // Initialize rings
    hasRings = false;
    ringTexture = 0;
    ringInnerRadius = 0.0f;
    ringOuterRadius = 0.0f;
    ringVAO = 0;
    ringVBO = 0;

    // Calculate initial position
    Update(0.0f);
}

Planet::~Planet() {
    // Clean up ring resources
    if (ringVAO) {
        glDeleteVertexArrays(1, &ringVAO);
    }
    if (ringVBO) {
        glDeleteBuffers(1, &ringVBO);
    }

    // Note: We don't delete satellites because they might be managed elsewhere
}

void Planet::Update(float deltaTime) {
    // First update rotation (from parent class)
    Update(deltaTime);

    // Update orbital position
    currentOrbitalAngle += (deltaTime / orbitalPeriod) * 2.0f * PI;

    // Keep angle in 0-2π range
    while (currentOrbitalAngle > 2.0f * PI) {
        currentOrbitalAngle -= 2.0f * PI;
    }

    // Calculate position in orbital plane using Kepler's formula
    float distance = semiMajorAxis * (1.0f - eccentricity * eccentricity) /
                    (1.0f + eccentricity * cos(currentOrbitalAngle));

    // Position in orbital plane
    glm::vec3 orbitPosition = glm::vec3(
        distance * cos(currentOrbitalAngle),
        0.0f,
        distance * sin(currentOrbitalAngle)
    );

    // Apply inclination
    glm::vec3 inclinedPosition = ApplyOrbitalInclination(orbitPosition, inclination);

    // Set position relative to parent
    if (parent) {
        position = parent->GetPosition() + inclinedPosition;
    } else {
        position = inclinedPosition;
    }

    // Update satellites (moons)
    for (auto& satellite : satellites) {
        satellite->Update(deltaTime);
    }
}

void Planet::Draw(Shader &shader) {
    // Draw the planet
    Draw(shader);

    // Draw rings if present
    if (hasRings) {
        RenderRings(shader);
    }

    // Draw satellites (moons)
    for (auto& satellite : satellites) {
        satellite->Draw(shader);
    }
}

void Planet::SetupMaterial(Shader &shader) {
    // Set planet material properties
    shader.SetVector3f("material.ambient", color * 0.2f);
    shader.SetVector3f("material.diffuse", color);
    shader.SetVector3f("material.specular", glm::vec3(0.5f));
    shader.SetFloat("material.shininess", 50.0f);

    // Planets don't emit light on their own
    shader.SetVector3f("material.emission", glm::vec3(0.0f));
    shader.SetInteger("material.useEmission", 0);

    // Bind texture if available
    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.SetInteger("texture_diffuse1", 0);
    }
}

void Planet::AddSatellite(CelestialBody* satellite) {
    satellites.push_back(satellite);
}

void Planet::EnableRings(unsigned int texture, float innerRadius, float outerRadius) {
    // Enable rings for this planet (like Saturn)
    hasRings = true;
    ringTexture = texture;
    ringInnerRadius = innerRadius;
    ringOuterRadius = outerRadius;

    // Set up ring geometry
    SetupRings();
}

glm::vec3 Planet::ApplyOrbitalInclination(const glm::vec3& position, float inclination) const {
    // Create a rotation matrix to apply the inclination
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), inclination, glm::vec3(1.0f, 0.0f, 0.0f));

    // Apply rotation to the position vector
    glm::vec4 rotatedPos = rotMat * glm::vec4(position, 1.0f);

    return glm::vec3(rotatedPos);
}

void Planet::SetupRings() {
    // Create ring geometry - a flat disc with inner hole
    std::vector<float> ringVertices;
    const int segments = 64;

    // Create vertices for a flat ring
    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * PI * float(i) / segments;
        float angle2 = 2.0f * PI * float(i + 1) / segments;

        float s1 = sin(angle1);
        float c1 = cos(angle1);
        float s2 = sin(angle2);
        float c2 = cos(angle2);

        // Inner vertex 1
        ringVertices.push_back(ringInnerRadius * c1); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringInnerRadius * s1); // z
        ringVertices.push_back(0.0f);                 // tex u
        ringVertices.push_back(0.0f);                 // tex v

        // Outer vertex 1
        ringVertices.push_back(ringOuterRadius * c1); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringOuterRadius * s1); // z
        ringVertices.push_back(1.0f);                 // tex u
        ringVertices.push_back(0.0f);                 // tex v

        // Outer vertex 2
        ringVertices.push_back(ringOuterRadius * c2); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringOuterRadius * s2); // z
        ringVertices.push_back(1.0f);                 // tex u
        ringVertices.push_back(1.0f);                 // tex v

        // Inner vertex 1
        ringVertices.push_back(ringInnerRadius * c1); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringInnerRadius * s1); // z
        ringVertices.push_back(0.0f);                 // tex u
        ringVertices.push_back(0.0f);                 // tex v

        // Outer vertex 2
        ringVertices.push_back(ringOuterRadius * c2); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringOuterRadius * s2); // z
        ringVertices.push_back(1.0f);                 // tex u
        ringVertices.push_back(1.0f);                 // tex v

        // Inner vertex 2
        ringVertices.push_back(ringInnerRadius * c2); // x
        ringVertices.push_back(0.0f);                 // y
        ringVertices.push_back(ringInnerRadius * s2); // z
        ringVertices.push_back(0.0f);                 // tex u
        ringVertices.push_back(1.0f);                 // tex v
    }

    // Create VAO and VBO for rings
    glGenVertexArrays(1, &ringVAO);
    glGenBuffers(1, &ringVBO);

    glBindVertexArray(ringVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
    glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * sizeof(float), ringVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void Planet::RenderRings(Shader &shader) {
    if (!hasRings || ringVAO == 0) return;

    // Save current shader settings
    shader.Use();

    // Create model matrix for rings
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // Apply axial tilt to rings
    model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));

    // Set model matrix in shader
    shader.SetMatrix4("model", model);

    // Set ring-specific material properties
    shader.SetInteger("material.useEmission", 0);
    shader.SetInteger("isRing", 1); // Special flag to handle ring transparency differently

    // Bind ring texture
    if (ringTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ringTexture);
        shader.SetInteger("texture_diffuse1", 0);
    }

    // Enable alpha blending for rings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing but keep depth testing
    glDepthMask(GL_FALSE);

    // Draw rings
    glBindVertexArray(ringVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * 64); // 6 vertices per segment, 64 segments
    glBindVertexArray(0);

    // Reset state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    shader.SetInteger("isRing", 0);
}