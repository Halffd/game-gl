#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    static std::shared_ptr<Camera> Instance; // Singleton instance

    Camera(glm::vec2 position, glm::vec2 size);
    
    void SetPosition(glm::vec2 position);
    void SetSize(glm::vec2 size);
    void SetZoom(float zoom);
    
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
};

#endif // CAMERA_H