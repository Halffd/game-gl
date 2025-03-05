#include "Collider.h"
#include <iostream>
#include <algorithm>

Collider::Collider(std::shared_ptr<DialogueSystem> dialogueSystem, std::shared_ptr<TilemapManager> tilemapManager)
    : dialogueSystem(dialogueSystem), levelWalls(tilemapManager), interactionCooldown(0.0f) {}

void Collider::Update(std::shared_ptr<Player>& player, float deltaTime) {
    // Reduce interaction cooldown timer
    if (interactionCooldown > 0.0f) {
        interactionCooldown -= deltaTime;
    }

    // Save the player's current position for collision resolution
    glm::vec2 oldPosition = player->Position;

    // Handle collisions
    HandleCollisions(player, oldPosition);
}

void Collider::HandleAxisCollisions(std::shared_ptr<Player>& player, const glm::vec2& oldPosition, [[maybe_unused]] bool checkX) {
    if (!levelWalls) {
        std::cerr << "TilemapManager not set for collision detection!" << std::endl;
        return;
    }

    // Get actual bounding box values
    glm::vec2 actualSize = (boundingBoxSize == glm::vec2(0.0f)) ? player->Size : boundingBoxSize;
    glm::vec2 boxPosition = player->Position + boundingBoxOffset;
    
    for (const auto& tile : levelWalls->tiles) {
        if (!tile.IsSolid) {
            continue; // Skip non-solid tiles
        }

        // Check for AABB collision using the bounding box
        if (CheckCollision(boxPosition, actualSize, tile.Position, tile.Size)) {
            // Calculate overlap
            float overlapX = std::min(boxPosition.x + actualSize.x, tile.Position.x + tile.Size.x) -
                            std::max(boxPosition.x, tile.Position.x);
            float overlapY = std::min(boxPosition.y + actualSize.y, tile.Position.y + tile.Size.y) -
                            std::max(boxPosition.y, tile.Position.y);

            // Determine which axis to resolve based on the smallest overlap
            if (overlapX < overlapY) {
                // Resolve X-axis collision
                if (oldPosition.x + boundingBoxOffset.x < tile.Position.x) {
                    player->Position.x = tile.Position.x - actualSize.x - boundingBoxOffset.x;
                }
                else {
                    player->Position.x = tile.Position.x + tile.Size.x - boundingBoxOffset.x;
                }
            }
            else {
                // Resolve Y-axis collision
                if (oldPosition.y + boundingBoxOffset.y < tile.Position.y) {
                    player->Position.y = tile.Position.y - actualSize.y - boundingBoxOffset.y;
                }
                else {
                    player->Position.y = tile.Position.y + tile.Size.y - boundingBoxOffset.y;
                }
            }
        }
    }
}

void Collider::HandleCollisions(std::shared_ptr<Player>& player, const glm::vec2& oldPosition) {
    if (!levelWalls) {
        std::cerr << "TilemapManager not set for collision detection!" << std::endl;
        return;
    }

    // Get actual bounding box values
    glm::vec2 actualSize = (boundingBoxSize == glm::vec2(0.0f)) ? player->Size : boundingBoxSize;
    glm::vec2 boxPosition = player->Position + boundingBoxOffset;
    
    for (const auto& tile : levelWalls->tiles) {
        if (!tile.IsSolid) {
            continue; // Skip non-solid tiles
        }

        // Check for AABB collision using the bounding box
        if (CheckCollision(boxPosition, actualSize, tile.Position, tile.Size)) {
            // Calculate overlap
            float overlapX = std::min(boxPosition.x + actualSize.x, tile.Position.x + tile.Size.x) -
                            std::max(boxPosition.x, tile.Position.x);
            float overlapY = std::min(boxPosition.y + actualSize.y, tile.Position.y + tile.Size.y) -
                            std::max(boxPosition.y, tile.Position.y);

            // Determine which axis to resolve based on the smallest overlap
            if (overlapX < overlapY) {
                // Resolve X-axis collision
                if (oldPosition.x + boundingBoxOffset.x < tile.Position.x) {
                    player->Position.x = tile.Position.x - actualSize.x - boundingBoxOffset.x;
                }
                else {
                    player->Position.x = tile.Position.x + tile.Size.x - boundingBoxOffset.x;
                }
            }
            else {
                // Resolve Y-axis collision
                if (oldPosition.y + boundingBoxOffset.y < tile.Position.y) {
                    player->Position.y = tile.Position.y - actualSize.y - boundingBoxOffset.y;
                }
                else {
                    player->Position.y = tile.Position.y + tile.Size.y - boundingBoxOffset.y;
                }
            }
        }
    }
}

void Collider::SetBoundingBoxOffset(const glm::vec2& offset) {
    boundingBoxOffset = offset;
}

void Collider::SetBoundingBoxSize(const glm::vec2& size) {
    boundingBoxSize = size;
}

bool Collider::CheckCollision(const glm::vec2& pos1, const glm::vec2& size1, const glm::vec2& pos2, const glm::vec2& size2) const {
    // Collision x-axis?
    bool collisionX = pos1.x + size1.x >= pos2.x &&
        pos2.x + size2.x >= pos1.x;
    
    // Collision y-axis?
    bool collisionY = pos1.y + size1.y >= pos2.y &&
        pos2.y + size2.y >= pos1.y;
    
    // Collision only if on both axes
    return collisionX && collisionY;
}

void Collider::ResolveCollision([[maybe_unused]] std::shared_ptr<Player>& player, 
                               [[maybe_unused]] const glm::vec2& wallPos,
                               [[maybe_unused]] const glm::vec2& wallSize, 
                               [[maybe_unused]] const glm::vec2& oldPosition) {
    // ... existing implementation ...
}

void Collider::HandleInteraction([[maybe_unused]] std::shared_ptr<Player>& player) {
    // ... existing implementation ...
}