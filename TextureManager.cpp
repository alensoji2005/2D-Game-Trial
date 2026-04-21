#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const char* fileName, SDL_Renderer* renderer) {
    // Load the image into RAM
    SDL_Surface* tempSurface = IMG_Load(fileName);
    
    // SAFETY CHECK: If the image fails to load, print the exact error to the VS Code terminal
    if (tempSurface == NULL) {
        std::cout << "Failed to load texture: " << fileName << " Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    // Convert it to a hardware-accelerated texture on the GPU
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    
    // Clean up the RAM surface since it's on the GPU now
    SDL_FreeSurface(tempSurface);
    
    return texture;
}