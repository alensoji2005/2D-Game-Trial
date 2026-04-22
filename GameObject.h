#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>

class GameObject {
public:
    GameObject(const char* textureSheet, SDL_Renderer* renderer, int x, int y);
    ~GameObject();

    void update();
    void render(SDL_Rect camera);

    // Collision helper
    SDL_Rect getCollider();
    void revertMovement();
    
    // Combat & Stats
    bool getIsActive() { return isActive; }
    int getHealth() { return health; }
    void takeDamage(int amount);
    SDL_Rect getAttackCollider(); 
    
    // --- NEW: RPG Progression Methods ---
    void addExperience(int amount);
    int getLevel() { return level; }
    int getExperience() { return experience; }
    int getNextLevelExp() { return nextLevelExp; }
    int getAttackDamage() { return attackDamage; }
    bool hasRewardedXP() { return rewardedXP; }
    void markXPRewarded() { rewardedXP = true; }
    
    void triggerAttack();
    bool getIsAttacking() { return isAttacking; }
    
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
    
    int currentFrame;
    int currentRow;
    
    int health;
    int maxHealth;
    bool isActive; 
    int lastDir;   
    
    // --- NEW: RPG Stats ---
    int level;
    int experience;
    int nextLevelExp;
    int attackDamage;
    bool rewardedXP; // Prevents double-claiming XP from a dead enemy

    bool isAttacking;
    int attackTimer;

    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect; 
    SDL_Renderer* renderer;
};

#endif