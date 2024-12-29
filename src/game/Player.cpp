// Player.cpp
#include "Player.h"

Player::Player() 
    : GameObject(),
      facing(Direction::DOWN),
      movementSpeed(100.0f),
      isMoving(false),
      animationTimer(0.0f),
      currentFrame(0),
      frameTime(0.0f),
      name("Player"),
      level(1)
{
}

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color)
    : GameObject(pos, size, sprite, color, glm::vec2(0.0f)),
      facing(Direction::DOWN),
      movementSpeed(100.0f),
      isMoving(false),
      animationTimer(0.0f),
      currentFrame(0),
      frameTime(0.0f),
      name("Player"),
      level(1)
{
}

void Player::Move(Direction dir) {
    facing = dir;
    isMoving = true;
    
    // Set velocity based on direction
    switch(dir) {
        case Direction::UP:
            Velocity = glm::vec2(0.0f, -movementSpeed);
            break;
        case Direction::DOWN:
            Velocity = glm::vec2(0.0f, movementSpeed);
            break;
        case Direction::LEFT:
            Velocity = glm::vec2(-movementSpeed, 0.0f);
            break;
        case Direction::RIGHT:
            Velocity = glm::vec2(movementSpeed, 0.0f);
            break;
    }
}

void Player::Update(float dt) {
    if (isMoving) {
        Position += Velocity * dt;
        UpdateAnimation(dt);
    }
}

void Player::UpdateAnimation(float dt) {
    frameTime += dt;
    if (frameTime >= FRAME_DURATION) {
        frameTime = 0.0f;
        currentFrame = (currentFrame + 1) % 4; // Assuming 4 frames per direction
        UpdateSpriteFrame();
    }
}

void Player::UpdateSpriteFrame() {
    // Assuming sprite sheet is organized with rows for each direction
    // and columns for animation frames
    float frameWidth = 1.0f / 4.0f;  // 4 frames per row
    float frameHeight = 1.0f / 4.0f; // 4 directions
    
    float yOffset = 0.0f;
    switch(facing) {
        case Direction::DOWN:  yOffset = 0.0f; break;
        case Direction::LEFT:  yOffset = 0.25f; break;
        case Direction::RIGHT: yOffset = 0.5f; break;
        case Direction::UP:    yOffset = 0.75f; break;
    }
    
    // Update sprite texture coordinates
    float xOffset = currentFrame * frameWidth;
    // You would need to implement a way to update the sprite's texture coordinates here
}

void Player::Draw(SpriteRenderer& renderer) {
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

