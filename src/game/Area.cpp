#include "Area.h"
#include <fstream>
#include <sstream>
#include <iostream>

Area::Area(unsigned int levelWidth, unsigned int levelHeight) {
    Width = levelWidth;
    Height = levelHeight;
}

// Function to read tile data from a file
std::vector<std::vector<unsigned int>> Area::readTileData(const std::string& filename) {
    std::vector<std::vector<unsigned int>> tileData;
    std::ifstream file(ResourceManager::root + filename);
    
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return tileData;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::vector<unsigned int> row;
        unsigned int tile;
        while (stream >> tile) {
            row.push_back(tile);
        }
        tileData.push_back(row);
    }

    file.close();
    return tileData;
}

void Area::LoadTilemap(const char* file, const char* texturePath, unsigned int tileWidth, unsigned int tileHeight) {
    std::vector<std::vector<unsigned int>> data = readTileData(file);
    tilemapManager = std::make_shared<TilemapManager>(texturePath, tileWidth, tileHeight); // Adjust the texture path and tile dimensions as needed
    tilemapManager->LoadTilemap(data, Width, Height);
}

void Area::Draw(SpriteRenderer& renderer) {
    if (tilemapManager) {
        tilemapManager->Draw(renderer);
    } 
}

void Area::Update(float deltaTime) {
    
}

bool Area::IsCompleted() const {
    return true;
}

void Area::Clean() {
    if (tilemapManager) {
        tilemapManager.reset();
    }
}