// Graph.h - Base abstract class
#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <string>

struct GLFWwindow;

class Graph {
public:
    struct GraphSettings {
        glm::vec3 color = glm::vec3(0.2f, 0.8f, 0.6f);
        float lineWidth = 2.0f;
        bool showGrid = true;
        bool showAxes = true;
        std::string title = "";
        glm::vec3 axisColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 gridColor = glm::vec3(0.3f, 0.3f, 0.3f);
    };

protected:
    GraphSettings settings;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    bool meshGenerated = false;
    std::string functionName;
    
public:
    Graph(const std::string& name) : functionName(name) {}
    virtual ~Graph() = default;
    
    // Pure virtual methods - must be implemented by derived classes
    virtual void generateMesh() = 0;
    virtual void update(GLFWwindow* window, float deltaTime) = 0;
    virtual void render() = 0;
    
    // Common methods
    void setColor(const glm::vec3& color) { settings.color = color; }
    void setTitle(const std::string& title) { settings.title = title; }
    GraphSettings& getSettings() { return settings; }
    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    
protected:
    void clearMesh() {
        vertices.clear();
        indices.clear();
        meshGenerated = false;
    }
};