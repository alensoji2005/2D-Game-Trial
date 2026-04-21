#include "Projectile.h"

Projectile::Projectile(SDL_Renderer* ren, int x, int y, int dx, int dy) {
    renderer = ren;
    xpos = x;
    ypos = y;
    xVel = dx;
    yVel = dy;
    speed = 8; // Projectiles move twice as fast as the player!
    isActive = true;
}

void Projectile::update() {
    xpos += xVel * speed;
    ypos += yVel * speed;
}

void Projectile::render(SDL_Rect camera) {
    // We will draw a bright orange 16x16 square as our fireball
    SDL_Rect dest = {xpos - camera.x, ypos - camera.y, 16, 16};
    
    SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255); // Orange
    SDL_RenderFillRect(renderer, &dest);
    
    // Reset color back to default just in case
    SDL_SetRenderDrawColor(renderer, 50, 150, 100, 255);
}

SDL_Rect Projectile::getCollider() {
    SDL_Rect col = {xpos, ypos, 16, 16};
    return col;
}