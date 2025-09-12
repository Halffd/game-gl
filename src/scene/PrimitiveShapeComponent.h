#pragma once

#include "Component.h"
#include "../render/primitives/PrimitiveShape.h"

class PrimitiveShapeComponent : public Component {
public:
    std::shared_ptr<m3D::PrimitiveShape> shape;
    PrimitiveShapeComponent(std::shared_ptr<m3D::PrimitiveShape> shape) : shape(shape) {} 

    void draw(Shader& shader) override {
        shape->Draw(shader);
    }
};
