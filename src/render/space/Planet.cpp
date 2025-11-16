#include "Planet.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define PI 3.14159265359f

Planet::Planet(float mass, float radius, float rotationPeriod, float axialTilt,
               CelestialBody* parent, float orbitalPeriod, float semiMajorAxis,
               float eccentricity, float inclination,
               std::shared_ptr<m3D::Mesh> mesh,
               float argumentOfPeriapsis, float longitudeAscendingNode,
               float meanAnomalyAtEpoch)
    : CelestialBody(mass, radius, rotationPeriod, axialTilt, mesh),
      parent(parent), 
      orbitalPeriod(orbitalPeriod), 
      semiMajorAxis(semiMajorAxis),
      eccentricity(eccentricity), 
      inclination(inclination),
      argumentOfPeriapsis(argumentOfPeriapsis), 
      longitudeAscendingNode(longitudeAscendingNode),
      meanAnomalyAtEpoch(meanAnomalyAtEpoch),
      totalTime(0.0f),
      hasRings(false), 
      ringTexture(0), 
      ringInnerRadius(0.0f),
      ringOuterRadius(0.0f),
      ringVAO(0), 
      ringVBO(0) {
    
    // Initialize random seed if needed
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    
    // Initialize perturbations
    perturbation.j2Factor = 0.0f;
    perturbation.relativisticFactor = 0.0f;
    perturbation.externalForce = glm::vec3(0.0f);
    
    // Set random initial orbital position
    float randomFactor = static_cast<float>(std::rand()) / RAND_MAX;
    float meanAnomaly = randomFactor * 2.0f * PI;
    float eccentricAnomaly = SolveKeplersEquation(meanAnomaly, eccentricity);
    currentOrbitalAngle = EccentricToTrueAnomaly(eccentricAnomaly, eccentricity);
    
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

float Planet::SolveKeplersEquation(float M, float e, int maxIterations) const {
    // Normalize M to [0, 2π]
    M = fmodf(M, 2.0f * PI);
    if (M < 0) M += 2.0f * PI;
    
    // Initial guess (improved for high eccentricity)
    float E = (e < 0.8f) ? M : PI;
    
    for (int i = 0; i < maxIterations; i++) {
        float sinE = sinf(E);
        float cosE = cosf(E);
        float f = E - e * sinE - M;
        
        // Check for convergence
        if (fabsf(f) < 1e-6f) break;
        
        // Newton-Raphson step
        float df = 1.0f - e * cosE;
        float dE = f / df;
        
        // Safeguard against large steps
        E -= (fabsf(dE) > 1.0f) ? copysignf(1.0f, dE) : dE;
    }
    
    return E;
}

float Planet::EccentricToTrueAnomaly(float E, float e) const {
    // Handle circular and near-circular orbits
    if (e < 1e-3f) return E;
    
    // Clamp eccentricity for numerical stability
    float clampedEccentricity = (e > 0.99f) ? 0.99f : e;
    
    // Calculate true anomaly using the correct formula
    float sqrtTerm = sqrtf((1.0f + clampedEccentricity) / (1.0f - clampedEccentricity));
    float trueAnomaly = 2.0f * atan2f(sqrtTerm * sinf(E/2.0f), cosf(E/2.0f));
    
    // Ensure the result is in [0, 2π)
    if (trueAnomaly < 0) trueAnomaly += 2.0f * PI;
    return trueAnomaly;
}

void Planet::Update(float deltaTime) {
    // First update rotation (from parent class)
    CelestialBody::Update(deltaTime);
    
    // Update time tracking
    totalTime += deltaTime;
    
    // Normalize totalTime periodically to prevent floating-point precision issues
    if (totalTime > orbitalPeriod * 100.0f) {
        totalTime = fmodf(totalTime, orbitalPeriod);
    }
    
    // Calculate mean anomaly
    float meanMotion = 2.0f * PI / orbitalPeriod;
    float meanAnomaly = meanAnomalyAtEpoch + meanMotion * totalTime;
    
    // Apply relativistic precession if enabled
    if (perturbation.relativisticFactor > 0.0f && eccentricity > 0.0f) {
        // Simplified relativistic precession formula
        // Real formula: (6πGM)/(c²a(1-e²)) per orbit
        float precessionRate = 3.0f * PI * powf(meanMotion * semiMajorAxis, 2.0f/3.0f) / 
                              (1.0f - eccentricity * eccentricity);
        meanAnomaly += perturbation.relativisticFactor * precessionRate * totalTime;
    }
    
    // Solve Kepler's equation for eccentric anomaly
    float eccentricAnomaly = SolveKeplersEquation(meanAnomaly, eccentricity);
    
    // Convert to true anomaly (current orbital angle)
    currentOrbitalAngle = EccentricToTrueAnomaly(eccentricAnomaly, eccentricity);
    
    // Calculate distance from focus using eccentric anomaly
    float distance = semiMajorAxis * (1.0f - eccentricity * cosf(eccentricAnomaly));
    
    // Position in orbital plane (in the 2D plane of the orbit)
    glm::vec3 orbitPosition = glm::vec3(
        distance * cosf(currentOrbitalAngle),
        0.0f,
        distance * sinf(currentOrbitalAngle)
    );
    
    // Apply argument of periapsis (rotation of ellipse in orbital plane)
    orbitPosition = RotateAroundY(orbitPosition, argumentOfPeriapsis);
    
    // Apply full 3D orientation (inclination + longitude of ascending node)
    glm::vec3 orientedPosition = ApplyFullOrbitalOrientation(
        orbitPosition, inclination, longitudeAscendingNode
    );
    
    // Apply J2 perturbation (oblateness effect) if enabled
    if (perturbation.j2Factor > 0.0f && parent) {
        float r = glm::length(orientedPosition);
        if (r > 0.0f) {
            float R = parent->GetRadius();
            float z = orientedPosition.y; // Y is up in this coordinate system
            float r2 = r * r;
            float z2 = z * z;
            
            // J2 perturbation formula
            float j2Term = 1.5f * perturbation.j2Factor * (R * R) / r2;
            float factor_z = j2Term * (5.0f * z2 / r2 - 1.0f);
            float factor_xy = j2Term * (5.0f * z2 / r2 - 3.0f);
            
            // Apply perturbation (small correction to position)
            orientedPosition.x *= (1.0f + factor_xy / r);
            orientedPosition.y *= (1.0f + factor_z / r);
            orientedPosition.z *= (1.0f + factor_xy / r);
        }
    }
    
    // Apply external forces (simplified acceleration integration)
    if (glm::dot(perturbation.externalForce, perturbation.externalForce) > 0.0f) {
        // F = ma → a = F/m
        // Δx = ½at² (simplified, assumes constant force)
        orientedPosition += (perturbation.externalForce / mass) * 
                           0.5f * deltaTime * deltaTime;
    }
    
    // Set position relative to parent
    if (parent) {
        position = parent->GetPosition() + orientedPosition;
    } else {
        position = orientedPosition;
    }
    
    // Update satellites (moons)
    for (auto& satellite : satellites) {
        satellite->Update(deltaTime);
    }
}

void Planet::Draw(Shader &shader) {
    // Draw the planet (call parent class method)
    CelestialBody::Draw(shader);
    
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
    if (satellite) {
        satellites.push_back(satellite);
    }
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

glm::vec3 Planet::RotateAroundY(const glm::vec3& point, float angle) const {
    // Rotate around Y-axis (for argument of periapsis)
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    
    return glm::vec3(        point.x * cosA - point.z * sinA,
        point.y,
        point.x * sinA + point.z * cosA
    );
}

glm::vec3 Planet::ApplyFullOrbitalOrientation(const glm::vec3& position, 
                                               float inclination, 
                                               float longitudeAscendingNode) const {
    // Apply full 3D orbital orientation using two rotations
    
    // 1. First rotation: around Z axis by longitude of ascending node (Ω)
    float cosOmega = cosf(longitudeAscendingNode);
    float sinOmega = sinf(longitudeAscendingNode);
    
    glm::vec3 rotated1 = glm::vec3(
        position.x * cosOmega - position.y * sinOmega,
        position.x * sinOmega + position.y * cosOmega,
        position.z
    );
    
    // 2. Second rotation: around X axis by inclination (i)
    float cosI = cosf(inclination);
    float sinI = sinf(inclination);
    
    return glm::vec3(
        rotated1.x,
        rotated1.y * cosI - rotated1.z * sinI,
        rotated1.y * sinI + rotated1.z * cosI
    );
}

void Planet::SetPerturbationJ2(float factor) {
    perturbation.j2Factor = factor;
}

void Planet::SetRelativisticFactor(float factor) {
    // Clamp to [0, 1] range
    perturbation.relativisticFactor = glm::clamp(factor, 0.0f, 1.0f);
}

void Planet::ApplyExternalForce(const glm::vec3& force) {
    perturbation.externalForce = force;
}

void Planet::SetupRings() {
    // Create ring geometry - a flat disc with inner hole
    std::vector<float> ringVertices;
    const int segments = 64;
    
    // Create vertices for a flat ring using triangle pairs
    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * PI * float(i) / segments;
        float angle2 = 2.0f * PI * float(i + 1) / segments;
        
        float s1 = sinf(angle1);
        float c1 = cosf(angle1);
        float s2 = sinf(angle2);
        float c2 = cosf(angle2);
        
        // First triangle of the quad
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
        
        // Second triangle of the quad
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
    glBufferData(GL_ARRAY_BUFFER, 
                 ringVertices.size() * sizeof(float), 
                 ringVertices.data(), 
                 GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Planet::RenderRings(Shader &shader) {
    if (!hasRings || ringVAO == 0) return;
    
    // Use the shader
    shader.Use();
    
    // Create model matrix for rings
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Apply axial tilt to rings (rings aligned with planet's equator)
    model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Set model matrix in shader
    shader.SetMatrix4("model", model);
    
    // Set ring-specific material properties
    shader.SetInteger("material.useEmission", 0);
    shader.SetInteger("isRing", 1); // Special flag for ring transparency
    
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
    // (allows rings to be semi-transparent)
    glDepthMask(GL_FALSE);
    
    // Draw rings
    glBindVertexArray(ringVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * 64); // 6 vertices per segment, 64 segments
    glBindVertexArray(0);
    
    // Reset OpenGL state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    shader.SetInteger("isRing", 0);
}