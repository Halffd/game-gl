#include "SnakeGame.h"
#include "render/SpriteRenderer.h"
#include "asset/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <ctime>

SnakeGame::SnakeGame(unsigned int width, unsigned int height)
    : Width(width), Height(height),
      gridWidth(width / cellSize), gridHeight(height / cellSize),
      dir(1, 0), nextDir(1, 0),
      moveTimer(0), moveInterval(0.15f), needsFood(true) {
    reset();
}

SnakeGame::~SnakeGame() = default;

void SnakeGame::init() {
    auto& shader = ResourceManager::GetShader("sprite");
    const std::vector<float> vertices = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    renderer = std::make_unique<SpriteRenderer>(shader, vertices);
    cellTex = ResourceManager::GetTexture("white");
    foodTex = ResourceManager::GetTexture("face");
}

void SnakeGame::update(float dt) {
    if (gameOver) return;
    moveTimer += dt;
    if (moveTimer < moveInterval) return;
    moveTimer = 0;

    dir = nextDir;
    glm::ivec2 head = snake.front() + dir;
    if (head.x < 0 || head.x >= gridWidth || head.y < 0 || head.y >= gridHeight) {
        gameOver = true;
        return;
    }
    for (size_t i = 0; i < snake.size(); i++) {
        if (snake[i] == head) { gameOver = true; return; }
    }
    snake.insert(snake.begin(), head);
    if (head.x == foodX && head.y == foodY) {
        score++;
        moveInterval = std::max(0.05f, moveInterval - 0.003f);
        spawnFood();
    } else {
        snake.pop_back();
    }
}

void SnakeGame::render() {
    auto& shader = ResourceManager::GetShader("sprite");
    glm::mat4 projection = glm::ortho(0.0f, (float)Width, (float)Height, 0.0f, -1.0f, 1.0f);
    shader.Use();
    shader.SetMatrix4("projection", projection);

    for (size_t i = 0; i < snake.size(); i++) {
        float brightness = 1.0f - (float)i / snake.size() * 0.5f;
        renderer->DrawSprite(*cellTex,
            glm::vec2(snake[i].x * cellSize, snake[i].y * cellSize),
            glm::vec2(cellSize, cellSize), 0.0f,
            glm::vec3(0.2f, brightness, 0.2f));
    }
    renderer->DrawSprite(*foodTex,
        glm::vec2(foodX * cellSize, foodY * cellSize),
        glm::vec2(cellSize, cellSize), 0.0f,
        glm::vec3(1.0f, 0.2f, 0.2f));
}

void SnakeGame::setDirection(const std::string& dirStr) {
    if (dirStr == "up" && dir.y != 1) nextDir = {0, -1};
    else if (dirStr == "down" && dir.y != -1) nextDir = {0, 1};
    else if (dirStr == "left" && dir.x != 1) nextDir = {-1, 0};
    else if (dirStr == "right" && dir.x != -1) nextDir = {1, 0};
}

std::string SnakeGame::getDirection() const {
    if (dir.x == 1) return "right";
    if (dir.x == -1) return "left";
    if (dir.y == -1) return "up";
    if (dir.y == 1) return "down";
    return "right";
}

int SnakeGame::getSnakeLength() const { return (int)snake.size(); }

void SnakeGame::spawnFood() {
    bool valid = false;
    int attempts = 0;
    while (!valid && attempts < 100) {
        foodX = rand() % gridWidth;
        foodY = rand() % gridHeight;
        valid = true;
        for (const auto& s : snake) {
            if (s.x == foodX && s.y == foodY) { valid = false; break; }
        }
        attempts++;
    }
}

void SnakeGame::reset() {
    snake.clear();
    snake.push_back({gridWidth / 2, gridHeight / 2});
    snake.push_back({gridWidth / 2 - 1, gridHeight / 2});
    snake.push_back({gridWidth / 2 - 2, gridHeight / 2});
    dir = {1, 0};
    nextDir = {1, 0};
    score = 0;
    gameOver = false;
    moveTimer = 0;
    moveInterval = 0.15f;
    spawnFood();
}
