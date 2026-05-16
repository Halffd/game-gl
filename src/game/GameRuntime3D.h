#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

class Renderer3D;
class Scene;
class Camera;
class Shader;
class Framebuffer;
class ModelObject;
namespace m3D { class Model; }
struct GLFWwindow;

struct Obj3D {
    int id = -1;
    std::string type;
    glm::vec3 pos{0}, rot{0}, scale{1};
    glm::vec3 color{0.8f, 0.8f, 0.8f};
    bool hidden = false;
};

class GameRuntime3D {
public:
    GameRuntime3D();
    ~GameRuntime3D();

    bool initWindow(const std::string& title, int w, int h);
    void destroy();

    // Primitives
    int createObject(const std::string& type, glm::vec3 pos, glm::vec3 scale, glm::vec3 color);
    int loadModel(const std::string& name, const std::string& path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
    void setObjectPosition(int id, glm::vec3 pos);
    void setObjectColor(int id, glm::vec3 color);
    void hideObject(int id);
    void destroyObject(int id);
    glm::vec3 getObjectPosition(int id);
    int objectCount() const { return (int)objects.size(); }

    // Frame
    void beginFrame();
    void endFrame();
    bool shouldClose() const;
    double getTime() const;
    float delta() const { return dt; }

    // Input
    bool keyDown(int key) const;
    bool mouseButton(int btn) const;
    glm::vec2 mousePos() const;

    // Camera
    glm::vec3 camPos() const;
    glm::vec3 camFront() const;
    void moveCamera(int dir, float amount);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Shaders
    int loadShader(const std::string& name, const std::string& vs, const std::string& fs);
    void setShaderVec3(const std::string& name, const std::string& uniform, float x, float y, float z);
    void setShaderFloat(const std::string& name, const std::string& uniform, float v);

    // Framebuffers
    int createFramebuffer(int w, int h);
    void bindFramebuffer(int id);
    void unbindFramebuffer();
    int fbColorTexture(int id);

    // Audio (stub - prints to console)
    void playSound(const std::string& name);

    // ImGui hooks (called at right time in frame)
    void guiBegin(const std::string& title);
    void guiEnd();
    void guiText(const std::string& text);
    bool guiButton(const std::string& label);
    float guiSlider(const std::string& label, float v, float min, float max);

    // Accessors for bindings
    Scene* getScene() const { return scene.get(); }
    Shader* getModelShader() const { return modelShader; }

private:
    GLFWwindow* window = nullptr;
    std::unique_ptr<Renderer3D> renderer;
    std::unique_ptr<Scene> scene;
    Camera* camera = nullptr;
    Shader* modelShader = nullptr;

    std::vector<Obj3D> objects;
    std::vector<std::shared_ptr<ModelObject>> modelObjects;
    std::vector<std::shared_ptr<Framebuffer>> framebuffers;
    std::unordered_map<std::string, Shader> customShaders;

    float dt = 1.0f / 60.0f;
    int width = 1024, height = 768;
    double lastMouseX = 0, lastMouseY = 0;
    bool firstMouse = true;
    bool inGui = false;
};
