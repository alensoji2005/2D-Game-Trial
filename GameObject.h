#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>

class GameObject {
public:
    GameObject(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    ~GameObject();

    void update();
    void render(SDL_Rect camera);


    // Collision helper methods
    SDL_Rect getCollider();
    void revertMovement();
    
    // Combat Methods
    bool getIsActive() { return isActive; }
    int getHealth() { return health; }
    void takeDamage(int amount);
    SDL_Rect getAttackCollider(); 
    
    // --- NEW: Animation Trigger Methods ---
    void triggerAttack();
    bool getIsAttacking() { return isAttacking; }
    
    // Getters for camera tracking
    int getX() { return xpos; }
    int getY() { return ypos; }

    void setVelocity(int dx, int dy) {
        xVel = dx;
        yVel = dy;
    }

protected:
    int xpos, ypos;
    int xVel, yVel;
    int speed;

    // Animation tracking variables
    int currentFrame;
    int currentRow;
    
    // Combat Variables
    int health;
    int maxHealth;
    bool isActive; 
    int lastDir;   
    
    // --- NEW: Attack Animation State Variables ---
    bool isAttacking;
    int attackTimer;

    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect; 
    SDL_Renderer* renderer;
};

#endif