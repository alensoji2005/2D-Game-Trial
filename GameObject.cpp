#include "GameObject.h"
#include "TextureManager.h"
#include <iostream>

GameObject::GameObject(const char* textureSheet, SDL_Renderer* ren, int x, int y) {
    renderer = ren;
    objTexture = TextureManager::LoadTexture(textureSheet, renderer);

    xpos = x;
    ypos = y;
    xVel = 0;
    yVel = 0;
    speed = 4;

    currentFrame = 1;
    currentRow = 0;
    
    // Base Stats
    level = 1;
    experience = 0;
    nextLevelExp = 100;
    attackDamage = 10;
    
    maxHealth = 30;
    health = maxHealth;
    isActive = true;
    rewardedXP = false;
    lastDir = 0; 
    
    isAttacking = false;
    attackTimer = 0;
}

GameObject::~GameObject() {
    SDL_DestroyTexture(objTexture);
}

void GameObject::update() {
    if (!isAttacking) {
        xpos += xVel * speed;
        ypos += yVel * speed;
    }

    if (isAttacking) {
        attackTimer--;
        if (attackTimer <= 0) {
            isAttacking = false;
        }
        if (attackTimer > 10) currentFrame = 3;
        else if (attackTimer > 5) currentFrame = 4;
        else currentFrame = 5;
    } else {
        if (xVel == 0 && yVel == 0) {
            currentFrame = 1; 
        } else {
            currentFrame = (SDL_GetTicks() / 150) % 3;
            if (yVel == 1) { currentRow = 0; lastDir = 0; }
            else if (xVel == -1) { currentRow = 1; lastDir = 1; }
            else if (xVel == 1) { currentRow = 2; lastDir = 2; }
            else if (yVel == -1) { currentRow = 3; lastDir = 3; }
        }
    }

    srcRect.h = 32;
    srcRect.w = 32;
    srcRect.x = currentFrame * 32;
    srcRect.y = currentRow * 32;

    destRect.w = 32;
    destRect.h = 32;
}

void GameObject::render(SDL_Rect camera) {
    destRect.x = xpos - camera.x;
    destRect.y = ypos - camera.y;

    if (isActive) {
        SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);
    }
}

void GameObject::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        isActive = false; 
    }
}

// --- NEW: Level Up Logic ---
void GameObject::addExperience(int amount) {
    experience += amount;
    std::cout << "Gained " << amount << " XP! Total: " << experience << "/" << nextLevelExp << std::endl;

    if (experience >= nextLevelExp) {
        level++;
        experience -= nextLevelExp;
        nextLevelExp = (int)(nextLevelExp * 1.5); // Increase required XP for next level
        
        // Power up stats!
        maxHealth += 10;
        health = maxHealth; // Heal on level up
        attackDamage += 5;
        
        std::cout << "LEVEL UP! Now Level " << level << "!" << std::endl;
        std::cout << "Stats increased: HP: " << maxHealth << " DMG: " << attackDamage << std::endl;
    }
}

SDL_Rect GameObject::getCollider() {
    SDL_Rect col = {xpos + 8, ypos + 8, 16, 24};
    return col;
}

void GameObject::revertMovement() {
    xpos -= xVel * speed;
    ypos -= yVel * speed;
}

SDL_Rect GameObject::getAttackCollider() {
    SDL_Rect col = getCollider();
    SDL_Rect attackBox = {col.x, col.y, 32, 32}; 
    if (lastDir == 0) attackBox.y += 32;      
    else if (lastDir == 1) attackBox.x -= 32; 
    else if (lastDir == 2) attackBox.x += 32; 
    else if (lastDir == 3) attackBox.y -= 32; 
    return attackBox;
}

void GameObject::triggerAttack() {
    isAttacking = true;
    attackTimer = 15;
}