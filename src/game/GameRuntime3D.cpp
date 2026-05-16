#include "GameRuntime3D.h"
#include "../render/Renderer3D.h"
#include "../render/Shader.h"
#include "../render/Framebuffer.hpp"
#include "../render/ModelObject.h"
#include "../render/primitives/PrimitiveShapes.h"
#include "../render/primitives/basic/Cube.h"
#include "../render/primitives/curved/Sphere.h"
#include "../scene/Scene.h"
#include "../include/Camera.hpp"
#include "../asset/ResourceManager.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include "../ui/Gui.h"
#include <iostream>

GameRuntime3D::GameRuntime3D() = default;
GameRuntime3D::~GameRuntime3D() { destroy(); }

bool GameRuntime3D::initWindow(const std::string& title, int w, int h) {
    width = w; height = h;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window" << std::endl; return false; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl; return false;
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    ResourceManager::LoadShader("3d.vs", "3d.fs", nullptr, "model");
    ResourceManager::LoadShader("outline.vs", "outline.fs", nullptr, "outline");
    modelShader = &ResourceManager::GetShader("model");

    Gui::Init(window);

    camera = new Camera(glm::vec3(0.0f, 1.7f, 3.0f));
    camera->MouseSensitivity = 0.2f;

    renderer = std::make_unique<Renderer3D>();
    scene = std::make_unique<Scene>();
    renderer->init();
    renderer->dirLight.enabled = true;
    renderer->dirLight.direction = glm::vec3(0.5f, -1.0f, 0.5f);
    renderer->dirLight.diffuse = glm::vec3(0.6f);
    renderer->dirLight.ambient = glm::vec3(0.3f);
    renderer->spotLight.enabled = false;
    return true;
}

void GameRuntime3D::destroy() {
    objects.clear(); modelObjects.clear(); framebuffers.clear(); customShaders.clear();
    scene.reset(); renderer.reset();
    delete camera; camera = nullptr;
    if (window) { glfwDestroyWindow(window); window = nullptr; }
}

int GameRuntime3D::createObject(const std::string& type, glm::vec3 pos, glm::vec3 scale, glm::vec3 color) {
    Obj3D obj;
    obj.id = (int)objects.size();
    obj.type = type; obj.pos = pos; obj.scale = scale; obj.color = color;
    if (type == "cube" || type == "wall")
        scene->AddObject(std::make_shared<m3D::Cube>(type, pos, glm::vec3(0), scale, color));
    else if (type == "sphere" || type == "enemy")
        scene->AddObject(std::make_shared<m3D::Sphere>(type, pos, glm::vec3(0), scale, color, 16, 16, 1.0f));
    objects.push_back(obj);
    return obj.id;
}

int GameRuntime3D::loadModel(const std::string& name, const std::string& path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    std::string fullPath = ResourceManager::GetModelPath(path);
    if (fullPath.empty()) fullPath = path;
    auto obj = std::make_shared<ModelObject>(name, fullPath, pos, rot, scale);
    scene->AddObject(obj);
    modelObjects.push_back(obj);
    int id = (int)objects.size();
    Obj3D o; o.id = id; o.type = "model";
    objects.push_back(o);
    return id;
}

void GameRuntime3D::setObjectPosition(int id, glm::vec3 pos) {
    if (id < 0 || id >= (int)objects.size()) return;
    objects[id].pos = pos;
    auto& objs = scene->getObjects();
    if (id < (int)objs.size()) objs[id]->position = pos;
}

void GameRuntime3D::setObjectColor(int id, glm::vec3 color) {
    if (id >= 0 && id < (int)objects.size()) objects[id].color = color;
}

glm::vec3 GameRuntime3D::getObjectPosition(int id) {
    return (id >= 0 && id < (int)objects.size()) ? objects[id].pos : glm::vec3(0);
}

void GameRuntime3D::hideObject(int id) {
    if (id >= 0 && id < (int)objects.size()) objects[id].hidden = true;
}

void GameRuntime3D::destroyObject(int id) {
    if (id >= 0 && id < (int)objects.size()) objects.erase(objects.begin() + id);
}

