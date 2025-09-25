#pragma once

#include "graph/Graph.h"
#include "render/Shader.h"
#include "../include/Camera.hpp"
#include <glm/glm.hpp>
#include <vector>

enum Graph3DInputType {
    CARTESIAN_Z_EQ_FXY,
    SPHERICAL_R_EQ_FTHETAPHI
};

class Graph3D : public Graph {
public:
    Graph3D(const std::string& name);
    ~Graph3D() override = default;

    void generateMesh() override;
    void update(GLFWwindow* window, float deltaTime) override;
    void render() override;

private:
    Shader surfaceShader;
    unsigned int vao = 0, vbo = 0, ebo = 0;
    Camera camera;
    char equationBuffer[256];
    float zMin = 0.0f;
    float zMax = 0.0f;
    Graph3DInputType inputType;
};
