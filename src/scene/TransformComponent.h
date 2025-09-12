#pragma once

#include "Component.h"
#include "Transform.h"

class TransformComponent : public Component {
public:
    Transform transform;
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : transform(pos, rot, scl) {}
};