void GameRuntime3D::beginFrame() {
    static double lastTime = 0;
    double now = glfwGetTime();
    dt = (float)(now - lastTime);
    if (dt > 0.1f) dt = 0.1f;
    lastTime = now;
    glfwPollEvents();
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    if (firstMouse) { lastMouseX = mx; lastMouseY = my; firstMouse = false; }
    double dx = mx - lastMouseX;
    double dy = lastMouseY - my;
    lastMouseX = mx; lastMouseY = my;
    if (dx != 0 || dy != 0) camera->ProcessMouseMovement((float)dx, (float)dy);
    inGui = false;
}

void GameRuntime3D::endFrame() {
    renderer->render(*scene, *camera);
    glfwSwapBuffers(window);
}

bool GameRuntime3D::shouldClose() const { return glfwWindowShouldClose(window); }
double GameRuntime3D::getTime() const { return glfwGetTime(); }
bool GameRuntime3D::keyDown(int key) const { return window && glfwGetKey(window, key) == GLFW_PRESS; }
bool GameRuntime3D::mouseButton(int btn) const { return window && glfwGetMouseButton(window, btn) == GLFW_PRESS; }
glm::vec2 GameRuntime3D::mousePos() const {
    double x, y; glfwGetCursorPos(window, &x, &y);
    return glm::vec2((float)x, (float)y);
}
glm::vec3 GameRuntime3D::camPos() const { return camera ? camera->Position : glm::vec3(0); }
glm::vec3 GameRuntime3D::camFront() const { return camera ? camera->Front : glm::vec3(0, 0, -1); }
void GameRuntime3D::moveCamera(int dir, float) {
    if (!camera) return;
    auto d = static_cast<Camera_Movement>(dir);
    camera->ProcessKeyboard(d, dt);
}

// Shaders
int GameRuntime3D::loadShader(const std::string& name, const std::string& vs, const std::string& fs) {
    try {
        ResourceManager::LoadShader(vs.c_str(), fs.c_str(), nullptr, name);
        customShaders[name] = ResourceManager::GetShader(name);
        return 1;
    } catch (...) { return 0; }
}

void GameRuntime3D::setShaderVec3(const std::string& name, const std::string& uniform, float x, float y, float z) {
    auto it = customShaders.find(name);
    if (it != customShaders.end()) it->second.SetVector3f(uniform.c_str(), x, y, z);
}

void GameRuntime3D::setShaderFloat(const std::string& name, const std::string& uniform, float v) {
    auto it = customShaders.find(name);
    if (it != customShaders.end()) it->second.SetFloat(uniform.c_str(), v);
}

// Framebuffers
int GameRuntime3D::createFramebuffer(int w, int h) {
    auto fb = std::make_shared<Framebuffer>();
    if (!fb->create(w, h)) return -1;
    int id = (int)framebuffers.size();
    framebuffers.push_back(std::move(fb));
    return id;
}

void GameRuntime3D::bindFramebuffer(int id) {
    if (id >= 0 && id < (int)framebuffers.size())
        framebuffers[id]->bind();
}

void GameRuntime3D::unbindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

int GameRuntime3D::fbColorTexture(int id) {
    if (id >= 0 && id < (int)framebuffers.size())
        return (int)framebuffers[id]->getColorTexture();
    return 0;
}

// Audio stub
void GameRuntime3D::playSound(const std::string& name) {
    std::cout << "[Audio] Would play: " << name << std::endl;
}

// ImGui wrappers
void GameRuntime3D::guiBegin(const std::string& title) {
    if (!inGui) { inGui = true; }
    ImGui::Begin(title.c_str());
}

void GameRuntime3D::guiEnd() { ImGui::End(); }

void GameRuntime3D::guiText(const std::string& text) {
    ImGui::Text("%s", text.c_str());
}

bool GameRuntime3D::guiButton(const std::string& label) {
    return ImGui::Button(label.c_str());
}

float GameRuntime3D::guiSlider(const std::string& label, float v, float min, float max) {
    ImGui::SliderFloat(label.c_str(), &v, min, max);
    return v;
}
