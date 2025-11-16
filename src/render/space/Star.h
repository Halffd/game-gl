#ifndef STAR_H
#define STAR_H

#include "CelestialBody.h"
#include <vector>

class Star : public CelestialBody {
private:
    float luminosity;    // Star's brightness (solar units)
    float temperature;   // Surface temperature in Kelvin
    
    // Stellar properties
    struct StellarProperties {
        // Spectral classification
        enum SpectralClass { O, B, A, F, G, K, M } spectralClass;
        enum LuminosityClass { Ia, Ib, II, III, IV, V, VI, VII } luminosityClass;
        
        // Evolution tracking
        float age;                    // Star's age (years)
        float lifespan;              // Expected lifespan (years)
        float fuelRemaining;         // Hydrogen fuel percentage (0-1)
        
        // Physical processes
        float corePressure;          // Core pressure (Pa)
        float coreTemperature;       // Core temperature (K)
        float massLossRate;          // Solar wind mass loss (kg/s)
        float rotationalVelocity;    // Equatorial rotation speed (m/s)
        
        // Magnetic field
        float magneticFieldStrength; // Tesla
        float solarCyclePhase;       // 0-1 for sunspot cycle (11 year cycle)
        float solarCycleTime;        // Time in current cycle
    } properties;
    
    // Surface features
    struct SurfaceFeatures {
        // Sunspots / Starspots
        struct Spot {
            glm::vec2 position;      // Latitude/longitude (radians)
            float radius;            // Spot radius (meters)
            float temperature;       // Cooler than surface
            float lifetime;          // Total lifetime (seconds)
            float age;              // Current age (seconds)
        };
        std::vector<Spot> spots;
        
        // Solar prominences / flares
        struct Prominence {
            glm::vec3 position;
            float height;
            float intensity;
            float duration;
            float age;
        };
        std::vector<Prominence> prominences;
        
        // Granulation pattern (convection cells)
        unsigned int granulationTexture;
        float granulationScale;
        float granulationSpeed;
        float granulationTime;
    } surface;
    
    // Corona (outer atmosphere)
    struct Corona {
        float innerRadius;     // Start of corona
        float outerRadius;     // End of corona
        float density;         // Plasma density
        float temperature;     // Temperature (K)
        unsigned int coronaVAO, coronaVBO;
        int numLayers;
        
        Corona() : innerRadius(0), outerRadius(0), density(0), 
                  temperature(0), coronaVAO(0), coronaVBO(0), numLayers(5) {}
    } corona;
    
    // Solar wind particles
    struct SolarWind {
        struct Particle {
            glm::vec3 position;
            glm::vec3 velocity;
            float lifetime;
            float age;
            float energy;
        };
        
        std::vector<Particle> particles;
        unsigned int particleVAO, particleVBO;
        int maxParticles;
        
        SolarWind() : particleVAO(0), particleVBO(0), maxParticles(10000) {}
    } solarWind;
    
    // HDR and bloom effects
    struct HDRFramebuffer {
        unsigned int FBO;
        unsigned int colorBuffer;
        unsigned int brightBuffer;
        unsigned int depthBuffer;
        int width, height;
        
        HDRFramebuffer() : FBO(0), colorBuffer(0), brightBuffer(0), 
                          depthBuffer(0), width(0), height(0) {}
    } hdrBuffer;
    
    struct BloomEffect {
        unsigned int pingpongFBO[2];
        unsigned int pingpongBuffer[2];
        Shader* blurShader;
        Shader* bloomShader;
        int blurPasses;
        
        BloomEffect() : blurShader(nullptr), bloomShader(nullptr), blurPasses(5) {
            pingpongFBO[0] = pingpongFBO[1] = 0;
            pingpongBuffer[0] = pingpongBuffer[1] = 0;
        }
    } bloom;
    
    // Glow effect resources
    unsigned int glowVAO, glowVBO;
    Shader* glowShader;
    Shader* coronaShader;
    Shader* limbDarkeningShader;

public:
    Star(float mass, float radius, float rotationPeriod, float axialTilt, 
         float luminosity, float temperature, std::shared_ptr<m3D::Mesh> mesh);
    ~Star();
    
    // Override methods
    void Update(float deltaTime) override;
    void SetupMaterial(Shader &shader) override;
    void Draw(Shader &shader) override;
    
    // Getters
    float GetLuminosity() const { return luminosity; }
    float GetTemperature() const { return temperature; }
    float GetAbsoluteMagnitude() const;
    
private:
    // Initialization
    void InitializeProperties();
    void SetupGlowEffect();
    void SetupCorona();
    void SetupSolarWind();
    void SetupHDR(int width, int height);
    void SetupBloom(int width, int height);
    
    // Physical calculations
    float CalculateLuminosityFromPhysics() const;
    float CalculateAbsoluteMagnitude() const;
    void DetermineSpectralClass();
    glm::vec3 CalculateColorFromTemperature(float temp) const;
    
    // Surface features
    void GenerateGranulation();
    void UpdateSunspots(float deltaTime);
    void UpdateProminences(float deltaTime);
    void GenerateProminence();
    
    // Solar wind
    void EmitSolarWind(float deltaTime);
    void UpdateSolarWind(float deltaTime);
    
    // Rendering
    void RenderWithHDR(Shader &shader);
    void RenderGlowEffect(Shader &mainShader);
    void RenderCorona(Shader &mainShader);
    void RenderSolarWind(Shader &mainShader);
    void RenderLimbDarkening(Shader &shader);
    void ApplyBloom();
    
    // Helpers
    float PerlinNoise(float x, float y) const;
    glm::vec3 RandomDirection() const;
};

#endif // STAR_H