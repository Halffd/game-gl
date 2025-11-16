#include "Star.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "asset/ResourceManager.h"

// Forward declaration
static void RenderQuad();

#define PI 3.14159265359f
#define STEFAN_BOLTZMANN 5.670374419e-8f  // W⋅m⁻²⋅K⁻⁴
#define SOLAR_LUMINOSITY 3.828e26f        // Watts
#define SOLAR_MASS 1.989e30f              // kg
#define SOLAR_RADIUS 6.957e8f             // meters

Star::Star(float mass, float radius, float rotationPeriod, float axialTilt,
           float luminosity, float temperature, std::shared_ptr<m3D::Mesh> mesh)
    : CelestialBody(mass, radius, rotationPeriod, axialTilt, mesh),
      luminosity(luminosity), 
      temperature(temperature),
      glowVAO(0),
      glowVBO(0),
      glowShader(nullptr),
      coronaShader(nullptr),
      limbDarkeningShader(nullptr) {
    
    // Initialize random seed
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    
    // Calculate color based on temperature
    color = CalculateColorFromTemperature(temperature);
    
    // Initialize stellar properties
    InitializeProperties();
    
    // Determine spectral classification
    DetermineSpectralClass();
    
    // Setup visual effects
    SetupGlowEffect();
    SetupCorona();
    SetupSolarWind();
    GenerateGranulation();
    
    // Setup HDR and bloom (default 1920x1080)
    SetupHDR(1920, 1080);
    SetupBloom(1920, 1080);
}

Star::~Star() {
    // Clean up glow effect
    if (glowVAO) glDeleteVertexArrays(1, &glowVAO);
    if (glowVBO) glDeleteBuffers(1, &glowVBO);
    
    // Clean up corona
    if (corona.coronaVAO) glDeleteVertexArrays(1, &corona.coronaVAO);
    if (corona.coronaVBO) glDeleteBuffers(1, &corona.coronaVBO);
    
    // Clean up solar wind
    if (solarWind.particleVAO) glDeleteVertexArrays(1, &solarWind.particleVAO);
    if (solarWind.particleVBO) glDeleteBuffers(1, &solarWind.particleVBO);
    
    // Clean up HDR buffers
    if (hdrBuffer.FBO) glDeleteFramebuffers(1, &hdrBuffer.FBO);
    if (hdrBuffer.colorBuffer) glDeleteTextures(1, &hdrBuffer.colorBuffer);
    if (hdrBuffer.brightBuffer) glDeleteTextures(1, &hdrBuffer.brightBuffer);
    if (hdrBuffer.depthBuffer) glDeleteRenderbuffers(1, &hdrBuffer.depthBuffer);
    
    // Clean up bloom buffers
    if (bloom.pingpongFBO[0]) glDeleteFramebuffers(2, bloom.pingpongFBO);
    if (bloom.pingpongBuffer[0]) glDeleteTextures(2, bloom.pingpongBuffer);
    
    // Clean up textures
    if (surface.granulationTexture) glDeleteTextures(1, &surface.granulationTexture);
    
    // Clean up shaders (if owned by this class)
    if (glowShader) delete glowShader;
    if (coronaShader) delete coronaShader;
    if (limbDarkeningShader) delete limbDarkeningShader;
    if (bloom.blurShader) delete bloom.blurShader;
    if (bloom.bloomShader) delete bloom.bloomShader;
}

