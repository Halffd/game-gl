#include "Scene.h"
#include "Component.h"
#include "../render/SceneObject.h"

void Scene::addEntity(std::unique_ptr<Entity> entity) {
    entities.emplace_back(std::move(entity));
}

void Scene::AddObject(std::shared_ptr<SceneObject> object) {
    objects.emplace_back(object);
}

void Scene::update(float dt) {
    for (auto& entity : entities) {
        for (auto& component : entity->getComponents()) {
            component->update(dt);
        }
    }
}

void Scene::draw(Shader& shader) {
    for (auto& entity : entities) {
        for (auto& component : entity->getComponents()) {
            component->draw(shader);
        }
    }
    for (auto& object : objects) {
        object->Draw(shader);
    }
}

std::vector<std::shared_ptr<SceneObject>>& Scene::getObjects() {
    return objects;
}

const std::vector<std::unique_ptr<Entity>>& Scene::getEntities() const {
    return entities;
}