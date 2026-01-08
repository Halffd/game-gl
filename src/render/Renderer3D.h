#pragma once

#include "Shader.h"
#include "../scene/Scene.h"
#include <glm/glm.hpp>
#include <vector>
#include "../include/Camera.hpp"
#include "DynamicEnvironmentMapping.h"

// Directional light
struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

// Point light
struct PointLight {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

// Spot light
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

const int MAX_POINT_LIGHTS = 20;

class Renderer3D {
public:
    Renderer3D(); // Added constructor declaration
    void init();
    void render(Scene& scene, Camera& camera);
    void setLightingUniforms(Shader &shader, Camera& camera);

private:
    void setupGround();
    void renderGround(Shader &shader);

public:
    DirLight dirLight;
    PointLight pointLight;
    SpotLight spotLight;
    float shininess;
    bool useNormalMap;
    bool useSpecularMap;
    bool useDetailMap;
    bool useScatterMap;
    bool useCelShading;
    float pointLightBrightness;
    float dirLightBrightness;
    float spotLightBrightness;
    std::vector<PointLight> randomPointLights;
    bool useRandomPointLights;

    // Reflection properties
    unsigned int skyboxTexture = 0;
    float modelReflectivity = 0.3f;  // Default to 0.3 (partial reflection)
    bool useModelReflection = true;  // Whether to enable reflections for models (default on)

    // Refraction properties
    bool useModelRefraction = false;  // Whether to enable refractions for models (default off)
    float modelRefractionRatio = 0.66f;  // Ratio of air to material (1.00 / 1.52 for glass ≈ 0.66)

    // Dynamic environment mapping properties
    std::unique_ptr<DynamicEnvironmentMapping> dynamicEnvMapping;
    bool useDynamicEnvironmentMapping = false;  // Whether to enable dynamic environment mapping (default off)
    int maxReflectionProbes = 10;  // Maximum number of reflection probes to use
    float reflectionUpdateInterval = 0.5f;  // Time interval (in seconds) between reflection updates

    void renderWithCustomView(Scene& scene, Camera& camera,
        const glm::mat4& customView,
        const glm::mat4& projection);
private:
    unsigned int groundVAO;
    unsigned int groundVBO;
    unsigned int groundTexture;
    unsigned int groundNormalTexture;
};