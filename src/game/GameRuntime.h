#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct GLFWwindow;
class SpriteRenderer;
class Shader;
class Texture2D;

struct GameObj {
    float x = 0, y = 0, w = 32, h = 32, rot = 0;
    float r = 1, g = 1, b = 1, a = 1;
    std::string type;
    Texture2D* tex = nullptr;
    bool hidden = false;
};

class GameRuntime {
public:
    GameRuntime();
    ~GameRuntime();

    bool initWindow(const std::string& title, int w, int h);
    void destroyWindow();

    int gameObjectCount() const { return (int)objects.size(); }
    GameObj& getObject(int index);
    int createObject(const std::string& type, float x, float y, float w, float h);
    void destroyObject(int index);

    bool keyDown(int glfwKey) const;
    bool keyUp(int glfwKey) const;
    float delta() const { return dt; }

    void beginFrame();
    void endFrame();
    void draw(int objIndex);

    bool shouldClose() const;
    double getTime() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::unique_ptr<SpriteRenderer> renderer;
    std::vector<GameObj> objects;
    GLFWwindow* window = nullptr;
    Shader* spriteShader = nullptr;
    Texture2D* whiteTex = nullptr;
    float dt = 1.0f / 60.0f;
    int width = 800, height = 600;
};
