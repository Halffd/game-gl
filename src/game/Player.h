// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <string>
#include <glm/glm.hpp>

class Player : public GameObject {
public:
    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    Player();
    Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f));
    
    // Getters/Setters
    Direction GetFacing() const { return facing; }
    float GetMovementSpeed() const { return movementSpeed; }
    bool IsMoving() const { return isMoving; }
    
    // Movement methods
    void Move(Direction dir);
    void Update(float dt);
    void Draw(SpriteRenderer& renderer) override;
    
    // Stats and inventory could be added here
    std::string name;
    int level;
    
private:
    Direction facing;
    float movementSpeed;
    bool isMoving;
    float animationTimer;
    
    // Sprite animation
    int currentFrame;
    float frameTime;
    const float FRAME_DURATION = 0.16f; // ~6 frames per second
    
    void UpdateAnimation(float dt);
    void UpdateSpriteFrame();
};
#endif