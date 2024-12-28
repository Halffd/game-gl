#include "Animation.h"

Animation::Animation(const std::string& texturePath, unsigned int tilesAcross, unsigned int tilesDown, float frameDuration)
    : TilemapManager(texturePath, tilesAcross, tilesDown), frameDuration(frameDuration), currentFrame(0), elapsedTime(0.0f) {}

void Animation::Update(float deltaTime) {
    elapsedTime += deltaTime;
    if (elapsedTime >= frameDuration) {
        elapsedTime = 0.0f;
        currentFrame = (currentFrame + 1) % (tilesAcross * tilesDown);
    }
}

void Animation::DrawAnimation(SpriteRenderer& renderer, const glm::vec2& position, const glm::vec2& size) {
    unsigned int texX = currentFrame % tilesAcross;
    unsigned int texY = currentFrame / tilesAcross;
    glm::vec2 textureOffset = glm::vec2(texX * (1.0f / tilesAcross), texY * (1.0f / tilesDown));
    glm::vec2 textureSize = glm::vec2(1.0f / tilesAcross, 1.0f / tilesDown);

    renderer.DrawSprite(
        texture,
        position,
        size,
        0.0f,
        glm::vec3(1.0f),
        textureOffset,
        textureSize
    );
}
