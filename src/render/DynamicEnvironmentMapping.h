#ifndef DYNAMIC_ENVIRONMENT_MAPPING_H
#define DYNAMIC_ENVIRONMENT_MAPPING_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

class DynamicEnvironmentMapping {
public:
    struct ReflectionProbe {
        glm::vec3 position;
        unsigned int framebuffer;
        unsigned int textureID;
        unsigned int depthMap;
        float nearPlane;
        float farPlane;
        bool needsUpdate;
        
        ReflectionProbe(const glm::vec3& pos) 
            : position(pos), framebuffer(0), textureID(0), depthMap(0), 
              nearPlane(0.1f), farPlane(25.0f), needsUpdate(true) {}
    };

private:
    std::vector<ReflectionProbe> probes;
    std::vector<glm::mat4> shadowTransforms;
    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int envCubemap;
    
    // Shader program for rendering the scene to the cubemap
    unsigned int shaderProgram;
    
    // View matrices for the 6 sides of the cubemap
    std::vector<glm::mat4> captureViews;
    glm::mat4 captureProjection;

public:
    DynamicEnvironmentMapping();
    ~DynamicEnvironmentMapping();
    
    void initialize();
    void cleanup();
    
    // Add a reflection probe at the specified position
    void addReflectionProbe(const glm::vec3& position);
    
    // Update all reflection probes
    void updateProbes();
    
    // Render the scene from the perspective of a probe
    void renderProbe(int probeIndex, 
                     void (*renderSceneCallback)(const glm::mat4&, const glm::mat4&));
    
    // Get the cubemap texture for a specific probe
    unsigned int getProbeCubemap(int probeIndex) const;
    
    // Get the closest probe to a given position
    int getClosestProbe(const glm::vec3& position) const;
    
    // Get probe data
    const ReflectionProbe& getProbe(int index) const { return probes[index]; }
    size_t getProbeCount() const { return probes.size(); }
    
    // Set shader program
    void setShaderProgram(unsigned int program) { shaderProgram = program; }
    
    // Update probe position
    void updateProbePosition(int index, const glm::vec3& newPosition);
    
    // Mark probe for update
    void markForUpdate(int index);
    
    // Render all probes
    void renderAllProbes(void (*renderSceneCallback)(const glm::mat4&, const glm::mat4&));
};

#endif // DYNAMIC_ENVIRONMENT_MAPPING_H