void Star::InitializeProperties() {
    // Calculate physical properties based on mass and radius
    float solarMasses = mass / SOLAR_MASS;
    float solarRadii = radius / SOLAR_RADIUS;
    
    // Age and lifespan (rough estimate)
    // Main sequence lifetime ≈ 10^10 * (M/M☉)^-2.5 years
    properties.lifespan = 1e10f * powf(solarMasses, -2.5f);
    properties.age = properties.lifespan * ((rand() % 100) / 100.0f); // Random age
    properties.fuelRemaining = 1.0f - (properties.age / properties.lifespan);
    
    // Core temperature (rough estimate)
    // T_core ≈ 15.7 million K * (M/M☉)
    properties.coreTemperature = 1.57e7f * solarMasses;
    
    // Core pressure (rough estimate)
    properties.corePressure = 2.5e16f * powf(solarMasses, 2.0f) / powf(solarRadii, 4.0f);
    
    // Mass loss rate (solar wind)
    // For Sun: ~2×10^9 kg/s
    properties.massLossRate = 2e9f * solarMasses;
    
    // Rotational velocity
    properties.rotationalVelocity = (2.0f * PI * radius) / rotationPeriod;
    
    // Magnetic field strength (rough estimate)
    // Sun's field: ~1-5 Gauss = 1e-4 to 5e-4 Tesla
    properties.magneticFieldStrength = 3e-4f * solarMasses;
    
    // Solar cycle
    properties.solarCyclePhase = 0.0f;
    properties.solarCycleTime = 0.0f;
}

void Star::DetermineSpectralClass() {
    // Harvard spectral classification (OBAFGKM)
    if (temperature >= 30000.0f) {
        properties.spectralClass = StellarProperties::O;
    } else if (temperature >= 10000.0f) {
        properties.spectralClass = StellarProperties::B;
    } else if (temperature >= 7500.0f) {
        properties.spectralClass = StellarProperties::A;
    } else if (temperature >= 6000.0f) {
        properties.spectralClass = StellarProperties::F;
    } else if (temperature >= 5200.0f) {
        properties.spectralClass = StellarProperties::G;
    } else if (temperature >= 3700.0f) {
        properties.spectralClass = StellarProperties::K;
    } else {
        properties.spectralClass = StellarProperties::M;
    }
    
    // Luminosity class (Yerkes classification)
    float solarRadii = radius / SOLAR_RADIUS;
    
    if (solarRadii > 100.0f) {
        properties.luminosityClass = StellarProperties::Ia; // Luminous supergiant
    } else if (solarRadii > 50.0f) {
        properties.luminosityClass = StellarProperties::Ib; // Less luminous supergiant
    } else if (solarRadii > 25.0f) {
        properties.luminosityClass = StellarProperties::II; // Bright giant
    } else if (solarRadii > 10.0f) {
        properties.luminosityClass = StellarProperties::III; // Giant
    } else if (solarRadii > 2.0f) {
        properties.luminosityClass = StellarProperties::IV; // Subgiant
    } else {
        properties.luminosityClass = StellarProperties::V; // Main sequence (dwarf)
    }
}

float Star::CalculateLuminosityFromPhysics() const {
    // Stefan-Boltzmann law: L = 4πR²σT⁴
    float surfaceArea = 4.0f * PI * radius * radius;
    float temp4 = temperature * temperature * temperature * temperature;
    return surfaceArea * STEFAN_BOLTZMANN * temp4;
}

float Star::CalculateAbsoluteMagnitude() const {
    float L = CalculateLuminosityFromPhysics();
    return 4.83f - 2.5f * log10f(L / SOLAR_LUMINOSITY);
}

float Star::GetAbsoluteMagnitude() const {
    return CalculateAbsoluteMagnitude();
}

glm::vec3 Star::CalculateColorFromTemperature(float temp) const {
    // More accurate blackbody radiation color approximation
    // Based on CIE color matching functions
    
    if (temp < 1000.0f) temp = 1000.0f;
    if (temp < 1000.0f) temp = 1000.0f;
    if (temp > 40000.0f) temp = 40000.0f;
    
    // Simplified blackbody color calculation
    float red, green, blue;
    
    // Red component
    if (temp <= 6600.0f) {
        red = 1.0f;
    } else {
        float t = (temp - 6600.0f) / 1000.0f;
        red = 1.292936f - 0.1332047f * t + 0.0159828f * t * t;
        red = glm::clamp(red, 0.0f, 1.0f);
    }
    
    // Green component
    if (temp <= 6600.0f) {
        float t = (temp - 1000.0f) / 1000.0f;
        green = -0.2661239f + 0.2343589f * t - 0.0138723f * t * t;
        green = glm::clamp(green, 0.0f, 1.0f);
    } else {
        float t = (temp - 6600.0f) / 1000.0f;
        green = 1.129803f - 0.0755148f * t;
        green = glm::clamp(green, 0.0f, 1.0f);
    }
    
    // Blue component
    if (temp >= 6600.0f) {
        blue = 1.0f;
    } else if (temp <= 2000.0f) {
        blue = 0.0f;
    } else {
        float t = (temp - 2000.0f) / 1000.0f;
        blue = -0.0164296f + 0.2151504f * t + 0.0281709f * t * t;
        blue = glm::clamp(blue, 0.0f, 1.0f);
    }
    
    return glm::vec3(red, green, blue);
}

