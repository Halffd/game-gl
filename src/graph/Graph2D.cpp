#include "graph/Graph2D.h"
#include "asset/ResourceManager.h"
#include "include_libs/tinyexpr/tinyexpr.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ui/Gui.h"
#include "imgui.h"

Graph2D::Graph2D(const std::string& name) : Graph(name) {
    strncpy(equationBuffer, name.c_str(), sizeof(equationBuffer));
    ResourceManager::LoadShader("shaders/line.vs", "shaders/line.fs", "line");
    lineShader = ResourceManager::GetShader("line");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Graph2D::generateMesh() {
    double x;
    te_variable vars[] = {{"x", &x}};

    int err;
    te_expr *expr = te_compile(equationBuffer, vars, 1, &err);

    if (expr) {
        lineVertices.clear();
        for (float i = -10.0f; i <= 10.0f; i += 0.1f) {
            x = i;
            float y = te_eval(expr);
            lineVertices.push_back(glm::vec2(i, y));
        }
        te_free(expr);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec2), lineVertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        meshGenerated = true;
    }
}

void Graph2D::update(GLFWwindow* window, float deltaTime) {
    // Nothing to do here for now
}

void Graph2D::render() {
    if (!meshGenerated) {
        generateMesh();
    }

    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    lineShader.Use();
    lineShader.SetMatrix4("projection", projection);
    lineShader.SetMatrix4("view", view);
    lineShader.SetVector3f("color", settings.color);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, lineVertices.size());
    glBindVertexArray(0);

    Gui::Start();
    ImGui::Begin("Graph2D");
    ImGui::InputText("Equation", equationBuffer, sizeof(equationBuffer));
    if (ImGui::Button("Generate")) {
        functionName = equationBuffer;
        generateMesh();
    }
    ImGui::End();
    Gui::Render();
}
