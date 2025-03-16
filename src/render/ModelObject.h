#ifndef MODEL_OBJECT_H
#define MODEL_OBJECT_H

#include "SceneObject.h"
#include "Model.h"
#include <memory>

// ModelObject class that wraps a Model and inherits from SceneObject
class ModelObject : public SceneObject {
private:
    std::shared_ptr<m3D::Model> model;
    
public:
    // Constructor with model path
    ModelObject(const std::string& name, const std::string& modelPath, 
                const glm::vec3& position = glm::vec3(0.0f),
                const glm::vec3& rotation = glm::vec3(0.0f),
                const glm::vec3& scale = glm::vec3(1.0f),
                bool gamma = false)
        : SceneObject(name, position, rotation, scale) {
        try {
            model = std::make_shared<m3D::Model>(modelPath, gamma);
            std::cout << "ModelObject created: " << name << " from " << modelPath << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Failed to create ModelObject: " << e.what() << std::endl;
            throw;
        }
    }
    
    // Constructor with existing model
    ModelObject(const std::string& name, std::shared_ptr<m3D::Model> existingModel,
                const glm::vec3& position = glm::vec3(0.0f),
                const glm::vec3& rotation = glm::vec3(0.0f),
                const glm::vec3& scale = glm::vec3(1.0f))
        : SceneObject(name, position, rotation, scale), model(existingModel) {
        std::cout << "ModelObject created: " << name << " from existing model" << std::endl;
    }
    
    // Draw function implementation
    void Draw(Shader& shader) override {
        if (!visible || !model) return;
        
        // Set model matrix
        shader.SetMatrix4("model", GetModelMatrix());
        
        // Draw the model
        model->Draw(shader);
    }
    
    // Get the underlying model
    std::shared_ptr<m3D::Model> GetModel() const {
        return model;
    }
};

#endif // MODEL_OBJECT_H 