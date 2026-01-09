#include "Renderer3D.h"
#include <string>
#include "asset/ResourceManager.h"
#include <glad/glad.h>
#include "../ConfigManager.hpp"
#include <memory>
#include "EnhancedVertexBuffer.h"

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

Renderer3D::Renderer3D()
    : dirLight {
        glm::vec3(-0.5f, -1.0f, -0.3f), // direction - adjusted for better angle
        glm::vec3(0.63f, 0.63f, 0.63f), // ambient - increased from 0.4 to 0.5
        glm::vec3(1.0f, 1.0f, 1.0f), // diffuse
        glm::vec3(1.0f, 1.0f, 1.0f), // specular
        true // enabled
      },
      pointLight {
        glm::vec3(0.0f, 15.0f, 0.0f), // position - moved to center and higher
        1.0f, // constant
        0.07f, // linear - slightly reduced for less attenuation
        0.017f, // quadratic - slightly reduced for less attenuation
        glm::vec3(0.3f, 0.3f, 0.3f), // ambient - increased from 0.2 to 0.3
        glm::vec3(1.0f, 1.0f, 1.0f), // diffuse
        glm::vec3(1.0f, 1.0f, 1.0f), // specular
        true // enabled
      },
      spotLight {
        glm::vec3(0.0f, 0.0f, 0.0f), // position - will be updated with camera position
        glm::vec3(0.0f, 0.0f, -1.0f), // direction - will be updated with camera direction
        glm::cos(glm::radians(12.5f)), // cutOff - inner cone (12.5 degrees)
        glm::cos(glm::radians(17.5f)), // outerCutOff - outer cone (17.5 degrees)
        1.0f, // constant
        0.09f, // linear
        0.032f, // quadratic
        glm::vec3(0.0f, 0.0f, 0.0f), // ambient - keep dark
        glm::vec3(1.0f, 1.0f, 1.0f), // diffuse - bright white
        glm::vec3(1.0f, 1.0f, 1.0f), // specular - bright white
        true // enabled
      },
      shininess(32.0f),
      useNormalMap(true),
      useSpecularMap(true),
      useDetailMap(false),
      useScatterMap(false),
      useCelShading(false),
      pointLightBrightness(5.0f),
      dirLightBrightness(0.5f),
      spotLightBrightness(0.7f),
      useRandomPointLights(false),
      groundVAO(0),
      groundVBO(0),
      groundTexture(0),
      groundNormalTexture(0)
{
    // Load reflection and refraction settings from config
    useModelReflection = game::cfg().GetUseReflection();
    useModelRefraction = game::cfg().GetUseRefraction();
    modelReflectivity = game::cfg().GetReflectionIntensity();
    modelRefractionRatio = game::cfg().GetRefractionRatio();

    // Initialize dynamic environment mapping
    dynamicEnvMapping = std::make_unique<DynamicEnvironmentMapping>();
    dynamicEnvMapping->initialize();

    // Initialize ground buffer
    groundBuffer = nullptr;
}

void Renderer3D::init() {
    setupGround();
}

