#include "graph/VectorField.h"
#include "../../include_libs/tinyexpr/tinyexpr.h"
#include "asset/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ui/Gui.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cmath>

VectorField::VectorField(const std::string& name) : Graph(name), camera(glm::vec3(5.0f, 5.0f, 5.0f)) {
    strncpy(equationBufferX, "y", sizeof(equationBufferX));
    strncpy(equationBufferY, "-x", sizeof(equationBufferY));
    strncpy(equationBufferZ, "0", sizeof(equationBufferZ));

    ResourceManager::LoadShader("shaders/vector.vs", "shaders/vector.fs", std::string("vector"));
    vectorShader = ResourceManager::GetShader("vector");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VectorField::generateMesh() {
    arrowVertices.clear();
    clearMesh();

    double x, y, z;
    te_variable vars[] = {{"x", &x, TE_VARIABLE, 0}, {"y", &y, TE_VARIABLE, 0}, {"z", &z, TE_VARIABLE, 0}};

    int errX, errY, errZ;
    te_expr *exprX = te_compile(equationBufferX, vars, 3, &errX);
    te_expr *exprY = te_compile(equationBufferY, vars, 3, &errY);
    te_expr *exprZ = te_compile(equationBufferZ, vars, 3, &errZ);

    if (exprX && exprY && exprZ) {
        int grid_size = 10;
        float step = 1.0f;

        for (int i = 0; i < grid_size; i++) {
            for (int j = 0; j < grid_size; j++) {
                for (int k = 0; k < grid_size; k++) {
                    x = (i - grid_size / 2) * step;
                    y = (j - grid_size / 2) * step;
                    z = (k - grid_size / 2) * step;

                    float vx = te_eval(exprX);
                    float vy = te_eval(exprY);
                    float vz = te_eval(exprZ);

                    glm::vec3 start_pos = glm::vec3(x, y, z);
                    glm::vec3 end_pos = start_pos + glm::normalize(glm::vec3(vx, vy, vz)) * 0.5f;

                    // Arrow line
                    arrowVertices.push_back(start_pos.x); arrowVertices.push_back(start_pos.y); arrowVertices.push_back(start_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(1.0f); arrowVertices.push_back(1.0f); // White color
                    arrowVertices.push_back(end_pos.x); arrowVertices.push_back(end_pos.y); arrowVertices.push_back(end_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(1.0f); arrowVertices.push_back(1.0f); // White color

                    // Simple arrowhead (cone approximation)
                    glm::vec3 direction = glm::normalize(end_pos - start_pos);
                    glm::vec3 perp1 = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
                    glm::vec3 perp2 = glm::normalize(glm::cross(direction, perp1));

                    float arrow_size = 0.1f;
                    
                    glm::vec3 p1 = end_pos - direction * arrow_size + perp1 * arrow_size * 0.5f;
                    glm::vec3 p2 = end_pos - direction * arrow_size - perp1 * arrow_size * 0.5f;
                    glm::vec3 p3 = end_pos - direction * arrow_size + perp2 * arrow_size * 0.5f;
                    glm::vec3 p4 = end_pos - direction * arrow_size - perp2 * arrow_size * 0.5f;

                    // Arrowhead lines
                    arrowVertices.push_back(end_pos.x); arrowVertices.push_back(end_pos.y); arrowVertices.push_back(end_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color
                    arrowVertices.push_back(p1.x); arrowVertices.push_back(p1.y); arrowVertices.push_back(p1.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color

                    arrowVertices.push_back(end_pos.x); arrowVertices.push_back(end_pos.y); arrowVertices.push_back(end_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color
                    arrowVertices.push_back(p2.x); arrowVertices.push_back(p2.y); arrowVertices.push_back(p2.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color

                    arrowVertices.push_back(end_pos.x); arrowVertices.push_back(end_pos.y); arrowVertices.push_back(end_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color
                    arrowVertices.push_back(p3.x); arrowVertices.push_back(p3.y); arrowVertices.push_back(p3.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color

                    arrowVertices.push_back(end_pos.x); arrowVertices.push_back(end_pos.y); arrowVertices.push_back(end_pos.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color
                    arrowVertices.push_back(p4.x); arrowVertices.push_back(p4.y); arrowVertices.push_back(p4.z);
                    arrowVertices.push_back(1.0f); arrowVertices.push_back(0.0f); arrowVertices.push_back(0.0f); // Red color
                }
            }
        }

        te_free(exprX);
        te_free(exprY);
        te_free(exprZ);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, arrowVertices.size() * sizeof(float), arrowVertices.data(), GL_DYNAMIC_DRAW);
        glBindVertexArray(0);

        meshGenerated = true;
    }
}

void VectorField::update(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CAMERA_RIGHT, deltaTime);
}

void VectorField::render() {
    if (!meshGenerated) {
        generateMesh();
    }

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    vectorShader.Use();
    vectorShader.SetMatrix4("projection", projection);
    vectorShader.SetMatrix4("view", view);
    vectorShader.SetMatrix4("model", model);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, arrowVertices.size() / 6); // 6 floats per vertex (pos + color)
    glBindVertexArray(0);

    Gui::Start();
    ImGui::Begin("VectorField");
    ImGui::InputText("Vx", equationBufferX, sizeof(equationBufferX));
    ImGui::InputText("Vy", equationBufferY, sizeof(equationBufferY));
    ImGui::InputText("Vz", equationBufferZ, sizeof(equationBufferZ));
    if (ImGui::Button("Generate")) {
        generateMesh();
    }
    ImGui::End();
    Gui::Render();
}