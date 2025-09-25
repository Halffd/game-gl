#include "../../include_libs/tinyexpr/tinyexpr.h"
#include "graph/Graph2D.h"
#include "asset/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ui/Gui.h"
#include "imgui.h"
#include <cmath>
#include <cfloat>
#include <cstring>

Graph2D::Graph2D(const std::string& name) : Graph(name), inputType(CARTESIAN_Y_EQ_FX) {
    strncpy(equationBuffer, name.c_str(), sizeof(equationBuffer));
    ResourceManager::LoadShader("line_heatmap.vs", "line_heatmap.fs", "line");
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
    lineVertices.clear();
    clearMesh();

    double var1;
    te_variable vars[2];
    int var_count = 0;

    switch (inputType) {
        case CARTESIAN_Y_EQ_FX:
            vars[0] = {"x", &var1, TE_VARIABLE, 0}; // Initialize type
            var_count = 1;
            break;
        case CARTESIAN_X_EQ_FY:
            vars[0] = {"y", &var1, TE_VARIABLE, 0}; // Initialize type
            var_count = 1;
            break;
        case POLAR_R_EQ_FTHETA:
            vars[0] = {"theta", &var1, TE_VARIABLE, 0}; // Initialize type
            var_count = 1;
            break;
    }

    int err;
    te_expr *expr = te_compile(equationBuffer, vars, var_count, &err);

    if (expr) {
        yMin = FLT_MAX;
        yMax = FLT_MIN;

        switch (inputType) {
            case CARTESIAN_Y_EQ_FX:
                for (float x_val = -10.0f; x_val <= 10.0f; x_val += 0.1f) {
                    var1 = x_val;
                    float y_val = te_eval(expr);
                    lineVertices.push_back(glm::vec2(x_val, y_val));
                    if (y_val < yMin) yMin = y_val;
                    if (y_val > yMax) yMax = y_val;
                }
                break;
            case CARTESIAN_X_EQ_FY:
                for (float y_val = -10.0f; y_val <= 10.0f; y_val += 0.1f) {
                    var1 = y_val;
                    float x_val = te_eval(expr);
                    lineVertices.push_back(glm::vec2(x_val, y_val));
                    if (y_val < yMin) yMin = y_val;
                    if (y_val > yMax) yMax = y_val;
                }
                break;
            case POLAR_R_EQ_FTHETA:
                for (float theta_val = 0.0f; theta_val <= 2 * M_PI; theta_val += 0.01f) {
                    var1 = theta_val;
                    float r_val = te_eval(expr);
                    float x_val = r_val * cos(theta_val);
                    float y_val = r_val * sin(theta_val);
                    lineVertices.push_back(glm::vec2(x_val, y_val));
                    if (y_val < yMin) yMin = y_val;
                    if (y_val > yMax) yMax = y_val;
                }
                break;
        }
        te_free(expr);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec2), lineVertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        meshGenerated = true;
    }
}

void Graph2D::update(GLFWwindow* window, float deltaTime) {
    (void)window;
    (void)deltaTime;
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
    lineShader.SetFloat("yMin", yMin);
    lineShader.SetFloat("yMax", yMax);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, lineVertices.size());
    glBindVertexArray(0);

    ImGui::Begin("Graph2D");
    ImGui::InputText("Equation", equationBuffer, sizeof(equationBuffer));
    ImGui::RadioButton("y = f(x)", (int*)&inputType, CARTESIAN_Y_EQ_FX);
    ImGui::SameLine();
    ImGui::RadioButton("x = f(y)", (int*)&inputType, CARTESIAN_X_EQ_FY);
    ImGui::SameLine();
    ImGui::RadioButton("r = f(theta)", (int*)&inputType, POLAR_R_EQ_FTHETA);
    if (ImGui::Button("Generate")) {
        functionName = equationBuffer;
        generateMesh();
    }
    ImGui::End();
    Gui::Render();
}