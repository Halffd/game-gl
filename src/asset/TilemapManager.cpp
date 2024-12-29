#include "TilemapManager.h"
#include <iostream>

TilemapManager::TilemapManager(const std::string& texturePath, unsigned int tilesAcross, unsigned int tilesDown)
    : texturePath(texturePath), tilesAcross(tilesAcross), tilesDown(tilesDown) {
    ResourceManager::LoadTexture2D(texturePath.c_str(), "tilemap");
    texture = std::make_shared<Texture2D>(
        ResourceManager::GetTexture2D(texturePath)
    );
}

void TilemapManager::LoadTilemap(const std::vector<std::vector<unsigned int>>& tileData, unsigned int levelWidth, unsigned int levelHeight) {
    tiles.clear();

    // Calculate individual tile dimensions in world space
    float tileWorldWidth = static_cast<float>(texture->Width);
    float tileWorldHeight = static_cast<float>(texture->Height);

    // Calculate UV dimensions for a single tile in texture space
    float tileUVWidth = 1.0f / static_cast<float>(tilesAcross);
    float tileUVHeight = 1.0f / static_cast<float>(tilesDown);

    for (unsigned int row = 0; row < tileData.size(); ++row) {
        for (unsigned int col = 0; col < tileData[row].size(); ++col) {
            unsigned int tileIndex = tileData[row][col];

            if (tileIndex == 0) continue; // Skip empty tiles

            // Calculate texture coordinates (UV offset)
            unsigned int texCol = (tileIndex - 1) % tilesAcross;
            unsigned int texRow = (tileIndex - 1) / tilesAcross;

            glm::vec2 tilePosition(col * tileWorldWidth, row * tileWorldHeight);
            glm::vec2 tileSize(tileWorldWidth, tileWorldHeight);
            glm::vec2 textureOffset(texCol * tileUVWidth, texRow * tileUVHeight);
            glm::vec2 textureSize(tileUVWidth, tileUVHeight);

            Tile tile;
            tile.Position = tilePosition;
            tile.Size = tileSize;
            tile.TileID = tileIndex; 
            tile.TextureOffset = textureOffset;
            tile.TextureSize = textureSize;
            tile.IsSolid = (tileIndex == 1); // Mark solid tiles (customize as needed)

            tiles.push_back(tile);
        }
    }
}

void TilemapManager::Draw(SpriteRenderer& renderer) {
    for (const Tile& tile : tiles) {
        renderer.DrawSprite(
            *texture,                  // Texture to use
            tile.Position,             // Position in world space
            tile.Size,                 // Size of the tile
            0.0f,                      // No rotation
            glm::vec3(1.0f),           // Default color (white)
            tile.TextureOffset,        // Texture UV offset
            tile.TextureSize           // Texture UV size
        );
    }
}
