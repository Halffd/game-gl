#ifndef PLANET_H
#define PLANET_H

#include "CelestialBody.h"
#include <vector>

class Planet : public CelestialBody {
private:
    // Orbital parameters (Keplerian elements)
    CelestialBody* parent;           // Body being orbited (usually a star)
    float orbitalPeriod;             // Time for one orbit (in seconds)
    float semiMajorAxis;             // Average distance from parent (meters)
    float eccentricity;              // Orbital eccentricity (0=circle, 0-1=ellipse)
    float inclination;               // Orbital tilt relative to reference plane (radians)
    float argumentOfPeriapsis;       // ω - rotation of ellipse in orbital plane (radians)
    float longitudeAscendingNode;    // Ω - longitude of ascending node (radians)
    float meanAnomalyAtEpoch;        // M₀ - mean anomaly at epoch (radians)
    float currentOrbitalAngle;       // Current position in orbit (true anomaly, radians)
    
    // Time tracking
    float totalTime;                 // Accumulated time for orbit calculations
    
    // Perturbations (optional corrections to Keplerian orbit)
    struct {
        float j2Factor;              // Oblateness effect (J2 coefficient)
        float relativisticFactor;    // GR precession strength (0-1 scale)
        glm::vec3 externalForce;     // External force vector (Newtons)
    } perturbation;
    
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
           float eccentricity, float inclination,
           float argumentOfPeriapsis = 0.0f, 
           float longitudeAscendingNode = 0.0f,
           float meanAnomalyAtEpoch = 0.0f);
    ~Planet();
    
    // Override methods
    void Update(float deltaTime) override;
    void Draw(Shader &shader) override;
    void SetupMaterial(Shader &shader) override;
    
    // Planet-specific methods
    void AddSatellite(CelestialBody* satellite);
    void EnableRings(unsigned int texture, float innerRadius, float outerRadius);
    
    // Perturbation controls
    void SetPerturbationJ2(float factor);
    void SetRelativisticFactor(float factor);
    void ApplyExternalForce(const glm::vec3& force);
    
    // Getters
    float GetOrbitalPeriod() const { return orbitalPeriod; }
    float GetSemiMajorAxis() const { return semiMajorAxis; }
    float GetEccentricity() const { return eccentricity; }
    float GetCurrentOrbitalAngle() const { return currentOrbitalAngle; }
    
private:
    // Helper methods
    glm::vec3 RotateAroundY(const glm::vec3& point, float angle) const;
    glm::vec3 ApplyFullOrbitalOrientation(const glm::vec3& position, 
                                          float inclination, 
                                          float longitudeAscendingNode) const;
    
    // Orbital mechanics helpers
    float SolveKeplersEquation(float M, float e, int iterations = 10) const;
    float EccentricToTrueAnomaly(float E, float e) const;
    
    // Ring rendering
    void SetupRings();
    void RenderRings(Shader &shader);
};

#endif // PLANET_H