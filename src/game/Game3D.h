#pragma once

#include "../scene/Scene.h"
#include "../render/Renderer3D.h"
#include "../include/Camera.hpp"
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../render/Model.h"
#include "../render/primitives/PrimitiveShapes.h"
#include "render/Framebuffer.hpp"
#include "render/primitives/2d/2D.hpp"

struct GLFWwindow;

class Game3D {
public:
    Game3D();
    void init();
    void run();

private:
    void processInput();
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(double xoffset, double yoffset);
    bool toggleKey(int key, bool &toggleState);
    void toggleCursor();
    void initSolarSystemScene();
    void loadModels(const std::string& modelBasePath, const std::string& binModelBasePath);
    bool loadModel(const std::string& name, const std::string& relativePath, const std::string& modelRoot, const std::string& binRoot, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    std::shared_ptr<Framebuffer> m_framebuffer;
    std::shared_ptr<VO::Quad> m_screenQuad;
    Shader m_postProcessShader;

public:
    Camera camera;
    bool firstMouse;
    float lastX;
    float lastY;
    float deltaTime;
    float lastFrame;
    bool cursorEnabled;
    std::map<int, bool> keyWasPressed;

    glm::mat4 projection;

    std::vector<m3D::Model *> models;
    std::vector<glm::vec3> modelPositions;
    std::vector<glm::vec3> modelRotations;
    std::vector<glm::vec3> modelScales;
    bool showModelsWindow;
    int selectedModel;

    std::vector<std::string> modelNames;

    std::vector<std::shared_ptr<m3D::PrimitiveShape> > primitiveShapes;
    std::vector<glm::vec3> rotationSpeeds;

    std::vector<std::shared_ptr<m3D::PrimitiveShape> > dynamicShapes;
    struct OrbitalData {
        std::shared_ptr<m3D::PrimitiveShape> body;
        float orbitRadius;
        float orbitSpeed;
        float rotationSpeed;
        float currentAngle;
        std::shared_ptr<m3D::PrimitiveShape> parentBody;
    };
    std::vector<OrbitalData> orbitalBodies;

    std::vector<std::shared_ptr<m3D::DynamicTransform> > dynamicTransforms;
    bool useDynamicShapes;
    bool showDynamicShapesWindow;
    bool useSolarSystemScene;

    std::shared_ptr<m3D::Mesh> sphereMesh;

    bool showCartesianPlane;
    bool showTriangleContours;
    bool runMode;
    float baseMovementSpeed;
    int maxAsteroids = 100;

private:
    Scene scene;
    Renderer3D renderer;
    GLFWwindow* window;
};