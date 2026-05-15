#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

class SpriteRenderer;
class Texture2D;

class PongGame {
public:
    PongGame(unsigned int width, unsigned int height);
    ~PongGame();

    void init();
    void processInput(float dt);
    void update(float dt);
    void render();
    void resetBall();
    float getBallVX() const { return ballVX; }
    float getBallVY() const { return ballVY; }
    float getPaddleHeight() const { return 100.0f; }
    void setRightPaddleTarget(float y) { rightPaddleTarget = y; }

    // Public state for Havel scripting
    float leftPaddleY = 0.0f;
    float rightPaddleY = 0.0f;
    float ballX = 0.0f;
    float ballY = 0.0f;
    int scoreLeft = 0;
    int scoreRight = 0;
    unsigned int Width, Height;

    // Script-controlled hook
    void (*onBallHitPaddle)(int side) = nullptr;

private:
    std::unique_ptr<SpriteRenderer> renderer;
    Texture2D* paddleTex;
    Texture2D* ballTex;

    float ballVX, ballVY;
    float paddleSpeed;
    float ballSpeed;
    float leftPaddleTarget;
    float rightPaddleTarget;
};
