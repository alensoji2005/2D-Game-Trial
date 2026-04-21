#ifndef NPC_H
#define NPC_H

#include "GameObject.h"
#include <stdlib.h> // Included for rand()

// NPC inherits from GameObject
class NPC : public GameObject {
public:
    NPC(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    
    // Override the update method to add custom AI logic
    void update() override;

private:
    int waitTimer;
};

#endif