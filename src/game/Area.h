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
    // Constructor
    Area(GameMode mode);

    // Loads an area from a tilemap file (only applicable for GAME mode)
    void LoadTilemap(const char* file, unsigned int areaWidth, unsigned int areaHeight);

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
    void init(const std::vector<std::vector<unsigned int>>& tileData, unsigned int areaWidth, unsigned int areaHeight);

    GameMode currentMode;                           // Current mode of the game
    std::shared_ptr<TilemapManager> tilemapManager; // Tilemap manager for handling static tiles in GAME mode
    std::shared_ptr<Gui> guiManager;                // GUI manager for handling non-tilemap modes
    std::vector<GameObject> interactiveTiles;       // Interactive or dynamic objects in the area
};

#endif // AREA_H