#include "Camera.h"

// Initialize the static instance pointer
std::shared_ptr<Camera> Camera::Instance = nullptr;

Camera::Camera(glm::vec2 position, glm::vec2 size)
    : Position(position), Size(size), Zoom(1.0f) {
    UpdateViewMatrix();
}

std::shared_ptr<Camera> Camera::GetInstance(glm::vec2 position, glm::vec2 size) {
    if (!Instance) {
        Instance = std::shared_ptr<Camera>(new Camera(position, size));
    }
    return Instance;
}

void Camera::SetPosition(glm::vec2 position) {
    Position = position;
    UpdateViewMatrix();
}

void Camera::SetSize(glm::vec2 size) {
    Size = size;
    UpdateViewMatrix();
}

void Camera::SetZoom(float zoom) {
    Zoom = zoom;
    UpdateViewMatrix();
}

void Camera::FollowPlayer(const glm::vec2& playerPosition) {
    SetPosition(playerPosition);
}

void Camera::FocusOnObject(const glm::vec2& objectPosition) {
    SetPosition(objectPosition);
}

void Camera::Update(float dt) {
    // This can be used for animations or smooth transitions
}

glm::mat4 Camera::GetViewMatrix() const {
    return View;
}

void Camera::UpdateViewMatrix() {
    View = glm::translate(glm::mat4(1.0f), glm::vec3(-Position, 0.0f));
    View = glm::scale(View, glm::vec3(Zoom, Zoom, 1.0f));
}