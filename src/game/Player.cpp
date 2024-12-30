// Player.cpp
#include "Player.h"

Player::Player()
    : GameObject(),
      facing(Direction::DOWN),
      movementSpeed(225.0f),  // Increased base movement speed
      isMoving(false),
      animationTimer(0.0f),
      currentFrame(0),
      frameTime(0.0f),
      name("Player"),
      level(1) 
{
}

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, unsigned int tileWidth, unsigned int tileHeight)
    : GameObject(pos, size, sprite, color, glm::vec2(0.0f)),
      facing(Direction::DOWN),
      movementSpeed(225.0f),  // Increased from 100.0f to 400.0f
      isMoving(false),
      animationTimer(0.0f),
      currentFrame(0),
      frameTime(0.0f),
      name("Player"),
      level(1)
{
    sheet = std::make_shared<TilemapManager>(Sprite, tileWidth, tileHeight);
    sheet->LoadTilemap(glm::vec2(tileWidth, tileHeight));
}

void Player::Move(Direction dir) {
    facing = dir;
    isMoving = true;
    
    // Calculate normalized velocity vector
    glm::vec2 moveDir(0.0f);
    switch(dir) {
        case Direction::UP:
            moveDir = glm::vec2(0.0f, -1.0f);
            break;
        case Direction::DOWN:
            moveDir = glm::vec2(0.0f, 1.0f);
            break;
        case Direction::LEFT:
            moveDir = glm::vec2(-1.0f, 0.0f);
            break;
        case Direction::RIGHT:
            moveDir = glm::vec2(1.0f, 0.0f);
            break;
    }
    
    // Set velocity with full movement speed
    Velocity = moveDir * movementSpeed;
}

void Player::Stop() {
    isMoving = false;
    Velocity = glm::vec2(0.0f);
}

void Player::Update(float dt) {
    if (isMoving) {
        // Apply movement with delta time
        Position += Velocity * dt;
        UpdateAnimation(dt);
    }
}

void Player::UpdateAnimation(float dt) {
    frameTime += dt;
    if (frameTime >= FRAME_DURATION) {
        frameTime = 0.0f;
        currentFrame = (currentFrame + 1) % 4;
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
    sheet->DrawPlayer(renderer, this->Position, this->Size, tile);
}

