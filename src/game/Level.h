#ifndef LEVEL_H
#define LEVEL_H
#include "GameObject.h"
#include "asset/ResourceManager.h"

class Level
{
public:
    // level state
    std::vector<GameObject> Bricks;
    // constructor
    Level() { }
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void Draw(SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, 
              unsigned int levelWidth, unsigned int levelHeight);
};  
#endif