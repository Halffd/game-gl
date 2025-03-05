#include "TilemapManager.h"
#include <iostream>

TilemapManager::TilemapManager(const std::string& texturePath, unsigned int tilesAcross, unsigned int tilesDown)
    : texturePath(texturePath), tilesAcross(tilesAcross), tilesDown(tilesDown) {
    ResourceManager::LoadTexture2D(texturePath.c_str(), "tilemap");
    texture = std::make_shared<Texture2D>(
        ResourceManager::GetTexture2D(texturePath)
    );
}

TilemapManager::TilemapManager(Texture2D& tex, unsigned int tilesAcross, unsigned int tilesDown)
    : tilesAcross(tilesAcross), tilesDown(tilesDown) {
    texture = std::make_shared<Texture2D>(tex);
}

TilemapManager::TilemapManager(const std::string& texturePath, const std::string& bgTexturePath, unsigned int tilesAcross, unsigned int tilesDown)
    : texturePath(texturePath), tilesAcross(tilesAcross), tilesDown(tilesDown) {
    ResourceManager::LoadTexture2D(texturePath.c_str(), "tilemap");
    texture = std::make_shared<Texture2D>(
        ResourceManager::GetTexture2D(texturePath)
    );
    bgTexture = std::make_shared<Texture2D>(
        ResourceManager::GetTexture2D(bgTexturePath)
    ); 
}
void TilemapManager::LoadTilemap(const std::vector<std::vector<unsigned int>>& tileData, 
                                [[maybe_unused]] unsigned int levelWidth, 
                                [[maybe_unused]] unsigned int levelHeight) {
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

            //if (tileIndex == 0) continue; // Skip empty tiles

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
            if(tileIndex != 0){
                tile.TileID = tileIndex; 
                tile.TextureOffset = textureOffset;
                tile.TextureSize = textureSize;
            } else {
                tile.TextureSize = glm::vec2(0.0f,0.0f);
            }
            tile.IsSolid = (tileIndex != 40); // Mark solid tiles (customize as needed)

            tiles.push_back(tile);
        }
    }
}
void TilemapManager::LoadTilemap(glm::vec2 dim) {
    tiles.clear();

    // Calculate individual tile dimensions in world space
    float tileWorldWidth = static_cast<float>(texture->Width);
    float tileWorldHeight = static_cast<float>(texture->Height);

    // Calculate UV dimensions for a single tile in texture space
    float tileUVWidth = 1.0f / static_cast<float>(tilesAcross);
    float tileUVHeight = 1.0f / static_cast<float>(tilesDown);

    for (unsigned int row = 0; row < dim.x; ++row) {
        for (unsigned int col = 0; col < dim.y; ++col) {
            unsigned int tileIndex = row * static_cast<unsigned int>(dim.y) + col;

            // Calculate texture coordinates (UV offset)
            unsigned int texCol = (tileIndex) % tilesAcross;
            unsigned int texRow = (tileIndex) / tilesAcross;

            glm::vec2 tilePosition(col * tileWorldWidth, row * tileWorldHeight);
            glm::vec2 tileSize(tileWorldWidth, tileWorldHeight);
            glm::vec2 textureOffset(texCol * tileUVWidth, texRow * tileUVHeight);
            glm::vec2 textureSize(tileUVWidth, tileUVHeight);
            Tile tile;
            tile.Position = glm::vec2(0.0f,0.0f);
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
void TilemapManager::DrawBackground(SpriteRenderer& renderer, int width, int height) {
    glm::vec2 size = glm::vec2(bgTexture->Width, bgTexture->Height); // Tile size

    for (int i = 0; i < width / size.x; i++) {
        for (int j = 0; j < height / size.y; j++) {
            renderer.DrawSprite(
                *bgTexture,                          // Texture to use
                glm::vec2(((float)(size.x) * (float) i) - width / 2.0f, ((float)(size.y) * (float) j) - height / 4.0f), // Position in world space
                size,                                 // Size of the tile
                0.0f,                                 // No rotation
                glm::vec3(1.0f)                       // Default color (white)
            );
        }
    }
}
void TilemapManager::DrawPlayer(SpriteRenderer& renderer, glm::vec2 pos, 
                              [[maybe_unused]] glm::vec2 size, int tile) {
    if (tile < 0 || static_cast<size_t>(tile) >= tiles.size()) {
        return;
    }

    if (!texture || texture->ID == 0) {
        std::cerr << "Player texture not loaded properly!" << std::endl;
        return;
    }

    Tile& tileData = tiles[tile];
    
    renderer.DrawSprite(
        *texture,              // Texture to use
        pos,                   // Position in world space
        glm::vec2(tileData.Size.x, tileData.Size.y),                  // Size of the tile
        0.0f,                
        glm::vec3(1.0f),       // Default color (white)
        tileData.TextureOffset,// Texture UV offset
        tileData.TextureSize   // Texture UV size
    );
}
