#ifndef NPC_H
#define NPC_H

#include "GameObject.h"
#include <stdlib.h> 

class NPC : public GameObject {
public:
    NPC(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    
    // --- NEW: Pass the player's position into the update loop ---
    void update(SDL_Rect playerCol);

private:
    int waitTimer;
    int aggroRadius; // NEW: How close the player has to be to trigger a chase
};

#endif