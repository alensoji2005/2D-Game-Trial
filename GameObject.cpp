#include "GameObject.h"
#include "TextureManager.h"

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
    
    health = 30;
    maxHealth = 30;
    isActive = true;
    lastDir = 0; 
    
    // --- NEW: Initialize attack state ---
    isAttacking = false;
    attackTimer = 0;
}

GameObject::~GameObject() {
    SDL_DestroyTexture(objTexture);
}

void GameObject::update() {
    // Only apply movement if we aren't currently frozen mid-attack!
    if (!isAttacking) {
        xpos += xVel * speed;
        ypos += yVel * speed;
    }

    // --- ANIMATION & DIRECTION LOGIC ---
    if (isAttacking) {
        attackTimer--;
        if (attackTimer <= 0) {
            isAttacking = false; // Attack finished!
        }
        
        // Cycle through frames 3, 4, and 5 for the sword swing
        if (attackTimer > 10) currentFrame = 3;
        else if (attackTimer > 5) currentFrame = 4;
        else currentFrame = 5;
        
    } else {
        // Normal movement animation (Frames 0, 1, 2)
        if (xVel == 0 && yVel == 0) {
            currentFrame = 1; 
        } else {
            currentFrame = (SDL_GetTicks() / 150) % 3;
            
            if (yVel == 1) { currentRow = 0; lastDir = 0; }       // Down
            else if (xVel == -1) { currentRow = 1; lastDir = 1; } // Left
            else if (xVel == 1) { currentRow = 2; lastDir = 2; }  // Right
            else if (yVel == -1) { currentRow = 3; lastDir = 3; } // Up
        }
    }

    srcRect.h = 32;
    srcRect.w = 32;
    srcRect.x = currentFrame * 32;
    srcRect.y = currentRow * 32;

    destRect.w = 32;
    destRect.h = 32;
    // Removed destRect.x and destRect.y from update() because camera is not in scope here.
    // Screen positioning is correctly handled in the render() function below.
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

// --- NEW: Trigger the animation state ---
void GameObject::triggerAttack() {
    isAttacking = true;
    attackTimer = 15; // The animation lasts for 15 frames (1/4 of a second)
}