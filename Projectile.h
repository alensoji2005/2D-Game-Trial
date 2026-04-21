#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL2/SDL.h>

class Projectile {
public:
    Projectile(SDL_Renderer* ren, int x, int y, int dx, int dy);
    
    void update();
    void render(SDL_Rect camera);
    
    SDL_Rect getCollider();
    bool getIsActive() { return isActive; }
    void destroy() { isActive = false; }

private:
    int xpos, ypos;
    int xVel, yVel;
    int speed;
    bool isActive;
    SDL_Renderer* renderer;
};

#endif