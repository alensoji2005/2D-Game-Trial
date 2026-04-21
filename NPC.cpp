#include "NPC.h"
#include <SDL2/SDL.h>

NPC::NPC(const char* textureSheet, SDL_Renderer* renderer, int x, int y) 
    : GameObject(textureSheet, renderer, x, y) {
    speed = 2; // NPCs walk a bit slower than the player
    waitTimer = 0;
}

void NPC::update() {
    waitTimer++;

    // Every 60 frames (about 1 second), pick a new random direction
    if (waitTimer >= 60) {
        waitTimer = 0;
        int randomDir = rand() % 5; // Generates 0, 1, 2, 3, or 4

        switch (randomDir) {
            case 0: setVelocity(0, -1); break; // Walk Up
            case 1: setVelocity(0, 1); break;  // Walk Down
            case 2: setVelocity(-1, 0); break; // Walk Left
            case 3: setVelocity(1, 0); break;  // Walk Right
            case 4: setVelocity(0, 0); break;  // Stand still (Idle)
        }
    }

    // Call the parent class update() to actually move the coordinates and animate the legs!
    GameObject::update();
}