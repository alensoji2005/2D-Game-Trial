#include "Map.h"
#include "TextureManager.h"
#include <iostream>
#include <fstream> 

Map::Map(SDL_Renderer* ren) {
    renderer = ren;
    
    // --- NEW: Load the single tileset image ---
    tileset = TextureManager::LoadTexture("tileset.png", renderer);

    if (!tileset) {
        std::cout << "MAP ERROR: tileset.png missing or invalid format!" << std::endl;
    }
    
    // Set the static width and height for our cropping rectangle
    src.w = 32;
    src.h = 32;
    dest.w = 32;
    dest.h = 32;
}

Map::~Map() {
    SDL_DestroyTexture(tileset); // Clean up memory
}

void Map::LoadMap(std::string path) {
    std::ifstream mapFile(path);
    
    if (!mapFile.is_open()) {
        std::cout << "MAP ERROR: Could not open file " << path << std::endl;
        return;
    }

    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 30; col++) {
            mapFile >> map[row][col];
        }
    }
    
    mapFile.close();
}

bool Map::isSolid(int pixelX, int pixelY) {
    int gridX = pixelX / 32;
    int gridY = pixelY / 32;

    if (gridX < 0 || gridX >= 30 || gridY < 0 || gridY >= 25) {
        return true; 
    }

    // Tile 0 is Water, which is a solid obstacle
    return map[gridY][gridX] == 0;
}

void Map::DrawMap(SDL_Rect camera) {
    int type = 0;

    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 30; col++) {
            type = map[row][col];

            dest.x = col * 32 - camera.x;
            dest.y = row * 32 - camera.y;

            // --- NEW: TILESET CROPPING MATH ---
            // If type is 0 (Water), src.x is 0. 
            // If type is 1 (Grass), src.x is 32. 
            // If type is 2 (Dirt),  src.x is 64.
            src.x = type * 32;
            src.y = 0; // Assuming all our tiles are in a single horizontal row for now

            if (tileset) {
                // Copy ONLY the specific 32x32 chunk from the tileset to the screen!
                SDL_RenderCopy(renderer, tileset, &src, &dest);
            } else {
                // Fallback coloring just in case tileset.png is missing
                if (type == 0) SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
                else if (type == 1) SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                else if (type == 2) SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);
                SDL_RenderFillRect(renderer, &dest);
            }
        }
    }
    
    SDL_SetRenderDrawColor(renderer, 50, 150, 100, 255);
}