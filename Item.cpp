#include "Item.h"
#include <cmath>

Item::Item(SDL_Renderer* ren, int x, int y, ItemType t) {
    renderer = ren;
    xpos = x;
    ypos = y;
    type = t;
    isActive = true;
    floatOffset = 0;
}

void Item::update() {
    // Simple bobbing animation
    floatOffset += 0.05f;
}

void Item::render(SDL_Rect camera) {
    int bob = (int)(std::sin(floatOffset) * 5);
    SDL_Rect dest = {xpos - camera.x, (ypos + bob) - camera.y, 16, 16};
    
    if (type == HEALTH) {
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // Red Heart
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Gold Coin
    }
    
    SDL_RenderFillRect(renderer, &dest);
}

SDL_Rect Item::getCollider() {
    return {xpos, ypos, 16, 16};
}