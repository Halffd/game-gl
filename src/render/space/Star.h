#ifndef STAR_H
#define STAR_H

#include "CelestialBody.h"

class Star : public CelestialBody {
private:
    float luminosity;    // Star's brightness
    float temperature;   // Surface temperature in Kelvin
    
    // Glow effect resources
    unsigned int glowVAO, glowVBO;
    Shader* glowShader;

public:
    Star(float mass, float radius, float rotationPeriod, float axialTilt, 
         float luminosity, float temperature);
    ~Star();
    
    // Override methods
    void SetupMaterial(Shader &shader) override;
    void Draw(Shader &shader) override;
    
private:
    // Helper methods
    glm::vec3 CalculateColorFromTemperature(float temp);
    void SetupGlowEffect();
    void RenderGlowEffect(Shader &mainShader);
};

#endif // STAR_H