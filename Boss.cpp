#include "Boss.h"
#include <cmath>
#include <cstdlib>

Boss::Boss(const char* textureSheet, SDL_Renderer* ren, int x, int y) 
    : GameObject(textureSheet, ren, x, y) {
    
    maxHealth = 200;
    health = maxHealth;
    speed = 2; 
    
    shootTimer = 60;
    moveTimer = 0;
}

void Boss::update(SDL_Rect playerCol, std::vector<Projectile*>& enemyProjectiles) {
    SDL_Rect myCol = getCollider();
    
    int distX = playerCol.x - myCol.x;
    int distY = playerCol.y - myCol.y;
    float distance = std::sqrt(distX * distX + distY * distY);

    // 1. AI Logic
    if (distance < 500) { 
        moveTimer++;
        if (moveTimer > 60) {
            moveTimer = 0;
            setVelocity((rand() % 3) - 1, (rand() % 3) - 1);
        }
        
        shootTimer--;
        if (shootTimer <= 0) {
            shootTimer = 90; 
            int dirX = (distX > 0) ? 1 : (distX < 0) ? -1 : 0;
            int dirY = (distY > 0) ? 1 : (distY < 0) ? -1 : 0;
            
            // Shoot from the center of the giant 128x128 boss sprite
            enemyProjectiles.push_back(new Projectile(renderer, xpos + 64, ypos + 64, dirX, dirY));
        }
    } else {
        setVelocity(0, 0);
    }

    // 2. Physical Movement
    xpos += xVel * speed;
    ypos += yVel * speed;

    // 3. Animation Frame Logic
    if (xVel == 0 && yVel == 0) {
        currentFrame = 1; // Idle frame
    } else {
        currentFrame = (SDL_GetTicks() / 150) % 3;
        
        // Directions (Down=0, Left=1, Right=2, Up=3)
        if (yVel > 0) { currentRow = 0; lastDir = 0; }
        else if (xVel < 0) { currentRow = 1; lastDir = 1; }
        else if (xVel > 0) { currentRow = 2; lastDir = 2; }
        else if (yVel < 0) { currentRow = 3; lastDir = 3; }
    }

    // 4. --- DYNAMIC SPRITE SIZING (THE FIX!) ---
    // Ask the GPU exactly how big your custom Boss image is
    int texW = 0, texH = 0;
    SDL_QueryTexture(objTexture, NULL, NULL, &texW, &texH);
    
    // Since your spritesheet has 3 columns and 4 rows, divide to get the exact frame sizes
    int frameW = texW / 3;
    int frameH = texH / 4;

    srcRect.w = frameW;
    srcRect.h = frameH;
    srcRect.x = currentFrame * frameW;
    srcRect.y = currentRow * frameH;
    
    // Render it HUGE on screen
    destRect.w = 128; 
    destRect.h = 128;
}

void Boss::render(SDL_Rect camera) {
    destRect.x = xpos - camera.x;
    destRect.y = ypos - camera.y;
    
    if (isActive) {
        SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);
    }
}