#include "graph/Graph3D.h"
#include "../../include_libs/tinyexpr/tinyexpr.h"
#include "asset/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ui/Gui.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cfloat>
#include <cmath>

Graph3D::Graph3D(const std::string& name) : Graph(name), camera(glm::vec3(5.0f, 5.0f, 5.0f)), inputType(CARTESIAN_Z_EQ_FXY) {
    strncpy(equationBuffer, name.c_str(), sizeof(equationBuffer));
    ResourceManager::LoadShader("heatmap.vs", "heatmap.fs", std::string("surface"));
    surfaceShader = ResourceManager::GetShader("surface");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Graph3D::generateMesh() {
    clearMesh();

    double var1, var2;
    te_variable vars[3];
    int var_count = 0;

    switch (inputType) {
        case CARTESIAN_Z_EQ_FXY:
            vars[0] = {"x", &var1, TE_VARIABLE, 0};
            vars[1] = {"y", &var2, TE_VARIABLE, 0};
            var_count = 2;
            break;
        case SPHERICAL_R_EQ_FTHETAPHI:
            vars[0] = {"theta", &var1, TE_VARIABLE, 0};
            vars[1] = {"phi", &var2, TE_VARIABLE, 0};
            var_count = 2;
            break;
    }

    int err;
    te_expr *expr = te_compile(equationBuffer, vars, var_count, &err);

    if (expr) {
        vertices.clear();
        indices.clear();

        int grid_size = 50;
        float step = 0.2f;

        zMin = FLT_MAX;
        zMax = FLT_MIN;

        switch (inputType) {
            case CARTESIAN_Z_EQ_FXY: {
                for (int i = 0; i < grid_size; i++) {
                    for (int j = 0; j < grid_size; j++) {
                        var1 = (i - grid_size / 2) * step; // x
                        var2 = (j - grid_size / 2) * step; // y
                        float z = te_eval(expr);
                        vertices.push_back((i - grid_size / 2) * step);
                        vertices.push_back(z);
                        vertices.push_back((j - grid_size / 2) * step);

                        if (z < zMin) zMin = z;
                        if (z > zMax) zMax = z;
                    }
                }
                break;
            }
            case SPHERICAL_R_EQ_FTHETAPHI: {
                for (int i = 0; i < grid_size; i++) {
                    for (int j = 0; j < grid_size; j++) {
                        var1 = (float)i / (grid_size - 1) * 2 * M_PI; // theta from 0 to 2PI
                        var2 = (float)j / (grid_size - 1) * M_PI;   // phi from 0 to PI
                        float r = te_eval(expr);

                        float x_val = r * sin(var2) * cos(var1);
                        float y_val = r * cos(var2);
                        float z_val = r * sin(var2) * sin(var1);

                        vertices.push_back(x_val);
                        vertices.push_back(y_val);
                        vertices.push_back(z_val);

                        if (y_val < zMin) zMin = y_val;
                        if (y_val > zMax) zMax = y_val;
                    }
                }
                break;
            }
        }

        for (int i = 0; i < grid_size - 1; i++) {
            for (int j = 0; j < grid_size - 1; j++) {
                int top_left = i * grid_size + j;
                int top_right = top_left + 1;
                int bottom_left = (i + 1) * grid_size + j;
                int bottom_right = bottom_left + 1;

                indices.push_back(top_left);
                indices.push_back(bottom_left);
                indices.push_back(top_right);

                indices.push_back(top_right);
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
            }
        }

        te_free(expr);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
        glBindVertexArray(0);

        meshGenerated = true;
    }
}

void Graph3D::update(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_RIGHT, deltaTime);
}

void Graph3D::render() {
    if (!meshGenerated) {
        generateMesh();
    }

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    surfaceShader.Use();
    surfaceShader.SetMatrix4("projection", projection);
    surfaceShader.SetMatrix4("view", view);
    surfaceShader.SetMatrix4("model", model);
    surfaceShader.SetVector3f("objectColor", settings.color);
    surfaceShader.SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    surfaceShader.SetVector3f("viewPos", camera.Position);
    surfaceShader.SetFloat("zMin", zMin);
    surfaceShader.SetFloat("zMax", zMax);
    surfaceShader.SetFloat("zMin", zMin);
    surfaceShader.SetFloat("zMax", zMax);
    surfaceShader.SetFloat("zMin", zMin);
    surfaceShader.SetFloat("zMax", zMax);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    ImGui::Begin("Graph3D");
    ImGui::InputText("Equation", equationBuffer, sizeof(equationBuffer));
    ImGui::RadioButton("z = f(x,y)", (int*)&inputType, CARTESIAN_Z_EQ_FXY);
    ImGui::SameLine();
    ImGui::RadioButton("r = f(theta,phi)", (int*)&inputType, SPHERICAL_R_EQ_FTHETAPHI);
    if (ImGui::Button("Generate")) {
        functionName = equationBuffer;
        generateMesh();
    }
    ImGui::End();
    Gui::Render();
}

