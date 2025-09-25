#pragma once

#include "graph/Graph.h"
#include "render/Shader.h"
#include "../include/Camera.hpp"
#include <glm/glm.hpp>
#include <vector>

class VectorField : public Graph {
public:
    VectorField(const std::string& name);
    ~VectorField() override = default;

    void generateMesh() override;
    void update(GLFWwindow* window, float deltaTime) override;
    void render() override;

private:
    Shader vectorShader;
    unsigned int vao = 0, vbo = 0;
    Camera camera;
    char equationBufferX[256];
    char equationBufferY[256];
    char equationBufferZ[256];
    std::vector<float> arrowVertices;
};
