#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

inline glm::mat4 transform(float x, float y, float z, float scale, float rotation, glm::vec3 color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::scale(model, glm::vec3(scale));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}

inline float clamp(float t, float (*func)(float), float min, float max, float period, float phase) {
    float value = func(t * period + phase);
    return glm::clamp(value, min, max);
}

inline float clampSin(float t, float (*func)(float), float min, float max) {
    return clamp(t, func, min, max, 1.0f, 0.0f);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline void printTransform(const glm::mat4& transform) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", transform[i][j]);
        }
        printf("\n");
    }
} 