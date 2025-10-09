#pragma once

#include <vector>
#include <memory>
#include "Entity.h"
#include "../render/SceneObject.h"

class Shader;

class Scene {
public:
    void addEntity(std::unique_ptr<Entity> entity);
    void AddObject(std::shared_ptr<SceneObject> object);
    void update(float dt);
    void draw(Shader& shader);

    std::vector<std::shared_ptr<SceneObject>>& getObjects();

private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::shared_ptr<SceneObject>> objects;
};