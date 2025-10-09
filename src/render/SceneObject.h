#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

// Base class for all scene objects
class SceneObject {
public:
    // Object properties
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool visible;
    
    // Constructor
    SceneObject(const std::string& objectName = "Unnamed", 
                const glm::vec3& pos = glm::vec3(0.0f),
                const glm::vec3& rot = glm::vec3(0.0f),
                const glm::vec3& scl = glm::vec3(1.0f))
        : name(objectName), position(pos), rotation(rot), scale(scl), visible(true) {}
    
    // Virtual destructor
    virtual ~SceneObject() {}
    
    // Virtual draw function to be implemented by derived classes
    virtual void Draw(Shader& shader) = 0;

    virtual Shader* getShader() const { return nullptr; }
    
    // Get model matrix based on position, rotation, and scale
    glm::mat4 GetModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

#endif // SCENE_OBJECT_H 