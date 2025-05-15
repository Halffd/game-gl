#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Camera {
public:
    static std::shared_ptr<Camera> GetInstance(glm::vec2 position = glm::vec2(0.0f, 0.0f), glm::vec2 size = glm::vec2(800.0f, 600.0f));
    static std::shared_ptr<Camera> Instance; // Singleton instance

    Camera(glm::vec2 position, glm::vec2 size); // Private constructor
    void SetPosition(glm::vec2 position);
    void SetSize(glm::vec2 size);
    void SetZoom(float zoom);
    void SetProjectionMatrix(glm::mat4 projectionMatrix);
    glm::mat4 GetProjectionMatrix();
    
    void FollowPlayer(const glm::vec2& playerPosition);
    void FocusOnObject(const glm::vec2& objectPosition);
    void Update(float dt); // Update logic, can include animations or transitions

    glm::mat4 GetViewMatrix() const;

private:
    void UpdateViewMatrix();
    
    glm::vec2 Position; // Camera position
    glm::vec2 Size;     // Size of the camera view
    float Zoom;         // Zoom level
    glm::mat4 View;     // View matrix
    glm::mat4 Projection;
};

#endif // CAMERA_H