#ifndef PLANET_H
#define PLANET_H

#include "CelestialBody.h"
#include <vector>

class Planet : public CelestialBody {
private:
    // Orbital parameters
    CelestialBody* parent;       // Body being orbited (usually a star)
    float orbitalPeriod;         // Time for one orbit (in days)
    float semiMajorAxis;         // Average distance from parent
    float eccentricity;          // Orbital eccentricity (0=circle, 0-1=ellipse)
    float inclination;           // Orbital tilt relative to parent's equator
    float currentOrbitalAngle;   // Current position in orbit
    
    // Ring system
    bool hasRings;
    unsigned int ringTexture;
    float ringInnerRadius;
    float ringOuterRadius;
    unsigned int ringVAO, ringVBO;
    
    // Satellites
    std::vector<CelestialBody*> satellites;  // Moons or other orbiting bodies

public:
    Planet(float mass, float radius, float rotationPeriod, float axialTilt,
           CelestialBody* parent, float orbitalPeriod, float semiMajorAxis, 
           float eccentricity, float inclination);
    ~Planet();
    
    // Override methods
    void Update(float deltaTime) override;
    void Draw(Shader &shader) override;
    void SetupMaterial(Shader &shader) override;
    
    // Planet-specific methods
    void AddSatellite(CelestialBody* satellite);
    void EnableRings(unsigned int texture, float innerRadius, float outerRadius);
    
private:
    // Helper methods
    glm::vec3 ApplyOrbitalInclination(const glm::vec3& position, float inclination) const;
    void SetupRings();
    void RenderRings(Shader &shader);
};

#endif // PLANET_H