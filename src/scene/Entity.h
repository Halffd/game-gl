#pragma once

#include <vector>
#include <memory>
#include <stdexcept> // Added for std::runtime_error
#include "Component.h"

class Entity {
public:
    template<typename T, typename... TArgs>
    T& addComponent(TArgs&&... args) {
        T* c(new T(std::forward<TArgs>(args)...));
        c->entity = this;
        std::unique_ptr<Component> uPtr{c};
        components.emplace_back(std::move(uPtr));
        return *c;
    }

    template<typename T>
    T& getComponent() {
        for (auto& c : components) {
            if (dynamic_cast<T*>(c.get())) {
                return *dynamic_cast<T*>(c.get());
            }
        }
        throw std::runtime_error("Component not found");
    }

    const std::vector<std::unique_ptr<Component>>& getComponents() const {
        return components;
    }

private:
    std::vector<std::unique_ptr<Component>> components;
};