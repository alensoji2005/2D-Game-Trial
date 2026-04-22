#ifndef BOSS_H
#define BOSS_H

#include "GameObject.h"
#include "Projectile.h"
#include <vector>

class Boss : public GameObject {
public:
    Boss(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    
    // Custom update loop for the boss AI and dynamic sprite sizing
    void update(SDL_Rect playerCol, std::vector<Projectile*>& enemyProjectiles);
    void render(SDL_Rect camera);
    
    // Override the collider to match the giant 128x128 size!
    SDL_Rect getCollider() { return {xpos + 16, ypos + 16, 96, 96}; }

private:
    int shootTimer;
    int moveTimer;
};

#endif