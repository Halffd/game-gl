#include "TilemapManager.h"
#include <iostream>

TilemapManager::TilemapManager(const std::string& texturePath, unsigned int tilesAcross, unsigned int tilesDown)
    : texturePath(texturePath), tilesAcross(tilesAcross), tilesDown(tilesDown) {
    texture = ResourceManager::LoadTexture2D(texturePath.c_str(), "tilemap");
}

void TilemapManager::LoadTilemap(const std::vector<std::vector<unsigned int>>& tileData, unsigned int levelWidth, unsigned int levelHeight) {
    tiles.clear();

    float tileWidth = static_cast<float>(levelWidth) / tileData[0].size();
    float tileHeight = static_cast<float>(levelHeight) / tileData.size();

    float texWidth = 1.0f / tilesAcross;
    float texHeight = 1.0f / tilesDown;

    for (unsigned int y = 0; y < tileData.size(); ++y) {
        for (unsigned int x = 0; x < tileData[y].size(); ++x) {
            unsigned int tileIndex = tileData[y][x];
            if (tileIndex == 0) continue; // Skip empty tiles

            unsigned int texX = (tileIndex - 1) % tilesAcross;
            unsigned int texY = (tileIndex - 1) / tilesAcross;

            Tile tile;
            tile.Position = glm::vec2(x * tileWidth, y * tileHeight);
            tile.Size = glm::vec2(tileWidth, tileHeight);
            tile.TextureOffset = glm::vec2(texX * texWidth, texY * texHeight);
            tile.TextureSize = glm::vec2(texWidth, texHeight);
            tile.IsSolid = tileIndex == 1; // Mark solid tiles (customize as needed)

            tiles.push_back(tile);
        }
    }
}

void TilemapManager::Draw(SpriteRenderer& renderer) {
    for (const Tile& tile : tiles) {
        renderer.DrawSprite(
            texture,
            tile.Position,
            tile.Size,
            0.0f,
            glm::vec3(1.0f), // Default color
            tile.TextureOffset,
            tile.TextureSize
        );
    }
}
