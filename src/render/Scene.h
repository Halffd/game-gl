#ifndef SCENE_H
#define SCENE_H

#include "SceneObject.h"
#include "ModelObject.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Scene class to manage all scene objects
class Scene {
private:
    // Map of scene objects by name
    std::unordered_map<std::string, std::shared_ptr<SceneObject>> objects;
    
    // Map of models that can be reused
    std::unordered_map<std::string, std::shared_ptr<m3D::Model>> modelCache;
    
public:
    // Constructor
    Scene() {
        std::cout << "Scene created" << std::endl;
    }
    
    // Destructor
    ~Scene() {
        Clear();
    }
    
    // Add a scene object
    void AddObject(std::shared_ptr<SceneObject> object) {
        if (!object) return;
        
        objects[object->name] = object;
        std::cout << "Added object to scene: " << object->name << std::endl;
    }
    
    // Create and add a model object
    std::shared_ptr<ModelObject> CreateModelObject(const std::string& name, const std::string& modelPath, 
                                                  const glm::vec3& position = glm::vec3(0.0f),
                                                  const glm::vec3& rotation = glm::vec3(0.0f),
                                                  const glm::vec3& scale = glm::vec3(1.0f),
                                                  bool gamma = false) {
        try {
            // Check if we already have this model in the cache
            std::shared_ptr<m3D::Model> model;
            if (modelCache.find(modelPath) != modelCache.end()) {
                std::cout << "Using cached model for: " << modelPath << std::endl;
                model = modelCache[modelPath];
            } else {
                // Create a new model and add it to the cache
                model = std::make_shared<m3D::Model>(modelPath, gamma);
                modelCache[modelPath] = model;
                std::cout << "Added model to cache: " << modelPath << std::endl;
            }
            
            // Create the model object
            auto modelObject = std::make_shared<ModelObject>(name, model, position, rotation, scale);
            
            // Add it to the scene
            AddObject(modelObject);
            
            return modelObject;
        } catch (const std::exception& e) {
            std::cout << "Failed to create model object: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Get an object by name
    std::shared_ptr<SceneObject> GetObject(const std::string& name) {
        auto it = objects.find(name);
        if (it != objects.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Get a model object by name
    std::shared_ptr<ModelObject> GetModelObject(const std::string& name) {
        auto obj = GetObject(name);
        if (obj) {
            return std::dynamic_pointer_cast<ModelObject>(obj);
        }
        return nullptr;
    }
    
    // Remove an object by name
    void RemoveObject(const std::string& name) {
        objects.erase(name);
    }
    
    // Draw all visible objects
    void Draw(Shader& shader) {
        for (auto& pair : objects) {
            if (pair.second && pair.second->visible) {
                pair.second->Draw(shader);
            }
        }
    }
    
    // Clear all objects
    void Clear() {
        objects.clear();
        modelCache.clear();
        std::cout << "Scene cleared" << std::endl;
    }
    
    // Get all object names
    std::vector<std::string> GetObjectNames() const {
        std::vector<std::string> names;
        for (const auto& pair : objects) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get count of objects
    size_t GetObjectCount() const {
        return objects.size();
    }
};

#endif // SCENE_H 