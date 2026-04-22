#include "NPC.h"
#include <SDL2/SDL.h>
#include <cmath> // NEW: Included for calculating distance

NPC::NPC(const char* textureSheet, SDL_Renderer* renderer, int x, int y) 
    : GameObject(textureSheet, renderer, x, y) {
    speed = 2; // NPCs are slightly slower than the player (who moves at 4)
    waitTimer = 0;
    aggroRadius = 250; // Detect the player if they are within 250 pixels!
}

void NPC::update(SDL_Rect playerCol) {
    SDL_Rect myCol = getCollider();
    
    // --- NEW: Calculate the distance to the player ---
    int distX = playerCol.x - myCol.x;
    int distY = playerCol.y - myCol.y;
    
    // Pythagorean theorem to find the true distance
    float distance = std::sqrt(distX * distX + distY * distY);

    if (distance < aggroRadius) {
        // CHASE STATE: The player is close, move toward them!
        int dx = 0, dy = 0;
        
        // Add a tiny buffer (2 pixels) so they don't jitter when perfectly aligned
        if (std::abs(distX) > 2) dx = (distX > 0) ? 1 : -1;
        if (std::abs(distY) > 2) dy = (distY > 0) ? 1 : -1;
        
        setVelocity(dx, dy);
    } else {
        // WANDER STATE: The player is far away, wander randomly
        waitTimer++;
        if (waitTimer >= 60) {
            waitTimer = 0;
            int randomDir = rand() % 5; 

            switch (randomDir) {
                case 0: setVelocity(0, -1); break; // Up
                case 1: setVelocity(0, 1); break;  // Down
                case 2: setVelocity(-1, 0); break; // Left
                case 3: setVelocity(1, 0); break;  // Right
                case 4: setVelocity(0, 0); break;  // Idle
            }
        }
    }

    // Call the parent class to actually apply the velocity and animate!
    GameObject::update();
}