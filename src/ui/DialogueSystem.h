
// DialogueSystem.h
#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Gui.h"

class DialogueSystem {
public:
    struct DialogueNode {
        std::string text;
        std::vector<std::string> choices;
        std::vector<int> nextNodes;
        bool isEnd;
    };

    DialogueSystem();
    void LoadDialogue(int id, const std::string& file);
    void StartDialogue(int id);
    void Update();
    bool IsDialogueActive() const { return isActive; }
    void EndDialogue();

private:
    std::unordered_map<int, std::vector<DialogueNode>> dialogues;
    bool isActive;
    int currentDialogueId;
    int currentNodeIndex;
    float textSpeed;
    std::string currentDisplayText;
    float textTimer;
    
    void RenderDialogueWindow();
    void UpdateTextReveal(float dt);
};
