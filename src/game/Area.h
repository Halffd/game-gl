#ifndef AREA_H
#define AREA_H

#include "GameObject.h"
#include "asset/TilemapManager.h"
#include "asset/ResourceManager.h"
#include "ui/Gui.h"
#include <vector>
#include <string>
#include <memory>
#include "gamemode.h"

class Area {
public:
    unsigned int Width;
    unsigned int Height;
    // Constructor
    Area(unsigned int levelWidth, unsigned int levelHeight);

    // Loads an area from a tilemap file (only applicable for GAME mode)
    void LoadTilemap(const char* file, const char* texturePath, unsigned int tileWidth, unsigned int tileHeight);

    // Renders the area or UI based on the mode
    void Draw(SpriteRenderer& renderer);

    // Updates logic for the area (handles GUI interactions in non-tilemap modes)
    void Update(float deltaTime);

    // Checks if the area objectives are completed (only applicable for GAME mode)
    bool IsCompleted() const;

    // Cleans up resources
    void Clean();

private:
    // Initializes the area from tile data
    std::vector<std::vector<unsigned int>> readTileData(const std::string& filename);
    std::shared_ptr<TilemapManager> tilemapManager; // Tilemap manager for handling static tiles in GAME mode
};

#endif // AREA_H