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
    
    // --- RPG Progression Methods ---
    void addExperience(int amount);
    int getLevel() { return level; }
    int getExperience() { return experience; }
    int getNextLevelExp() { return nextLevelExp; }
    int getAttackDamage() { return attackDamage; }
    bool hasRewardedXP() { return rewardedXP; }
    void markXPRewarded() { rewardedXP = true; }
    
    // Animation Trigger Methods
    void triggerAttack();
    bool getIsAttacking() { return isAttacking; }
    
    // Getters and Setters for camera tracking and map transitions
    int getX() { return xpos; }
    int getY() { return ypos; }
    void setPosition(int x, int y) { xpos = x; ypos = y; }

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
    
    // RPG Stats
    int level;
    int experience;
    int nextLevelExp;
    int attackDamage;
    bool rewardedXP; 

    // Attack Animation State Variables
    bool isAttacking;
    int attackTimer;

    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect; 
    SDL_Renderer* renderer;
};

#endif