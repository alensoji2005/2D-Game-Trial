#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class UIManager {
public:
    static TTF_Font* loadFont(const char* path, int fontSize);
    static void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
    static void drawTextBox(SDL_Renderer* renderer, TTF_Font* font, const std::string& text);
};

#endif