#include "DialogueSystem.h"

DialogueSystem::DialogueSystem() : active(false), displayTime(0.0f) {}

DialogueSystem::~DialogueSystem() {}

void DialogueSystem::ShowDialogue(const std::string& text) {
    active = true;
    currentText = text;
    displayTime = 0.0f;
}

void DialogueSystem::Update(float deltaTime) {
    if (active) {
        displayTime += deltaTime;
        // Auto-close after 5 seconds
        if (displayTime > 5.0f) {
            Close();
        }
    }
}

void DialogueSystem::Render() {
    if (active) {
        // Rendering would be implemented here
    }
}

bool DialogueSystem::IsActive() const {
    return active;
}

void DialogueSystem::Close() {
    active = false;
    currentText = "";
} 