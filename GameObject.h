#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>

class GameObject {
public:
    GameObject(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    virtual ~GameObject();

    virtual void update();
    void render(SDL_Rect camera);
    
    // New method to accept input commands
    void setVelocity(int dx, int dy);

    // Collision helper methods
    SDL_Rect getCollider();
    void revertMovement();
    
    // --- NEW: Combat Methods ---
    bool getIsActive() { return isActive; }
    int getHealth() { return health; }
    void takeDamage(int amount);
    SDL_Rect getAttackCollider(); // Gets the hitbox for a sword swing
    
    // Getters for camera tracking
    int getX() { return xpos; }
    int getY() { return ypos; }

protected:
    int xpos;
    int ypos;
    
    // New variables for movement math
    int xVel;
    int yVel;
    int speed;

    // Animation tracking variables
    int currentFrame;
    int currentRow;
    
    // --- NEW: Combat Variables ---
    int health;
    int maxHealth;
    bool isActive; // True if alive, False if defeated
    int lastDir;   // Tracks the direction we are facing (0=Down, 1=Left, 2=Right, 3=Up)

    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect; 
    SDL_Renderer* renderer;
};

#endif