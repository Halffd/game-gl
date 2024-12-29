// Collider.cpp
#include "Collider.h"
#include <algorithm>
#include <GLFW/glfw3.h> // For key codes

Collider::Collider(std::shared_ptr<DialogueSystem> dialogueSystem)
    : dialogueSystem(dialogueSystem)
    , interactionCooldown(0.0f)
{
    if (!dialogueSystem) {
        throw std::runtime_error("Collider: DialogueSystem pointer cannot be null");
    }
}

void Collider::Update(std::shared_ptr<Player> player, float deltaTime) {
    if (!player) {
        return;
    }

    // Update interaction cooldown
    if (interactionCooldown > 0.0f) {
        interactionCooldown -= deltaTime;
    }

    // Store old position for collision resolution
    glm::vec2 oldPosition = player->Position;
    
    // Update player position based on input
    player->Position += player->Velocity * deltaTime;
    
    // Check collisions with all walls/objects
    HandleCollisions(player, oldPosition);
}

void Collider::SetLevelObjects(
    std::vector<std::shared_ptr<GameObject>> walls,
    std::vector<std::shared_ptr<GameObject>> interactables
) {
    levelWalls = std::move(walls);
    levelInteractables = std::move(interactables);
}

void Collider::HandleCollisions(std::shared_ptr<Player> player, const glm::vec2& oldPosition) {
    // Check collisions with walls
    for (const auto& wall : levelWalls) {
        if (wall && !wall->Destroyed && CheckCollision(*player, *wall)) {
            ResolveCollision(player, *wall, oldPosition);
        }
    }
    
    // Check for interactable objects
    for (const auto& obj : levelInteractables) {
        if (obj && obj->interactable && CheckInteractionRange(*player, *obj)) {
            HandleInteraction(player, *obj);
        }
    }
}

bool Collider::CheckCollision(const GameObject& one, const GameObject& two) const {
    // AABB Collision check
    bool collisionX = one.Position.x + one.Size.x > two.Position.x && 
                     two.Position.x + two.Size.x > one.Position.x;
    bool collisionY = one.Position.y + one.Size.y > two.Position.y && 
                     two.Position.y + two.Size.y > one.Position.y;
    
    return collisionX && collisionY;
}

bool Collider::CheckInteractionRange(const GameObject& player, const GameObject& object) const {
    glm::vec2 playerCenter = player.Position + player.Size * 0.5f;
    glm::vec2 objectCenter = object.Position + object.Size * 0.5f;
    
    float distance = glm::length(playerCenter - objectCenter);
    return distance <= INTERACTION_RANGE;
}

void Collider::ResolveCollision(
    std::shared_ptr<Player> player,
    const GameObject& wall,
    const glm::vec2& oldPosition
) {
    // Calculate overlap on both axes
    float overlapX = std::min(
        player->Position.x + player->Size.x - wall.Position.x,
        wall.Position.x + wall.Size.x - player->Position.x
    );
    
    float overlapY = std::min(
        player->Position.y + player->Size.y - wall.Position.y,
        wall.Position.y + wall.Size.y - player->Position.y
    );

    // Resolve along axis with smallest overlap
    if (overlapX < overlapY) {
        // X-axis collision
        if (player->Position.x < wall.Position.x) {
            player->Position.x = wall.Position.x - player->Size.x;
        } else {
            player->Position.x = wall.Position.x + wall.Size.x;
        }
        player->Velocity.x = 0.0f;
    } else {
        // Y-axis collision
        if (player->Position.y < wall.Position.y) {
            player->Position.y = wall.Position.y - player->Size.y;
        } else {
            player->Position.y = wall.Position.y + wall.Size.y;
        }
        player->Velocity.y = 0.0f;
    }
}

void Collider::HandleInteraction(std::shared_ptr<Player> player, const GameObject& object) {
    // Only handle interaction if cooldown is ready and interaction key is pressed
    if (interactionCooldown <= 0.0f && glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E)) {
        // Get facing direction as vector
        glm::vec2 facingVector;
        switch (player->GetFacing()) {
            case Player::Direction::UP:    
                facingVector = glm::vec2(0.0f, -1.0f); 
                break;
            case Player::Direction::DOWN:  
                facingVector = glm::vec2(0.0f, 1.0f); 
                break;
            case Player::Direction::LEFT:  
                facingVector = glm::vec2(-1.0f, 0.0f); 
                break;
            case Player::Direction::RIGHT: 
                facingVector = glm::vec2(1.0f, 0.0f); 
                break;
        }

        // Check if player is facing the object
        glm::vec2 toObject = glm::normalize(object.Position - player->Position);
        float dotProduct = glm::dot(toObject, facingVector);

        if (dotProduct > FACING_DIRECTION_THRESHOLD) {
            // Trigger dialogue if available
            if (object.dialogueID >= 0) {
                dialogueSystem->StartDialogue(object.dialogueID);
            }
            
            interactionCooldown = INTERACTION_COOLDOWN_TIME;
        }
    }
}