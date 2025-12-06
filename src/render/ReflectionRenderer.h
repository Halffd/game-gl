#pragma once
#include "render/Shader.h"
#include "render/Model.h"
#include "asset/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <vector>
#include <string>

class ReflectionRenderer {
private:
    Shader* reflectionShader;
    unsigned int cubeVAO, cubeVBO;
    std::vector<std::string> modelNames;
    int currentModelIndex;
    m3D::Model* currentModel;
    bool useCustomModel;

public:
    ReflectionRenderer() : currentModelIndex(0), currentModel(nullptr), useCustomModel(false) {
        // Load shader using ResourceManager
        ResourceManager::LoadShader("reflection.vs", "reflection.fs", "reflection");
        reflectionShader = &ResourceManager::GetShader("reflection");

        setupCube();
        setupDefaultModels();
    }

    void setupCube() {
        // Cube vertices with position and normal
        float cubeVertices[] = {
            // positions          // normals
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void setupDefaultModels() {
        modelNames = {"backpack", "cube", "sphere"};
        currentModelIndex = 0; // Default to backpack
    }

    void setModel(const std::string& modelName) {
        // Try to load the model, default to cube if not found
        try {
            std::string modelPath = "models/" + modelName + "/scene.gltf";
            if (!ResourceManager::GetTexture2D(modelPath).status) { // Check if model exists
                // If specific model doesn't exist, revert to cube
                currentModelIndex = 1; // cube
                useCustomModel = false;
            } else {
                // For now, we'll handle custom models later
                useCustomModel = true;
            }
        } catch (...) {
            currentModelIndex = 1; // cube
            useCustomModel = false;
        }
    }

    void setModelByIndex(int index) {
        if (index >= 0 && index < modelNames.size()) {
            currentModelIndex = index;
            useCustomModel = false; // For now, use primitives for simplicity
        }
    }

    std::string getCurrentModelName() const {
        if (currentModelIndex >= 0 && currentModelIndex < modelNames.size()) {
            return modelNames[currentModelIndex];
        }
        return "cube";
    }

    int getCurrentModelIndex() const {
        return currentModelIndex;
    }

    int getModelCount() const {
        return modelNames.size();
    }

    std::string getModelName(int index) const {
        if (index >= 0 && index < modelNames.size()) {
            return modelNames[index];
        }
        return "";
    }

    void renderReflection(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                         const glm::vec3& cameraPos, unsigned int skyboxTexture) {
        reflectionShader->Use();
        reflectionShader->SetMatrix4("model", model);
        reflectionShader->SetMatrix4("view", view);
        reflectionShader->SetMatrix4("projection", projection);
        reflectionShader->SetVector3f("cameraPos", cameraPos);

        // Render based on selected model type
        switch(currentModelIndex) {
            case 0: // backpack (default cube for now)
            case 1: // cube
            default:
                glBindVertexArray(cubeVAO);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                break;
            case 2: // sphere
                // Render a sphere with reflection
                renderSphereReflection(model, view, projection, cameraPos, skyboxTexture);
                break;
        }
    }

private:
    void renderSphereReflection(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                               const glm::vec3& cameraPos, unsigned int skyboxTexture) {
        // For now, just use a simple sphere implementation
        // In a real implementation, you'd have a sphere VAO/VBO
        glBindVertexArray(cubeVAO); // Use cube VAO as placeholder
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
};