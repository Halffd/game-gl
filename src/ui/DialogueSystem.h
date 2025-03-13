#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class DialogueSystem {
public:
    DialogueSystem();
    ~DialogueSystem();

    void ShowDialogue(const std::string& text);
    void Update(float deltaTime);
    void Render();
    bool IsActive() const;
    void Close();

private:
    bool active;
    std::string currentText;
    float displayTime;
};

#endif 