void Star::Update(float deltaTime) {
    // Update base celestial body (rotation)
    CelestialBody::Update(deltaTime);
    
    // Update age and evolution
    properties.age += deltaTime / (365.25f * 86400.0f); // Convert to years
    properties.fuelRemaining = 1.0f - (properties.age / properties.lifespan);
    properties.fuelRemaining = glm::max(0.0f, properties.fuelRemaining);
    
    // Update solar cycle (11 years for Sun-like stars)
    const float SOLAR_CYCLE_DURATION = 11.0f * 365.25f * 86400.0f; // seconds
    properties.solarCycleTime += deltaTime;
    if (properties.solarCycleTime > SOLAR_CYCLE_DURATION) {
        properties.solarCycleTime -= SOLAR_CYCLE_DURATION;
    }
    properties.solarCyclePhase = properties.solarCycleTime / SOLAR_CYCLE_DURATION;
    
    // Update surface features
    UpdateSunspots(deltaTime);
    UpdateProminences(deltaTime);
    
    // Update granulation animation
    surface.granulationTime += deltaTime * surface.granulationSpeed;
    
    // Update solar wind
    EmitSolarWind(deltaTime);
    UpdateSolarWind(deltaTime);
}

void Star::UpdateSunspots(float deltaTime) {
    // Sunspot activity varies with solar cycle
    float spotActivity = sinf(properties.solarCyclePhase * 2.0f * PI);
    spotActivity = (spotActivity + 1.0f) * 0.5f; // Normalize to 0-1
    
    // Generate new spots based on activity level
    float spotProbability = spotActivity * 0.05f; // Max 5% chance per second
    
    if ((rand() / (float)RAND_MAX) < spotProbability * deltaTime) {
        SurfaceFeatures::Spot newSpot;
        
        // Latitude: spots appear in bands (±30° from equator)
        float latitudeBand = ((rand() / (float)RAND_MAX) - 0.5f) * (PI / 3.0f);
        newSpot.position.y = latitudeBand; // Latitude
        
        // Longitude: random
        newSpot.position.x = (rand() / (float)RAND_MAX) * 2.0f * PI;
        
        // Size: 0.5% to 5% of stellar radius
        newSpot.radius = radius * (0.005f + (rand() / (float)RAND_MAX) * 0.045f);
        
        // Temperature: 70-80% of surface temperature
        newSpot.temperature = temperature * (0.7f + (rand() / (float)RAND_MAX) * 0.1f);
        
        // Lifetime: 1-4 weeks
        newSpot.lifetime = (1.0f + (rand() / (float)RAND_MAX) * 3.0f) * 7.0f * 86400.0f;
        newSpot.age = 0.0f;
        
        surface.spots.push_back(newSpot);
    }
    
    // Update existing spots
    for (auto it = surface.spots.begin(); it != surface.spots.end();) {
        it->age += deltaTime;
        
        // Remove old spots
        if (it->age > it->lifetime) {
            it = surface.spots.erase(it);
        } else {
            // Spots evolve: grow then shrink
            float lifecycle = it->age / it->lifetime;
            if (lifecycle < 0.3f) {
                // Growing phase
            } else if (lifecycle > 0.7f) {
                // Shrinking phase
            }
            ++it;
        }
    }
    
    // Limit total number of spots
    if (surface.spots.size() > 100) {
        surface.spots.erase(surface.spots.begin());
    }
}

