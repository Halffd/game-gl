// Collider.h
#ifndef COLLIDER_H
#define COLLIDER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Player.h"
#include "GameObject.h"
#include "ui/DialogueSystem.h"

/**
 * @brief Manages collision detection and interaction handling for a top-down RPG game.
 * 
 * The Collider class handles:
 * - AABB collision detection between game objects
 * - Player-object interaction logic
 * - Dialogue triggering for interactable objects
 * - Collision resolution with proper physics response
 */
class Collider {
public:
    /**
     * @brief Construct a new Collider object
     * @param dialogueSystem Shared pointer to the dialogue system
     */
    explicit Collider(std::shared_ptr<DialogueSystem> dialogueSystem);
    
    /**
     * @brief Updates collision and interaction checks for the current frame
     * @param player Reference to the player object
     * @param deltaTime Time elapsed since last frame
     */
    void Update(std::shared_ptr<Player> player, float deltaTime);
    
    /**
     * @brief Sets the current level's objects for collision checking
     * @param walls Vector of wall objects in the current level
     * @param interactables Vector of interactable objects in the current level
     */
    void SetLevelObjects(
        std::vector<std::shared_ptr<GameObject>> walls,
        std::vector<std::shared_ptr<GameObject>> interactables
    );

private:
    // Constants
    static constexpr float INTERACTION_RANGE = 20.0f;
    static constexpr float INTERACTION_COOLDOWN_TIME = 0.5f;
    static constexpr float FACING_DIRECTION_THRESHOLD = 0.5f;

    // Member variables
    std::shared_ptr<DialogueSystem> dialogueSystem;
    std::vector<std::shared_ptr<GameObject>> levelWalls;
    std::vector<std::shared_ptr<GameObject>> levelInteractables;
    float interactionCooldown;

    /**
     * @brief Handles all collision checks and resolutions for the current frame
     * @param player Player object to check collisions for
     * @param oldPosition Player's position before movement
     */
    void HandleCollisions(std::shared_ptr<Player> player, const glm::vec2& oldPosition);

    /**
     * @brief Checks for AABB collision between two objects
     * @param one First game object
     * @param two Second game object
     * @return true if objects are colliding
     */
    bool CheckCollision(const GameObject& one, const GameObject& two) const;

    /**
     * @brief Checks if an object is within interaction range of the player
     * @param player Player attempting to interact
     * @param object Target object for interaction
     * @return true if object is within range
     */
    bool CheckInteractionRange(const GameObject& player, const GameObject& object) const;

    /**
     * @brief Resolves collision by adjusting player position
     * @param player Player object to adjust
     * @param wall Wall object collided with
     * @param oldPosition Player's position before collision
     */
    void ResolveCollision(std::shared_ptr<Player> player, 
                         const GameObject& wall,
                         const glm::vec2& oldPosition);

    /**
     * @brief Handles interaction between player and an interactable object
     * @param player Player attempting interaction
     * @param object Object being interacted with
     */
    void HandleInteraction(std::shared_ptr<Player> player, 
                          const GameObject& object);
};

#endif // COLLIDER_H