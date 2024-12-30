// Collider.h
#ifndef COLLIDER_H
#define COLLIDER_H

#include <memory>
#include "Player.h"
#include "asset/TilemapManager.h"
#include "ui/DialogueSystem.h"

class Collider {
public:
    Collider(std::shared_ptr<DialogueSystem> dialogueSystem, std::shared_ptr<TilemapManager> tilemapManager);
    void Update(std::unique_ptr<Player>& player, float deltaTime);

    // Adjust bounding box without altering the player's size
    void SetBoundingBoxOffset(const glm::vec2& offset);
    void SetBoundingBoxSize(const glm::vec2& size);

private:
    std::shared_ptr<DialogueSystem> dialogueSystem;
    std::shared_ptr<TilemapManager> levelWalls;
    float interactionCooldown;

    glm::vec2 boundingBoxOffset;
    glm::vec2 boundingBoxSize;

    void HandleAxisCollisions(std::unique_ptr<Player>& player, const glm::vec2& oldPosition, bool checkX);
    void HandleCollisions(std::unique_ptr<Player>& player, const glm::vec2& oldPosition);
    bool CheckCollision(const glm::vec2& pos1, const glm::vec2& size1, const glm::vec2& pos2, const glm::vec2& size2) const;
    void ResolveCollision(std::unique_ptr<Player>& player, const glm::vec2& wallPos, const glm::vec2& wallSize, const glm::vec2& oldPosition);
    void HandleInteraction(std::unique_ptr<Player>& player);
};

#endif