void Star::UpdateProminences(float deltaTime) {
    // Generate solar prominences/flares
    float flareRate = properties.solarCyclePhase * 0.01f; // More flares at solar max
    
    if ((rand() / (float)RAND_MAX) < flareRate * deltaTime) {
        GenerateProminence();
    }
    
    // Update existing prominences
    for (auto it = surface.prominences.begin(); it != surface.prominences.end();) {
        it->age += deltaTime;
        
        if (it->age > it->duration) {
            it = surface.prominences.erase(it);
        } else {
            // Prominences rise and fade
            float lifecycle = it->age / it->duration;
            it->height = radius * 0.2f * sinf(lifecycle * PI);
            it->intensity = sinf(lifecycle * PI);
            ++it;
        }
    }
}

void Star::GenerateProminence() {
    SurfaceFeatures::Prominence prom;
    
    // Random position on surface
    float theta = (rand() / (float)RAND_MAX) * 2.0f * PI;
    float phi = acosf(2.0f * (rand() / (float)RAND_MAX) - 1.0f);
    
    prom.position = glm::vec3(
        radius * sinf(phi) * cosf(theta),
        radius * sinf(phi) * sinf(theta),
        radius * cosf(phi)
    ) + position;
    
    prom.height = 0.0f;
    prom.intensity = 1.0f;
    prom.duration = 1800.0f + (rand() / (float)RAND_MAX) * 3600.0f; // 30-90 minutes
    prom.age = 0.0f;
    
    surface.prominences.push_back(prom);
}

void Star::EmitSolarWind(float deltaTime) {
    // Calculate number of particles to emit this frame
    float particlesPerSecond = properties.massLossRate / 1e15f; // Rough approximation
    int numParticles = (int)(particlesPerSecond * deltaTime);
    
    // Cap at reasonable number
    numParticles = glm::min(numParticles, 100);
    
    for (int i = 0; i < numParticles; i++) {
        // Don't exceed max particles
        if (solarWind.particles.size() >= (size_t)solarWind.maxParticles) {
            break;
        }
        
        SolarWind::Particle particle;
        
        // Random direction from star surface
        glm::vec3 dir = RandomDirection();
        
        particle.position = position + dir * radius;
        
        // Solar wind speed: ~400 km/s average
        float speed = 300000.0f + (rand() / (float)RAND_MAX) * 200000.0f;
        particle.velocity = dir * speed;
        
        particle.lifetime = 100.0f + (rand() / (float)RAND_MAX) * 100.0f;
        particle.age = 0.0f;
        particle.energy = 1000.0f; // eV
        
        solarWind.particles.push_back(particle);
    }
}

