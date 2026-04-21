#include "Map.h"
#include "TextureManager.h"
#include <iostream>
#include <fstream> // NEW: C++ File Stream library

Map::Map(SDL_Renderer* ren) {
    renderer = ren;
    
    // Load tile textures
    water = TextureManager::LoadTexture("water.png", renderer);
    grass = TextureManager::LoadTexture("grass.png", renderer);
    dirt = TextureManager::LoadTexture("dirt.png", renderer);

    if (!water) std::cout << "MAP ERROR: water.png missing or invalid format!" << std::endl;
    if (!grass) std::cout << "MAP ERROR: grass.png missing or invalid format!" << std::endl;
    if (!dirt) std::cout << "MAP ERROR: dirt.png missing or invalid format!" << std::endl;

    // Notice we no longer call LoadMap here! PlayState will handle it.
}

Map::~Map() {
    SDL_DestroyTexture(water);
    SDL_DestroyTexture(grass);
    SDL_DestroyTexture(dirt);
}

void Map::LoadMap(std::string path) {
    // Open the text file
    std::ifstream mapFile(path);
    
    // Safety check if the file wasn't found
    if (!mapFile.is_open()) {
        std::cout << "MAP ERROR: Could not open file " << path << std::endl;
        return;
    }

    // Read the numbers one by one and slot them into our 2D array
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 30; col++) {
            mapFile >> map[row][col];
        }
    }
    
    // Close the file to free memory
    mapFile.close();
}

bool Map::isSolid(int pixelX, int pixelY) {
    // Convert pixel coordinates back to grid coordinates
    int gridX = pixelX / 32;
    int gridY = pixelY / 32;

    // Prevent walking off the edge of the new 25x30 map
    if (gridX < 0 || gridX >= 30 || gridY < 0 || gridY >= 25) {
        return true; 
    }

    // Return true if the tile at this coordinate is 0 (Water)
    return map[gridY][gridX] == 0;
}

void Map::DrawMap(SDL_Rect camera) {
    int type = 0;

    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 30; col++) {
            type = map[row][col];

            // Offset drawing position by the camera's position!
            dest.x = col * 32 - camera.x;
            dest.y = row * 32 - camera.y;
            dest.w = 32; 
            dest.h = 32;

            // Fallback rendering: If texture is missing, draw a colored square instead
            switch (type) {
                case 0:
                    if (water) {
                        SDL_RenderCopy(renderer, water, NULL, &dest);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255); // Blue
                        SDL_RenderFillRect(renderer, &dest);
                    }
                    break;
                case 1:
                    if (grass) {
                        SDL_RenderCopy(renderer, grass, NULL, &dest);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255); // Green
                        SDL_RenderFillRect(renderer, &dest);
                    }
                    break;
                case 2:
                    if (dirt) {
                        SDL_RenderCopy(renderer, dirt, NULL, &dest);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);  // Brown
                        SDL_RenderFillRect(renderer, &dest);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    // Reset the renderer's draw color back to default for anything else drawn afterward
    SDL_SetRenderDrawColor(renderer, 50, 150, 100, 255);
}