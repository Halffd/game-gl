#include "Area.h"
#include <fstream>
#include <sstream>
#include <iostream>

Area::Area(GameMode mode) : currentMode(mode), tilemapManager(nullptr), guiManager(nullptr) {
    if (mode != GAME) {
        guiManager = std::make_shared<Gui>();
    }
}

void Area::LoadTilemap(const char* file, unsigned int areaWidth, unsigned int areaHeight) {
    if (currentMode != GAME) return;

    tilemapManager = std::make_shared<TilemapManager>(file, areaWidth, areaHeight); // Adjust the texture path and tile dimensions as needed
}

void Area::Draw(SpriteRenderer& renderer) {
    if (currentMode == GAME && tilemapManager) {
        tilemapManager->Draw(renderer);
        for (auto& tile : interactiveTiles) {
            if (!tile.Destroyed) {
                tile.Draw(renderer);
            }
        }
    } else if (guiManager) {
        guiManager->Start();
        if (currentMode == MENU) {
            ImGui::Begin("Main Menu");
            ImGui::Text("Welcome to the Game!");
            if (ImGui::Button("Start Game")) {
                std::cout << "Game Start Selected" << std::endl;
            }
            if (ImGui::Button("Credits")) {
                std::cout << "Credits Selected" << std::endl;
            }
            ImGui::End();
        } else if (currentMode == CREDITS) {
            ImGui::Begin("Credits");
            ImGui::Text("Created by Halff and Team");
            if (ImGui::Button("Back to Menu")) {
                std::cout << "Returning to Menu" << std::endl;
            }
            ImGui::End();
        }
        guiManager->Render();
    }
}

void Area::Update(float deltaTime) {
    if (currentMode == GAME && tilemapManager) {
        // Update logic for the game mode (e.g., animation or object states)
    } else if (guiManager) {
        // Handle GUI-specific updates
    }
}

bool Area::IsCompleted() const {
    if (currentMode != GAME) return false;

    for (const auto& tile : interactiveTiles) {
        if (!tile.IsSolid && !tile.Destroyed) {
            return false;
        }
    }
    return true;
}

void Area::Clean() {
    if (tilemapManager) {
        tilemapManager.reset();
    }
    if (guiManager) {
        guiManager->Clean();
        guiManager.reset();
    }
}

void Area::init(const std::vector<std::vector<unsigned int>>& tileData, unsigned int areaWidth, unsigned int areaHeight) {
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unitWidth = static_cast<float>(areaWidth) / width;
    float unitHeight = static_cast<float>(areaHeight) / height;

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (tileData[y][x] == 1) {
                glm::vec2 pos(unitWidth * x, unitHeight * y);
                glm::vec2 size(unitWidth, unitHeight);
                GameObject obj(pos, size, ResourceManager::GetTexture2D("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                interactiveTiles.push_back(obj);
            } else if (tileData[y][x] > 1) {
                glm::vec3 color(1.0f);
                if (tileData[y][x] == 2) color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3) color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4) color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5) color = glm::vec3(1.0f, 0.5f, 0.0f);

                glm::vec2 pos(unitWidth * x, unitHeight * y);
                glm::vec2 size(unitWidth, unitHeight);
                interactiveTiles.push_back(GameObject(pos, size, ResourceManager::GetTexture2D("bookshelf"), color));
            }
        }
    }
}
