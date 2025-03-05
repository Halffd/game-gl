#include "TilemapManager.h"
/**
 * @brief Specialized class for handling animations based on a tilemap.
 */
class Animation : public TilemapManager {
public:
    /**
     * @brief Constructs an Animation instance.
     * @param texturePath Path to the texture atlas.
     * @param tilesAcross Number of tiles across the atlas.
     * @param tilesDown Number of tiles down the atlas.
     * @param frameDuration Duration of each animation frame in seconds.
     */
    Animation(const std::string& texturePath, unsigned int tilesAcross, unsigned int tilesDown, float frameDuration);

    /**
     * @brief Updates the animation state.
     * @param deltaTime Time elapsed since the last update.
     */
    void Update(float deltaTime);

    /**
     * @brief Draws the current frame of the animation.
     * @param renderer SpriteRenderer used for drawing.
     * @param position Position to draw the animation.
     * @param size Size of the animation sprite.
     */
    void DrawAnimation(SpriteRenderer& renderer, const glm::vec2& position, const glm::vec2& size);

private:
    float frameDuration; ///< Duration of each frame in seconds.
    unsigned int currentFrame; ///< Index of the current animation frame.
    float elapsedTime; ///< Time accumulated for frame switching.
};