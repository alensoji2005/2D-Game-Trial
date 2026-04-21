#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject(const char* textureSheet, SDL_Renderer* ren, int x, int y) {
    renderer = ren;
    objTexture = TextureManager::LoadTexture(textureSheet, renderer);
    xpos = x;
    ypos = y;
    
    // Initialize movement variables
    xVel = 0;
    yVel = 0;
    speed = 4; // The character will move 4 pixels per frame

    // Initialize animation to idle facing down
    currentFrame = 1;
    currentRow = 0;
    
    // --- NEW: Initialize Combat Stats ---
    health = 30;
    maxHealth = 30;
    isActive = true;
    lastDir = 0; // Default facing down
}

GameObject::~GameObject() {
    SDL_DestroyTexture(objTexture);
}

// Update velocity based on input
void GameObject::setVelocity(int dx, int dy) {
    xVel = dx;
    yVel = dy;
}

void GameObject::revertMovement() {
    // Undo the movement if we hit a wall
    xpos -= xVel * speed;
    ypos -= yVel * speed;
}

SDL_Rect GameObject::getCollider() {
    SDL_Rect col;
    // Shrink the hitbox so it only covers the character's body/feet
    // Our visual sprite is 64x64, let's make the physical hitbox 32x32 at the bottom
    col.x = xpos + 16; 
    col.y = ypos + 32; 
    col.w = 32;
    col.h = 32;
    return col;
}

void GameObject::update() {
    // Apply velocity to position
    xpos += xVel * speed;
    ypos += yVel * speed;

    // --- ANIMATION & DIRECTION LOGIC ---
    if (xVel == 0 && yVel == 0) {
        currentFrame = 1; 
    } else {
        currentFrame = (SDL_GetTicks() / 150) % 3;
        
        if (yVel == 1) { currentRow = 0; lastDir = 0; }       // Down
        else if (xVel == -1) { currentRow = 1; lastDir = 1; } // Left
        else if (xVel == 1) { currentRow = 2; lastDir = 2; }  // Right
        else if (yVel == -1) { currentRow = 3; lastDir = 3; } // Up
    }

    // Source rect grabs a single 32x32 frame from the sprite sheet
    srcRect.w = 32; 
    srcRect.h = 32;
    srcRect.x = currentFrame * 32;
    srcRect.y = currentRow * 32;

    destRect.w = srcRect.w * 2; 
    destRect.h = srcRect.h * 2;
}

void GameObject::render(SDL_Rect camera) {
    // Offset the destination rectangle by the camera's position
    destRect.x = xpos - camera.x;
    destRect.y = ypos - camera.y;

    // Only draw the object if it is alive!
    if (isActive) {
        SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);
    }
}

// --- NEW: Combat Implementations ---
void GameObject::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        isActive = false; // The object is defeated!
    }
}

SDL_Rect GameObject::getAttackCollider() {
    SDL_Rect col = getCollider();
    SDL_Rect attackBox = {col.x, col.y, 32, 32}; // Create a 32x32 hitbox
    
    // Push the hitbox out in front of the player based on the direction they are facing
    if (lastDir == 0) attackBox.y += 32;      // Attacking Down
    else if (lastDir == 1) attackBox.x -= 32; // Attacking Left
    else if (lastDir == 2) attackBox.x += 32; // Attacking Right
    else if (lastDir == 3) attackBox.y -= 32; // Attacking Up
    
    return attackBox;
}