#pragma once

#include "Shader.h"
#include "../asset/Cubemap.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Skybox {
public:
    unsigned int VAO, VBO;
    
    Shader* skyboxShader;
    Cubemap* cubemap;

    Skybox() : skyboxShader(nullptr), cubemap(nullptr) {
        setup();
    }

    ~Skybox() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        delete skyboxShader;
        delete cubemap; // Note: Cubemap might be shared, so this should be used carefully
    }

    void setup() {
        // Initialize the skybox shader
        skyboxShader = new Shader();
        skyboxShader->Compile("shaders/skybox.vs", "shaders/skybox.fs", nullptr);

        // Define the vertices for a cube
        float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        // Create and configure the VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    void render(const glm::mat4& view, const glm::mat4& projection) {
        if (!skyboxShader || !cubemap) {
            return;
        }

        // Disable depth writing
        glDepthMask(GL_FALSE);

        // Use the skybox shader
        skyboxShader->Use();

        // Remove translation from the view matrix
        glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));

        // Set uniforms
        skyboxShader->SetMatrix4("view", viewWithoutTranslation);
        skyboxShader->SetMatrix4("projection", projection);

        // Bind the cubemap texture
        glActiveTexture(GL_TEXTURE0);
        cubemap->Bind();
        skyboxShader->SetInteger("skybox", 0);

        // Render the skybox cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Re-enable depth writing
        glDepthMask(GL_TRUE);
    }
};