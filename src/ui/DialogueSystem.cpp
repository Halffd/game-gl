// DialogueSystem.cpp
#include "DialogueSystem.h"

DialogueSystem::DialogueSystem() 
    : isActive(false), 
      currentDialogueId(-1), 
      currentNodeIndex(0),
      textSpeed(30.0f), // Characters per second
      textTimer(0.0f)
{
}

void DialogueSystem::Update() {
    if (!isActive) return;

    ImGui::SetNextWindowSize(ImVec2(800, 200), ImGuiCond_Once);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y - 220),
        ImGuiCond_Once,
        ImVec2(0.5f, 0.0f)
    );

    if (ImGui::Begin("Dialogue", nullptr, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar)) {
        
        const auto& currentDialogue = dialogues[currentDialogueId][currentNodeIndex];
        
        // Display text with typewriter effect
        ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 20);
        ImGui::TextWrapped("%s", currentDisplayText.c_str());
        ImGui::PopTextWrapPos();
        
        // Update typewriter effect
        if (currentDisplayText.length() < currentDialogue.text.length()) {
            textTimer += ImGui::GetIO().DeltaTime;
            if (textTimer >= 1.0f / textSpeed) {
                textTimer = 0.0f;
                currentDisplayText += currentDialogue.text[currentDisplayText.length()];
            }
        }
        
        // Show choices if text is fully displayed
        if (currentDisplayText.length() == currentDialogue.text.length() && 
            !currentDialogue.choices.empty()) {
            
            ImGui::Spacing();
            for (size_t i = 0; i < currentDialogue.choices.size(); i++) {
                if (ImGui::Button(currentDialogue.choices[i].c_str(), 
                    ImVec2(ImGui::GetWindowWidth() - 30, 0))) {
                    if (currentDialogue.nextNodes[i] >= 0) {
                        currentNodeIndex = currentDialogue.nextNodes[i];
                        currentDisplayText = "";
                    } else {
                        EndDialogue();
                    }
                }
            }
        }
        // Continue button for non-choice dialogues
        else if (currentDisplayText.length() == currentDialogue.text.length() && 
                 currentDialogue.choices.empty()) {
            if (ImGui::Button("Continue", ImVec2(ImGui::GetWindowWidth() - 30, 0))) {
                if (currentDialogue.isEnd) {
                    EndDialogue();
                } else {
                    currentNodeIndex++;
                    currentDisplayText = "";
                }
            }
        }
    }
    ImGui::End();
}

void DialogueSystem::StartDialogue(int id) {
    if (dialogues.find(id) != dialogues.end()) {
        isActive = true;
        currentDialogueId = id;
        currentNodeIndex = 0;
        currentDisplayText = "";
        textTimer = 0.0f;
    }
}

void DialogueSystem::EndDialogue() {
    isActive = false;
    currentDialogueId = -1;
    currentNodeIndex = 0;
    currentDisplayText = "";
}