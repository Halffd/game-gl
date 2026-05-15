#include "PongGame.h"
#include "render/SpriteRenderer.h"
#include "asset/ResourceManager.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const float PADDLE_WIDTH = 20.0f;
const float PADDLE_HEIGHT = 100.0f;
const float BALL_SIZE = 16.0f;

PongGame::PongGame(unsigned int width, unsigned int height)
    : Width(width), Height(height),
      leftPaddleY(height / 2.0f), rightPaddleY(height / 2.0f),
      ballX(width / 2.0f), ballY(height / 2.0f),
      ballVX(300.0f), ballVY(150.0f),
      paddleSpeed(400.0f), ballSpeed(350.0f),
      leftPaddleTarget(height / 2.0f), rightPaddleTarget(height / 2.0f) {
}

PongGame::~PongGame() = default;

void PongGame::init() {
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
    paddleTex = ResourceManager::GetTexture("white");
    ballTex = ResourceManager::GetTexture("face");
}

void PongGame::processInput(float dt) {
    auto* window = glfwGetCurrentContext();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        leftPaddleTarget = std::max(0.0f, leftPaddleTarget - paddleSpeed * dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        leftPaddleTarget = std::min((float)Height - PADDLE_HEIGHT, leftPaddleTarget + paddleSpeed * dt);
    leftPaddleY += (leftPaddleTarget - leftPaddleY) * std::min(1.0f, dt * 10.0f);
}

void PongGame::update(float dt) {
    rightPaddleY += (rightPaddleTarget - rightPaddleY) * std::min(1.0f, dt * 10.0f);
    rightPaddleTarget = std::max(PADDLE_HEIGHT / 2, std::min((float)Height - PADDLE_HEIGHT / 2, rightPaddleTarget));

    ballX += ballVX * dt;
    ballY += ballVY * dt;

    if (ballY <= 0) { ballY = 0; ballVY = -ballVY; }
    if (ballY >= Height - BALL_SIZE) { ballY = Height - BALL_SIZE; ballVY = -ballVY; }

    float lpx = 30;
    if (ballX <= lpx + PADDLE_WIDTH && ballX >= lpx &&
        ballY + BALL_SIZE >= leftPaddleY && ballY <= leftPaddleY + PADDLE_HEIGHT) {
        ballVX = -ballVX * 1.05f;
        ballX = lpx + PADDLE_WIDTH;
        ballVY += (ballY + BALL_SIZE / 2 - leftPaddleY - PADDLE_HEIGHT / 2) * 0.3f;
        if (onBallHitPaddle) onBallHitPaddle(0);
    }

    float rpx = Width - 30 - PADDLE_WIDTH;
    if (ballX + BALL_SIZE >= rpx && ballX <= rpx + PADDLE_WIDTH &&
        ballY + BALL_SIZE >= rightPaddleY && ballY <= rightPaddleY + PADDLE_HEIGHT) {
        ballVX = -ballVX * 1.05f;
        ballX = rpx - BALL_SIZE;
        ballVY += (ballY + BALL_SIZE / 2 - rightPaddleY - PADDLE_HEIGHT / 2) * 0.3f;
        if (onBallHitPaddle) onBallHitPaddle(1);
    }

    if (ballX < -BALL_SIZE) { scoreRight++; resetBall(); }
    if (ballX > Width) { scoreLeft++; resetBall(); }

    float speed = sqrt(ballVX * ballVX + ballVY * ballVY);
    if (speed > ballSpeed * 1.5f) {
        ballVX = ballVX / speed * ballSpeed * 1.5f;
        ballVY = ballVY / speed * ballSpeed * 1.5f;
    }
}

void PongGame::render() {
    auto& shader = ResourceManager::GetShader("sprite");
    glm::mat4 projection = glm::ortho(0.0f, (float)Width, (float)Height, 0.0f, -1.0f, 1.0f);
    shader.Use();
    shader.SetMatrix4("projection", projection);

    renderer->DrawSprite(*paddleTex, glm::vec2(30, leftPaddleY), glm::vec2(PADDLE_WIDTH, PADDLE_HEIGHT), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    renderer->DrawSprite(*paddleTex, glm::vec2(Width - 30 - PADDLE_WIDTH, rightPaddleY), glm::vec2(PADDLE_WIDTH, PADDLE_HEIGHT), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    renderer->DrawSprite(*ballTex, glm::vec2(ballX, ballY), glm::vec2(BALL_SIZE, BALL_SIZE), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void PongGame::resetBall() {
    ballX = Width / 2.0f;
    ballY = Height / 2.0f;
    ballVX = (rand() % 2 == 0 ? 1 : -1) * ballSpeed * 0.8f;
    ballVY = (rand() % 2 == 0 ? 1 : -1) * ballSpeed * 0.4f;
}
