#pragma once

class Entity;
class Shader;

class Component {
public:
    virtual ~Component() = default;
    Entity* entity;
    virtual void init() {}
    virtual void update(float dt) {}
    virtual void draw(Shader& shader) {}
};