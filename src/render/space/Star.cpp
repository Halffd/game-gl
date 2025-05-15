#include "Star.h"
#include <glm/gtc/type_ptr.hpp>

Star::Star(float mass, float radius, float rotationPeriod, float axialTilt,
           float luminosity, float temperature)
    : CelestialBody(mass, radius, rotationPeriod, axialTilt),
      luminosity(luminosity), temperature(temperature) {
    // Calculate color based on temperature (blackbody radiation)
    color = CalculateColorFromTemperature(temperature);

    // Setup glow effect resources
    glowShader = nullptr;
    SetupGlowEffect();
}

Star::~Star() {
    // Clean up OpenGL resources
    if (glowVAO) {
        glDeleteVertexArrays(1, &glowVAO);
    }
    if (glowVBO) {
        glDeleteBuffers(1, &glowVBO);
    }

    // Don't delete glowShader here as it might be shared
}

void Star::SetupMaterial(Shader &shader) {
    // Set star material properties
    shader.SetVector3f("material.ambient", color * 0.2f);
    shader.SetVector3f("material.diffuse", color);
    shader.SetVector3f("material.specular", glm::vec3(1.0f));
    shader.SetFloat("material.shininess", 32.0f);

    // Stars are emissive
    shader.SetVector3f("material.emission", color * luminosity);
    shader.SetInteger("material.useEmission", 1);

    // Bind texture if available
    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.SetInteger("texture_diffuse1", 0);
    }
}

void Star::Draw(Shader &shader) {
    // Draw the star itself
    Draw(shader);

    // Add the glow effect
    if (glowShader) {
        RenderGlowEffect(shader);
    }
}

glm::vec3 Star::CalculateColorFromTemperature(float temp) {
    // Simple approximation of blackbody radiation color
    // Real implementation would use Planck's law

    // Temperature ranges and corresponding RGB values (very simplified)
    if (temp < 3500) {
        // Red stars
        return glm::vec3(1.0f, 0.5f, 0.0f);
    } else if (temp < 5000) {
        // Yellow stars
        return glm::vec3(1.0f, 0.8f, 0.4f);
    } else if (temp < 6000) {
        // White-yellow stars
        return glm::vec3(1.0f, 0.9f, 0.7f);
    } else if (temp < 7500) {
        // White stars
        return glm::vec3(1.0f, 1.0f, 1.0f);
    } else if (temp < 10000) {
        // Blue-white stars
        return glm::vec3(0.8f, 0.9f, 1.0f);
    } else {
        // Blue stars
        return glm::vec3(0.6f, 0.7f, 1.0f);
    }
}

void Star::SetupGlowEffect() {
    // Create geometry for a quad that will be used for the glow effect
    // This would be a billboard quad that always faces the camera

    // Placeholder code - in a real implementation this would set up
    // the glow effect with proper geometry and shader

    // Example vertex data for a simple quad
    float glowVertices[] = {
        // positions         // texture coords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &glowVAO);
    glGenBuffers(1, &glowVBO);

    glBindVertexArray(glowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glowVertices), glowVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coords attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    // Load glow shader (would be implemented elsewhere in your application)
    // glowShader = new Shader("shaders/glow.vs", "shaders/glow.fs");
}

void Star::RenderGlowEffect(Shader &mainShader) {
    // This would render a billboarded glow quad that follows the star
    // Using additive blending to create the glow effect

    // In a real implementation, this would:
    // 1. Enable additive blending
    // 2. Use the glow shader
    // 3. Update the model matrix to face the camera (billboard)
    // 4. Draw the glow quad

    if (!glowShader) return;

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Use glow shader
    glowShader->Use();

    // Pass necessary uniforms (view, projection matrices would be needed)
    // This is a placeholder - you'd get these from your camera/scene
    //glowShader->SetMatrix4("projection", mainShader.GetMatrix4("projection"));
    //glowShader->SetMatrix4("view", mainShader.GetMatrix4("view"));

    // Create a billboard model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    // Scale based on star size and luminosity
    float glowSize = radius * (1.0f + 0.5f * luminosity);
    model = glm::scale(model, glm::vec3(glowSize));

    glowShader->SetMatrix4("model", model);

    // Set glow color based on star color
    glowShader->SetVector3f("glowColor", color);

    // Draw the glow quad
    glBindVertexArray(glowVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Reset blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}