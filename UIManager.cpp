#include "UIManager.h"
#include <iostream>

TTF_Font* UIManager::loadFont(const char* path, int fontSize) {
    TTF_Font* font = TTF_OpenFont(path, fontSize);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    return font;
}

void UIManager::drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    
    // Render text to a surface, then convert to a hardware texture
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void UIManager::drawTextBox(SDL_Renderer* renderer, TTF_Font* font, const std::string& text) {
    // Draw a semi-transparent black rectangle at the bottom of the screen
    SDL_Rect box = {50, 450, 700, 100};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Enable transparency
    SDL_RenderFillRect(renderer, &box);

    // Draw a solid white border around it
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &box);

    // Draw the actual dialogue text inside the box
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    drawText(renderer, font, text, 70, 470, textColor);
}