#pragma once

#include "Component.h"
#include "../render/Model.h"
#include "TransformComponent.h"
#include "../scene/Entity.h"

class ModelComponent : public Component {
public:
    m3D::Model* model;
    ModelComponent(m3D::Model* model) : model(model) {}

    void draw(Shader& shader) override {
        TransformComponent& transform = entity->getComponent<TransformComponent>();
        shader.SetMatrix4("model", transform.transform.GetModelMatrix());
        model->Draw(shader);
    }
};