Renderer3D::~Renderer3D() {
    // Clean up dynamic environment mapping
    if (dynamicEnvMapping) {
        dynamicEnvMapping->cleanup();
    }

    // Clean up ground buffer
    groundBuffer.reset();
}
void Renderer3D::renderWithCustomView(Scene& scene, Camera& camera, 
    const glm::mat4& customView, 
    const glm::mat4& projection) {

    // Store original camera position
    glm::vec3 originalPosition = camera.Position;
    glm::vec3 originalFront = camera.Front;

    // Extract position and direction from custom view
    glm::mat4 invCustomView = glm::inverse(customView);
    glm::vec3 customCameraPos = glm::vec3(invCustomView[3]);
    glm::vec3 customCameraFront = -glm::vec3(customView[2]);

    // Temporarily modify camera for lighting calculations
    camera.Position = customCameraPos;
    camera.Front = customCameraFront;

    // Use your existing render method but override matrices
    Shader &shader = ResourceManager::GetShader("model");
    shader.Use();
    shader.SetMatrix4("view", customView);
    shader.SetMatrix4("projection", projection);

    setLightingUniforms(shader, camera);

    // Render ground and scene
    glStencilMask(0x00);
    renderGround(shader);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    scene.draw(shader);

    // Restore original camera state
    camera.Position = originalPosition;
    camera.Front = originalFront;

    // Reset OpenGL state
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
void Renderer3D::render(Scene& scene, Camera& camera) {
    // Handle dynamic environment mapping - render scene from each probe's perspective
    if (useDynamicEnvironmentMapping && dynamicEnvMapping) {
        // Update probes that need updating
        dynamicEnvMapping->updateProbes();

        // Render the scene from each probe's perspective
        dynamicEnvMapping->renderAllProbes([&](const glm::mat4& projection, const glm::mat4& view) {
            // Configure shader for rendering
            Shader &defaultShader = ResourceManager::GetShader("model");

            // Set camera uniforms
            defaultShader.Use();
            defaultShader.SetMatrix4("projection", projection);
            defaultShader.SetMatrix4("view", view);

            // Set lighting uniforms without dynamic environment mapping to avoid recursion
            bool tempUseDynamic = useDynamicEnvironmentMapping;
            useDynamicEnvironmentMapping = false;
            setLightingUniforms(defaultShader, camera);
            useDynamicEnvironmentMapping = tempUseDynamic;

            // Render the ground
            renderGround(defaultShader);

            // Render all objects in the scene
            for (auto& object : scene.getObjects()) {
                Shader* customShader = object->getShader();
                Shader& activeShader = customShader ? *customShader : defaultShader;

                activeShader.Use();
                activeShader.SetMatrix4("projection", projection);
                activeShader.SetMatrix4("view", view);

                // Temporarily disable dynamic environment mapping to avoid recursion
                useDynamicEnvironmentMapping = false;
                setLightingUniforms(activeShader, camera);
                useDynamicEnvironmentMapping = tempUseDynamic;

                object->Draw(activeShader);
            }

            for (auto& entity : scene.getEntities()) {
                for (auto& component : entity->getComponents()) {
                    defaultShader.Use();
                    defaultShader.SetMatrix4("projection", projection);
                    defaultShader.SetMatrix4("view", view);

                    // Temporarily disable dynamic environment mapping to avoid recursion
                    useDynamicEnvironmentMapping = false;
                    setLightingUniforms(defaultShader, camera);
                    useDynamicEnvironmentMapping = tempUseDynamic;

                    component->draw(defaultShader);
                }
            }
        });
    }

    // Configure shader for rendering
    Shader &defaultShader = ResourceManager::GetShader("model");

    // Set camera uniforms
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                            0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // PHASE 1: Render regular objects and mark them in stencil buffer
    glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor
    // Render the ground
    defaultShader.Use();
    defaultShader.SetMatrix4("projection", projection);
    defaultShader.SetMatrix4("view", view);
    setLightingUniforms(defaultShader, camera);
    renderGround(defaultShader);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    for (auto& object : scene.getObjects()) {
        Shader* customShader = object->getShader();
        Shader& activeShader = customShader ? *customShader : defaultShader;

        activeShader.Use();
        activeShader.SetMatrix4("projection", projection);
        activeShader.SetMatrix4("view", view);
        setLightingUniforms(activeShader, camera);

        object->Draw(activeShader);
    }

    for (auto& entity : scene.getEntities()) {
        for (auto& component : entity->getComponents()) {
            defaultShader.Use();
            defaultShader.SetMatrix4("projection", projection);
            defaultShader.SetMatrix4("view", view);
            setLightingUniforms(defaultShader, camera);
            component->draw(defaultShader);
        }
    }

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    // Get the outline shader
    Shader &outlineShader = ResourceManager::GetShader("outline");

    // Explicitly activate the shader before rendering
    outlineShader.Use();
    outlineShader.SetMatrix4("view", camera.GetViewMatrix());
    outlineShader.SetMatrix4("projection", projection);
    // Scale factor for outlines (slightly larger than original)
    const float outlineScale = 1.03f; // 5% larger

    // Render model outlines
    for (auto& object : scene.getObjects()) {
        // We need to set the model matrix for the outline shader
        outlineShader.SetMatrix4("model", object->GetModelMatrix());
        object->Draw(outlineShader);
    }

    // restore state
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

// Function to set lighting uniforms
void Renderer3D::setLightingUniforms(Shader &shader, Camera& camera) {
    // Set material properties
    shader.SetFloat("shininess", shininess);
    shader.SetInteger("useNormalMap", useNormalMap ? 1 : 0);
    shader.SetInteger("useSpecularMap", useSpecularMap ? 1 : 0);
    shader.SetInteger("useDetailMap", useDetailMap ? 1 : 0);
    shader.SetInteger("useScatterMap", useScatterMap ? 1 : 0);
    shader.SetInteger("useCelShading", useCelShading ? 1 : 0);

    // Set light brightness adjustment uniforms
    shader.SetFloat("pointLightBrightness", pointLightBrightness);
    shader.SetFloat("dirLightBrightness", dirLightBrightness);
    shader.SetFloat("spotLightBrightness", spotLightBrightness);

    // Set camera position for lighting calculations
    shader.SetVector3f("viewPos", camera.Position);

    // Update spotlight position and direction to match camera
    spotLight.position = camera.Position;
    spotLight.direction = camera.Front;

    // Set directional light properties
    shader.SetVector3f("dirLight.direction", dirLight.direction);
    shader.SetVector3f("dirLight.ambient", dirLight.ambient);
    shader.SetVector3f("dirLight.diffuse", dirLight.diffuse);
    shader.SetVector3f("dirLight.specular", dirLight.specular);
    shader.SetInteger("useDirLight", dirLight.enabled ? 1 : 0);

    // Set main point light properties
    shader.SetVector3f("pointLight.position", pointLight.position);
    shader.SetFloat("pointLight.constant", pointLight.constant);
    shader.SetFloat("pointLight.linear", pointLight.linear);
    shader.SetFloat("pointLight.quadratic", pointLight.quadratic);
    shader.SetVector3f("pointLight.ambient", pointLight.ambient);
    shader.SetVector3f("pointLight.diffuse", pointLight.diffuse);
    shader.SetVector3f("pointLight.specular", pointLight.specular);
    shader.SetInteger("usePointLight", pointLight.enabled ? 1 : 0);

    // Set spotlight position and direction for tangent space calculations
    shader.SetVector3f("spotLightPos", spotLight.position);
    shader.SetVector3f("spotLightDir", spotLight.direction);

    // Set spotlight uniforms (add these back)
    shader.SetVector3f("spotLight.position", spotLight.position);
    shader.SetVector3f("spotLight.direction", spotLight.direction);
    shader.SetFloat("spotLight.cutOff", spotLight.cutOff);
    shader.SetFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    shader.SetFloat("spotLight.constant", spotLight.constant);
    shader.SetFloat("spotLight.linear", spotLight.linear);
    shader.SetFloat("spotLight.quadratic", spotLight.quadratic);
    shader.SetVector3f("spotLight.ambient", spotLight.ambient);
    shader.SetVector3f("spotLight.diffuse", spotLight.diffuse);
    shader.SetVector3f("spotLight.specular", spotLight.specular);
    shader.SetInteger("useSpotLight", spotLight.enabled ? 1 : 0);

    // Set random point lights
    shader.SetInteger("numRandomPointLights", static_cast<int>(randomPointLights.size()));
    shader.SetInteger("useRandomPointLights", useRandomPointLights ? 1 : 0);

    for (size_t i = 0; i < randomPointLights.size() && i < MAX_POINT_LIGHTS; i++) {
        std::string index = std::to_string(i);
        shader.SetVector3f(("randomPointLights[" + index + "].position").c_str(), randomPointLights[i].position);
        shader.SetFloat(("randomPointLights[" + index + "].constant").c_str(), randomPointLights[i].constant);
        shader.SetFloat(("randomPointLights[" + index + "].linear").c_str(), randomPointLights[i].linear);
        shader.SetFloat(("randomPointLights[" + index + "].quadratic").c_str(), randomPointLights[i].quadratic);
        shader.SetVector3f(("randomPointLights[" + index + "].ambient").c_str(), randomPointLights[i].ambient);
        shader.SetVector3f(("randomPointLights[" + index + "].diffuse").c_str(), randomPointLights[i].diffuse);
        shader.SetVector3f(("randomPointLights[" + index + "].specular").c_str(), randomPointLights[i].specular);
    }

    // Optionally adjust material properties for reflective models in models scene
    if (useModelReflection) {
        // Increase shininess for more mirror-like reflections
        // Check if the uniform exists before setting it to prevent GL_INVALID_OPERATION
        GLint shininessLoc = glGetUniformLocation(shader.ID, "shininess");
        if (shininessLoc != -1) {
            shader.SetFloat("shininess", 128.0f);  // Higher shininess = sharper reflections
        }

        // Only set reflection uniforms if they exist in this shader
        GLint skyboxLoc = glGetUniformLocation(shader.ID, "skybox");
        GLint reflectivityLoc = glGetUniformLocation(shader.ID, "reflectivity");
        GLint useReflectionLoc = glGetUniformLocation(shader.ID, "useReflection");

        if (skyboxLoc != -1 && reflectivityLoc != -1 && useReflectionLoc != -1) {
            glUniform1i(useReflectionLoc, 1);
            glUniform1f(reflectivityLoc, modelReflectivity);

            // Bind skybox to a texture unit (e.g., unit 5 to avoid conflicts)
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glUniform1i(skyboxLoc, 5);
        }
    } else {
        // Only set reflection uniforms if they exist in this shader
        GLint skyboxLoc = glGetUniformLocation(shader.ID, "skybox");
        GLint useReflectionLoc = glGetUniformLocation(shader.ID, "useReflection");
        GLint reflectivityLoc = glGetUniformLocation(shader.ID, "reflectivity");

        if (useReflectionLoc != -1 && reflectivityLoc != -1) {
            glUniform1i(useReflectionLoc, 0);
            glUniform1f(reflectivityLoc, 0.0f);
        }
    }

    // Set refraction uniforms if they exist in this shader
    GLint useRefractionLoc = glGetUniformLocation(shader.ID, "useRefraction");
    GLint refractionRatioLoc = glGetUniformLocation(shader.ID, "refractionRatio");

    if (useRefractionLoc != -1) {
        glUniform1i(useRefractionLoc, useModelRefraction ? 1 : 0);
    }
    if (refractionRatioLoc != -1) {
        glUniform1f(refractionRatioLoc, modelRefractionRatio);
    }

    // Dynamic environment mapping
    if (useDynamicEnvironmentMapping) {
        // Find the closest reflection probe to this object
        int closestProbe = dynamicEnvMapping->getClosestProbe(camera.Position);
        if (closestProbe != -1) {
            unsigned int probeCubemap = dynamicEnvMapping->getProbeCubemap(closestProbe);

            // Bind the dynamic cubemap to texture unit 6
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, probeCubemap);

            // Set the dynamic environment map uniform if it exists
            GLint dynamicEnvMapLoc = glGetUniformLocation(shader.ID, "dynamicEnvironmentMap");
            if (dynamicEnvMapLoc != -1) {
                glUniform1i(dynamicEnvMapLoc, 6); // Texture unit 6
            }

            // Set the dynamic environment mapping flag
            GLint useDynamicEnvMapLoc = glGetUniformLocation(shader.ID, "useDynamicEnvironmentMap");
            if (useDynamicEnvMapLoc != -1) {
                glUniform1i(useDynamicEnvMapLoc, 1);
            }
        }
    } else {
        // Disable dynamic environment mapping
        GLint useDynamicEnvMapLoc = glGetUniformLocation(shader.ID, "useDynamicEnvironmentMap");
        if (useDynamicEnvMapLoc != -1) {
            glUniform1i(useDynamicEnvMapLoc, 0);
        }
    }
}

void Renderer3D::setupGround() {
    // Create a large ground plane - lowered by 50% (y = -0.5f instead of 0.0f)
    std::vector<VO::VertexData> groundVertices = {
        // Triangle 1
        {{-50.0f, -0.5f, -50.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{50.0f, -0.5f, -50.0f}, {0.0f, 1.0f, 0.0f}, {50.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{50.0f, -0.5f, 50.0f}, {0.0f, 1.0f, 0.0f}, {50.0f, 50.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

        // Triangle 2
        {{-50.0f, -0.5f, -50.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{50.0f, -0.5f, 50.0f}, {0.0f, 1.0f, 0.0f}, {50.0f, 50.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-50.0f, -0.5f, 50.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 50.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    // Use enhanced vertex buffer with interleaved strategy
    groundBuffer = std::make_unique<VO::EnhancedVertexBuffer>();
    std::vector<unsigned int> indices = {0, 1, 2, 3, 4, 5}; // Simple index buffer
    groundBuffer->initializeInterleaved(groundVertices, indices);

    // Create a larger, more detailed ground texture (4x4 grid instead of 2x2)
    unsigned char groundTextureData[] = {
        // Row 1
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,

        // Row 2
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,

        // Row 3
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        40, 60, 120, 255, 50, 70, 130, 255, 50, 70, 130, 255, 40, 60, 120, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,

        // Row 4
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        30, 50, 110, 255, 40, 60, 120, 255, 40, 60, 120, 255, 30, 50, 110, 255,
        20, 40, 100, 255, 30, 50, 110, 255, 30, 50, 110, 255, 20, 40, 100, 255
    };

    // Generate and bind texture
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use linear filtering

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundTextureData);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps to prevent flickering at distance

    // Create a normal map for the ground
    unsigned char normalMapData[16 * 16 * 4];
    // Fill with flat normals (128, 128, 255, 255)
    for (int i = 0; i < 16 * 16; i++) {
        normalMapData[i * 4 + 0] = 128;
        normalMapData[i * 4 + 1] = 128;
        normalMapData[i * 4 + 2] = 255;
        normalMapData[i * 4 + 3] = 255;
    }

    // Generate and bind normal texture
    glGenTextures(1, &groundNormalTexture);
    glBindTexture(GL_TEXTURE_2D, groundNormalTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload normal map data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalMapData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer3D::renderGround(Shader &shader) {
    // Set model matrix for ground
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);

    // Bind ground diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    shader.SetInteger("texture_diffuse1", 0);

    // Bind ground normal texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, groundNormalTexture);
    shader.SetInteger("texture_normal1", 1);

    // Draw ground using enhanced buffer
    if (groundBuffer) {
        groundBuffer->bind();
        groundBuffer->draw();
        groundBuffer->unbind();
    } else {
        // Fallback to legacy rendering
        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

// Add the missing closing brace for the render function
