#ifndef ITEM_H
#define ITEM_H

#include <SDL2/SDL.h>
#include <string>

enum ItemType { HEALTH, COIN };

class Item {
public:
    Item(SDL_Renderer* ren, int x, int y, ItemType type);
    
    void update();
    void render(SDL_Rect camera);
    
    SDL_Rect getCollider();
    ItemType getType() { return type; }
    bool getIsActive() { return isActive; }
    void collect() { isActive = false; }

private:
    int xpos, ypos;
    ItemType type;
    bool isActive;
    SDL_Renderer* renderer;
    
    // A simple floating effect variable
    float floatOffset;
};

#endif