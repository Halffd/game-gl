#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "../asset/Texture2D.h"

class GameLevel
{
public:
    // level state
    std::vector<GameObject> Bricks;
    
    // constructor
    GameLevel() {}
    
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    
    // render level
    void Draw(SpriteRenderer &renderer);
    
    // check if the level is completed (all non-solid bricks are destroyed)
    bool IsCompleted();

private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif 