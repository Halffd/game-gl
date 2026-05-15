#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

class SpriteRenderer;
class Texture2D;

class SnakeGame {
public:
    SnakeGame(unsigned int width, unsigned int height);
    ~SnakeGame();

    void init();
    void update(float dt);
    void render();

    unsigned int Width, Height;
    int score = 0;
    bool gameOver = false;

    void setDirection(const std::string& dir);
    std::string getDirection() const;
    int getSnakeLength() const;
    int getFoodX() const { return foodX; }
    int getFoodY() const { return foodY; }
    int getHeadX() const { return snake.empty() ? 0 : snake.front().x; }
    int getHeadY() const { return snake.empty() ? 0 : snake.front().y; }

private:
    std::unique_ptr<SpriteRenderer> renderer;
    Texture2D* cellTex;
    Texture2D* foodTex;

    int cellSize = 20;
    int gridWidth, gridHeight;

    std::vector<glm::ivec2> snake;
    glm::ivec2 dir;
    glm::ivec2 nextDir;
    int foodX, foodY;
    float moveTimer;
    float moveInterval;
    bool needsFood;

    void spawnFood();
    void reset();
};
