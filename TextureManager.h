#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class TextureManager {
public:
    // A static function so we can call it anywhere without instantiating the class
    static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* renderer);
};

#endif