void Star::UpdateSolarWind(float deltaTime) {
    for (auto it = solarWind.particles.begin(); it != solarWind.particles.end();) {
        it->position += it->velocity * deltaTime;
        it->age += deltaTime;
        
        // Remove old particles
        if (it->age > it->lifetime) {
            it = solarWind.particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Star::SetupMaterial(Shader &shader) {
    shader.Use();
    
    // Set star material properties
    shader.SetVector3f("material.ambient", color * 0.3f);
    shader.SetVector3f("material.diffuse", color);
    shader.SetVector3f("material.specular", glm::vec3(1.0f));
    shader.SetFloat("material.shininess", 32.0f);
    
    // Stars are self-illuminating
    shader.SetVector3f("material.emission", color * luminosity);
    shader.SetInteger("material.useEmission", 1);
    
    // Bind surface texture (if available)
    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.SetInteger("texture_diffuse1", 0);
    }
    
    // Bind granulation texture
    if (surface.granulationTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, surface.granulationTexture);
        shader.SetInteger("granulationMap", 1);
        shader.SetFloat("granulationTime", surface.granulationTime);
    }
    
    // Pass sunspot data to shader
    shader.SetInteger("numSunspots", (int)surface.spots.size());
    for (size_t i = 0; i < surface.spots.size() && i < 32; i++) {
        std::string base = "sunspots[" + std::to_string(i) + "]";
        shader.SetVector2f((base + ".position").c_str(), surface.spots[i].position);
        shader.SetFloat((base + ".radius").c_str(), surface.spots[i].radius / radius);
        shader.SetFloat((base + ".temperature").c_str(), surface.spots[i].temperature / temperature);
    }
}

void Star::Draw(Shader &shader) {
    // Render with HDR
    RenderWithHDR(shader);
    
    // Draw the star with limb darkening
    RenderLimbDarkening(shader);
    
    // Draw corona
    RenderCorona(shader);
    
    // Draw glow effect
    if (glowShader && glowVAO != 0) {
        RenderGlowEffect(shader);
    }
    
    // Draw solar wind particles
    RenderSolarWind(shader);
    
    // Apply bloom post-processing
    ApplyBloom();
}

void Star::RenderWithHDR(Shader &shader) {
    // Bind HDR framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer.FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render star to HDR buffer
    // (actual rendering happens in other methods)
    
    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Star::RenderLimbDarkening(Shader &shader) {
    // Use limb darkening shader if available
    if (limbDarkeningShader) {
        limbDarkeningShader->Use();
        
        // Set uniforms specific to limb darkening
        limbDarkeningShader->SetFloat("limbDarkeningU1", 0.6f); // Linear coefficient
        limbDarkeningShader->SetFloat("limbDarkeningU2", 0.2f); // Quadratic coefficient
        
        // Draw star body
        CelestialBody::Draw(*limbDarkeningShader);
    } else {
        // Fallback to normal rendering
        CelestialBody::Draw(shader);
    }
}

void Star::SetupGlowEffect() {
    // Create billboard quad for glow
    float glowVertices[] = {
        // positions         // texture coords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &glowVAO);
    glGenBuffers(1, &glowVBO);
    
    glBindVertexArray(glowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glowVertices), glowVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
    
    // Load shader
    try {
        glowShader = &ResourceManager::GetShader("glow");
    } catch (...) {
        glowShader = nullptr;
    }
}

void Star::SetupCorona() {
    // Initialize corona properties
    corona.innerRadius = radius * 1.05f;
    corona.outerRadius = radius * 3.0f;
    corona.temperature = 1e6f; // ~1 million K for solar corona
    corona.density = 1e-12f; // Very low density
    corona.numLayers = 8; // Multiple layers for volumetric effect
    
    // Create sphere geometry for corona layers
    const int sectors = 36;
    const int stacks = 18;
    std::vector<float> vertices;
    
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = PI / 2 - i * PI / stacks;
        float xy = cosf(stackAngle);
        float z = sinf(stackAngle);
        
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * PI / sectors;
            
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    
    glGenVertexArrays(1, &corona.coronaVAO);
    glGenBuffers(1, &corona.coronaVBO);
    
    glBindVertexArray(corona.coronaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, corona.coronaVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
    
    // Load corona shader
    try {
        coronaShader = &ResourceManager::GetShader("corona");
    } catch (...) {
        coronaShader = nullptr;
    }
}

void Star::SetupSolarWind() {
    // Initialize solar wind particle system
    solarWind.maxParticles = 10000;
    solarWind.particles.reserve(solarWind.maxParticles);
    
    // Create VAO for particle rendering
    glGenVertexArrays(1, &solarWind.particleVAO);
    glGenBuffers(1, &solarWind.particleVBO);
    
    glBindVertexArray(solarWind.particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solarWind.particleVBO);
    
    // Allocate buffer for max particles (position + color)
    glBufferData(GL_ARRAY_BUFFER, solarWind.maxParticles * 7 * sizeof(float), 
                 nullptr, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Star::SetupHDR(int width, int height) {
    hdrBuffer.width = width;
    hdrBuffer.height = height;
    
    // Create framebuffer
    glGenFramebuffers(1, &hdrBuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer.FBO);
    
    // HDR color buffer
    glGenTextures(1, &hdrBuffer.colorBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrBuffer.colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                          GL_TEXTURE_2D, hdrBuffer.colorBuffer, 0);
    
    // Bright areas buffer (for bloom extraction)
    glGenTextures(1, &hdrBuffer.brightBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrBuffer.brightBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 
                          GL_TEXTURE_2D, hdrBuffer.brightBuffer, 0);
    
    // Tell OpenGL we're rendering to multiple buffers
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    
    // Depth buffer
    glGenRenderbuffers(1, &hdrBuffer.depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrBuffer.depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                             GL_RENDERBUFFER, hdrBuffer.depthBuffer);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Handle error
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Star::SetupBloom(int width, int height) {
    // Create ping-pong framebuffers for Gaussian blur
    glGenFramebuffers(2, bloom.pingpongFBO);
    glGenTextures(2, bloom.pingpongBuffer);
    
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, bloom.pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, bloom.pingpongBuffer[i]);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 
                     0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                              GL_TEXTURE_2D, bloom.pingpongBuffer[i], 0);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Load blur shader
    try {
        bloom.blurShader = &ResourceManager::GetShader("blur");
    } catch (...) {
        bloom.blurShader = nullptr;
    }
    
    // Load final bloom composition shader
    try {
        bloom.bloomShader = &ResourceManager::GetShader("bloom");
    } catch (...) {
        bloom.bloomShader = nullptr;
    }
}

void Star::GenerateGranulation() {
    // Generate procedural granulation texture (convection cells)
    const int textureSize = 512;
    std::vector<float> pixels(textureSize * textureSize * 3);
    
    surface.granulationScale = 0.05f;
    surface.granulationSpeed = 0.1f;
    surface.granulationTime = 0.0f;
    
    for (int y = 0; y < textureSize; y++) {
        for (int x = 0; x < textureSize; x++) {
            // Multi-octave Perlin noise for realistic convection pattern
            float noise = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            
            for (int octave = 0; octave < 4; octave++) {
                noise += amplitude * PerlinNoise(x * frequency * 0.05f, 
                                                 y * frequency * 0.05f);
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            
            // Normalize to 0-1
            noise = (noise + 1.0f) * 0.5f;
            
            // Add some cell-like structure
            float cellPattern = sinf(x * 0.2f) * sinf(y * 0.2f);
            cellPattern = (cellPattern + 1.0f) * 0.5f;
            
            float finalValue = noise * 0.7f + cellPattern * 0.3f;
            
            int idx = (y * textureSize + x) * 3;
            pixels[idx + 0] = finalValue;
            pixels[idx + 1] = finalValue;
            pixels[idx + 2] = finalValue;
        }
    }
    
    // Create OpenGL texture
    glGenTextures(1, &surface.granulationTexture);
    glBindTexture(GL_TEXTURE_2D, surface.granulationTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 
                 0, GL_RGB, GL_FLOAT, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Star::RenderGlowEffect(Shader &mainShader) {
    if (!glowShader || glowVAO == 0) return;
    
    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    
    glowShader->Use();
    
    // Create billboard model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Scale based on luminosity
    float glowScale = radius * (2.5f + luminosity * 0.5f);
    model = glm::scale(model, glm::vec3(glowScale));
    
    glowShader->SetMatrix4("model", model);
    glowShader->SetVector3f("glowColor", color);
    glowShader->SetFloat("glowIntensity", luminosity);
    
    // Draw glow quad
    glBindVertexArray(glowVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // Restore state
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}

void Star::RenderCorona(Shader &mainShader) {
    if (!coronaShader || corona.coronaVAO == 0) return;
    
    // Enable additive blending for corona
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    
    coronaShader->Use();
    
    // Render multiple layers from outer to inner
    for (int layer = corona.numLayers - 1; layer >= 0; layer--) {
        float t = layer / (float)(corona.numLayers - 1);
        float layerRadius = glm::mix(corona.outerRadius, corona.innerRadius, t);
        float alpha = (1.0f - t) * 0.3f; // Fade out towards edge
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(layerRadius));
        
        coronaShader->SetMatrix4("model", model);
        coronaShader->SetVector3f("coronaColor", color * 1.5f); // Brighter than surface
        coronaShader->SetFloat("coronaAlpha", alpha);
        coronaShader->SetFloat("coronaTemperature", corona.temperature);
        
        // Draw sphere
        glBindVertexArray(corona.coronaVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (36 + 1) * (18 + 1));
        glBindVertexArray(0);
    }
    
    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Star::RenderSolarWind(Shader &mainShader) {
    if (solarWind.particles.empty() || solarWind.particleVAO == 0) return;
    
    // Prepare particle data
    std::vector<float> particleData;
    particleData.reserve(solarWind.particles.size() * 7);
    
    for (const auto& particle : solarWind.particles) {
        // Position
        particleData.push_back(particle.position.x);
        particleData.push_back(particle.position.y);
        particleData.push_back(particle.position.z);
        
        // Color with fade based on age
        float fade = 1.0f - (particle.age / particle.lifetime);
        particleData.push_back(color.r * fade);
        particleData.push_back(color.g * fade);
        particleData.push_back(color.b * fade);
        particleData.push_back(fade * 0.5f); // Alpha
    }
    
    // Update VBO with particle data
    glBindBuffer(GL_ARRAY_BUFFER, solarWind.particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    particleData.size() * sizeof(float), 
                    particleData.data());
    
    // Enable additive blending for particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    
    // Enable point sprites
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    mainShader.Use();
    mainShader.SetMatrix4("model", glm::mat4(1.0f)); // Identity for world space
    
    // Draw particles
    glBindVertexArray(solarWind.particleVAO);
    glDrawArrays(GL_POINTS, 0, (GLsizei)solarWind.particles.size());
    glBindVertexArray(0);
    
    // Restore state
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Star::ApplyBloom() {
    if (!bloom.blurShader || !bloom.bloomShader) return;
    
    // First pass: Gaussian blur (horizontal and vertical)
    bool horizontal = true;
    bool firstIteration = true;
    
    bloom.blurShader->Use();
    
    for (int i = 0; i < bloom.blurPasses * 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, bloom.pingpongFBO[horizontal]);
        bloom.blurShader->SetInteger("horizontal", horizontal);
        
        // Bind texture from previous pass
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, firstIteration ? 
                     hdrBuffer.brightBuffer : bloom.pingpongBuffer[!horizontal]);
        
        // Render full-screen quad
        RenderQuad();
        
        horizontal = !horizontal;
        if (firstIteration) firstIteration = false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Second pass: Combine original HDR scene with blurred bloom
    bloom.bloomShader->Use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrBuffer.colorBuffer);
    bloom.bloomShader->SetInteger("hdrScene", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloom.pingpongBuffer[!horizontal]);
    bloom.bloomShader->SetInteger("bloomBlur", 1);
    
    bloom.bloomShader->SetFloat("exposure", 1.0f);
    bloom.bloomShader->SetFloat("bloomStrength", luminosity * 0.3f);
    
    // Render to screen
    RenderQuad();
}

// Helper function to render a full-screen quad
static void RenderQuad() {
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO = 0;
    
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 
                             (void*)(2 * sizeof(float)));
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

float Star::PerlinNoise(float x, float y) const {
    // Simplified 2D Perlin noise implementation
    // For production, use a proper noise library like FastNoise
    
    int xi = (int)floorf(x);
    int yi = (int)floorf(y);
    
    float xf = x - xi;
    float yf = y - yi;
    
    // Fade curves
    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);
    
    // Hash function for pseudo-random gradients
    auto hash = [](int x, int y) -> float {
        int n = x + y * 57;
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    };
    
    // Interpolate
    float a = hash(xi, yi);
    float b = hash(xi + 1, yi);
    float c = hash(xi, yi + 1);
    float d = hash(xi + 1, yi + 1);
    
    float x1 = glm::mix(a, b, u);
    float x2 = glm::mix(c, d, u);
    
    return glm::mix(x1, x2, v);
}

glm::vec3 Star::RandomDirection() const {
    // Generate random direction on unit sphere
    float theta = (rand() / (float)RAND_MAX) * 2.0f * PI;
    float phi = acosf(2.0f * (rand() / (float)RAND_MAX) - 1.0f);
    
    return glm::vec3(
        sinf(phi) * cosf(theta),
        sinf(phi) * sinf(theta),
        cosf(phi)
    );
}