#include "Camera.h"
std::shared_ptr<Camera> Camera::Instance = nullptr;

Camera::Camera(glm::vec2 position, glm::vec2 size)
    : Position(position), Size(size), Zoom(1.0f) {
    UpdateViewMatrix();
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
    // Simple follow logic; you can add smoothing if desired
    SetPosition(playerPosition);
}

void Camera::FocusOnObject(const glm::vec2& objectPosition) {
    // Center the camera on the specified object
    SetPosition(objectPosition);
}

void Camera::Update(float dt) {
    // This can be used for animations or smooth transitions
    // For example, implement a lerp function here if you want to smooth the camera motion
}

glm::mat4 Camera::GetViewMatrix() const {
    return View;
}

void Camera::UpdateViewMatrix() {
    View = glm::translate(glm::mat4(1.0f), glm::vec3(-Position, 0.0f));
    View = glm::scale(View, glm::vec3(Zoom, Zoom, 1.0f));
}