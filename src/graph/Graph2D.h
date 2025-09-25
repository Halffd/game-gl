#pragma once

#include "graph/Graph.h"
#include "render/Shader.h"
#include <glm/glm.hpp>
#include <vector>

enum Graph2DInputType {
    CARTESIAN_Y_EQ_FX,
    CARTESIAN_X_EQ_FY,
    POLAR_R_EQ_FTHETA
};

class Graph2D : public Graph {
public:
    Graph2D(const std::string& name);
    ~Graph2D() override = default;

    void generateMesh() override;
    void update(GLFWwindow* window, float deltaTime) override;
    void render() override;

private:
    Shader lineShader;
    unsigned int vao = 0, vbo = 0;
    std::vector<glm::vec2> lineVertices;
    char equationBuffer[256];
    Graph2DInputType inputType;
    float yMin = 0.0f;
    float yMax = 0.0f;
};
