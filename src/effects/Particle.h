#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <glm/glm.hpp>
#include "../render/Shader.h"
#include "../asset/Texture2D.h"
#include "../game/GameObject.h"

// Forward declarations
class SpriteRenderer;

/**
 * @brief Represents a single particle in the particle system
 */
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() 
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

/**
 * @brief Manages a collection of particles for visual effects
 */
class ParticleGenerator {
public:
    /**
     * @brief Constructor
     */
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

    /**
     * @brief Update all particles
     */
    void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    
    /**
     * @brief Draw all particles
     */
    void Draw();

private:
    std::vector<Particle> particles;
    Shader shader;
    Texture2D texture;
    unsigned int amount;
    unsigned int VAO;

    /**
     * @brief Initialize the buffer and vertex attributes
     */
    void init();
    
    /**
     * @brief Returns the first Particle index that's currently unused
     */
    unsigned int firstUnusedParticle();
    
    /**
     * @brief Respawns particle
     */
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif // PARTICLE_H