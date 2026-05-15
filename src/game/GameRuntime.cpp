#include "GameRuntime.h"
#include "render/SpriteRenderer.h"
#include "asset/ResourceManager.h"
#include "asset/Texture2D.h"
#include "render/Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

GameRuntime::GameRuntime() = default;
GameRuntime::~GameRuntime() { destroyWindow(); }

bool GameRuntime::initWindow(const std::string& title, int w, int h) {
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ResourceManager::LoadShader("sprite/vertex.glsl", "sprite/fragment.glsl", nullptr, "sprite");
    spriteShader = &ResourceManager::GetShader("sprite");
    auto proj = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    spriteShader->Use().SetInteger("image", 0);
    spriteShader->SetMatrix4("projection", proj);

    ResourceManager::LoadTexture2D("white.png", "white");
    whiteTex = ResourceManager::GetTexture("white");

    const std::vector<float> verts = {
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };
    renderer = std::make_unique<SpriteRenderer>(*spriteShader, verts);
    return true;
}

void GameRuntime::destroyWindow() {
    objects.clear();
    renderer.reset();
    if (window) { glfwDestroyWindow(window); window = nullptr; }
}

GameObj& GameRuntime::getObject(int index) { return objects[index]; }

int GameRuntime::createObject(const std::string& type, float x, float y, float w, float h) {
    GameObj obj;
    obj.type = type; obj.x = x; obj.y = y; obj.w = w; obj.h = h;
    if (type == "sprite") obj.tex = whiteTex;
    objects.push_back(obj);
    return (int)objects.size() - 1;
}

void GameRuntime::destroyObject(int index) {
    if (index >= 0 && index < (int)objects.size())
        objects.erase(objects.begin() + index);
}

bool GameRuntime::keyDown(int key) const {
    return window && glfwGetKey(window, key) == GLFW_PRESS;
}

bool GameRuntime::keyUp(int key) const {
    return window && glfwGetKey(window, key) == GLFW_RELEASE;
}

void GameRuntime::beginFrame() {
    static double lastTime = 0;
    double now = glfwGetTime();
    dt = (float)(now - lastTime);
    lastTime = now;
    glfwPollEvents();
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto proj = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    spriteShader->Use();
    spriteShader->SetMatrix4("projection", proj);
}

void GameRuntime::endFrame() {
    glfwSwapBuffers(window);
}

void GameRuntime::draw(int objIndex) {
    if (objIndex < 0 || objIndex >= (int)objects.size()) return;
    auto& o = objects[objIndex];
    if (o.hidden) return;
    auto& tex = o.tex ? *o.tex : *whiteTex;
    renderer->DrawSprite(tex, glm::vec2(o.x, o.y), glm::vec2(o.w, o.h),
        o.rot, glm::vec3(o.r, o.g, o.b));
}

bool GameRuntime::shouldClose() const { return glfwWindowShouldClose(window); }

double GameRuntime::getTime() const { return glfwGetTime(); }
