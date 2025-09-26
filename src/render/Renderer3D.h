#pragma once

#include "Shader.h"
#include "../scene/Scene.h"
#include <glm/glm.hpp>
#include <vector>
#include "../include/Camera.hpp"

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

    void renderWithCustomView(Scene& scene, Camera& camera, 
        const glm::mat4& customView, 
        const glm::mat4& projection);
private:
    unsigned int groundVAO;
    unsigned int groundVBO;
    unsigned int groundTexture;
    unsigned int groundNormalTexture;
};