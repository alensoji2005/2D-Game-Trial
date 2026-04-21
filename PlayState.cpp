#include "PlayState.h"
#include "Game.h"
#include "SoundManager.h"
#include "UIManager.h"
#include <iostream>

const std::string PlayState::stateID = "PLAY";

// We need a cooldown so you don't hit the enemy 60 times a second holding the button down
int attackCooldown = 0;

bool PlayState::checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w <= b.x) return false;
    if (a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y) return false;
    if (a.y >= b.y + b.h) return false;
    return true;
}

bool PlayState::onEnter() {
    std::cout << "Entering Play State" << std::endl;
    
    // Set up the camera
    camera = {0, 0, 800, 600};
    
    // Spawn our world
    map = new Map(game->getRenderer());
    map->LoadMap("level1.txt"); 
    
    player = new GameObject("player.png", game->getRenderer(), 350, 250);
    townGuard = new NPC("player.png", game->getRenderer(), 450, 350);
    
    // Load Audio
    bgMusic = SoundManager::loadMusic("bgm.mp3");
    bumpSound = SoundManager::loadSound("bump.wav");
    if (bgMusic) SoundManager::playMusic(bgMusic);

    // Load Fonts & Init Dialogue
    font = UIManager::loadFont("font.ttf", 24);
    isDialogueActive = false;
    currentDialogue = "";

    return true;
}

bool PlayState::onExit() {
    std::cout << "Exiting Play State" << std::endl;
    
    delete player;
    delete townGuard;
    delete map;
    
    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(bumpSound);
    if (font) TTF_CloseFont(font);
    
    return true;
}

void PlayState::update() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    // --- DIALOGUE FREEZE LOGIC ---
    if (isDialogueActive) {
        // If they are talking, block all movement and wait for SPACEBAR
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            isDialogueActive = false; // Dismiss the dialogue
        }
        return; // Skip the rest of the update loop!
    }

    // 1. Handle Input locally
    int dx = 0; int dy = 0;
    if (currentKeyStates[SDL_SCANCODE_W]) dy = -1;
    if (currentKeyStates[SDL_SCANCODE_S]) dy = 1;
    if (currentKeyStates[SDL_SCANCODE_A]) dx = -1;
    if (currentKeyStates[SDL_SCANCODE_D]) dx = 1;
    player->setVelocity(dx, dy);

    // --- NEW: COMBAT INPUT (Press 'J' to Attack) ---
    if (attackCooldown > 0) attackCooldown--; // Reduce cooldown timer
    
    if (currentKeyStates[SDL_SCANCODE_J] && attackCooldown == 0) {
        attackCooldown = 30; // Half a second cooldown before you can swing again
        
        SDL_Rect attackBox = player->getAttackCollider();
        
        // If the NPC is alive, and your sword hits them...
        if (townGuard->getIsActive() && checkCollision(attackBox, townGuard->getCollider())) {
            townGuard->takeDamage(10); // Deal 10 damage!
            if (bumpSound) SoundManager::playSound(bumpSound);
            std::cout << "HIT! Guard Health: " << townGuard->getHealth() << std::endl;
        }
    }

    // 2. Update logic
    player->update();
    if (townGuard->getIsActive()) townGuard->update();

    // 3. Wall Collisions (Player)
    SDL_Rect col = player->getCollider();
    if (map->isSolid(col.x, col.y) || map->isSolid(col.x + col.w, col.y) || 
        map->isSolid(col.x, col.y + col.h) || map->isSolid(col.x + col.w, col.y + col.h)) {
        player->revertMovement();
    }

    // 4. Wall Collisions (NPC)
    if (townGuard->getIsActive()) {
        SDL_Rect npcCol = townGuard->getCollider();
        if (map->isSolid(npcCol.x, npcCol.y) || map->isSolid(npcCol.x + npcCol.w, npcCol.y) || 
            map->isSolid(npcCol.x, npcCol.y + npcCol.h) || map->isSolid(npcCol.x + npcCol.w, npcCol.y + npcCol.h)) {
            townGuard->revertMovement();
            townGuard->setVelocity(0, 0);
        }
    }

    // 5. Entity Collisions & Dialogue Trigger
    // We only bump into the guard if they are still alive!
    if (townGuard->getIsActive() && checkCollision(player->getCollider(), townGuard->getCollider())) {
        player->revertMovement();
        townGuard->revertMovement();
        townGuard->setVelocity(0, 0);
        
        isDialogueActive = true;
        currentDialogue = "Town Guard: Ouch! Stop pushing me!";
    }

    // 6. Camera Tracking
    camera.x = player->getX() + 16 - 400; 
    camera.y = player->getY() + 16 - 300;
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > 960 - camera.w) camera.x = 960 - camera.w;
    if (camera.y > 800 - camera.h) camera.y = 800 - camera.h;
}

void PlayState::render() {
    SDL_SetRenderDrawColor(game->getRenderer(), 50, 150, 100, 255);
    SDL_RenderClear(game->getRenderer());
    
    map->DrawMap(camera);
    townGuard->render(camera);
    player->render(camera);
    
    // --- NEW: Draw the UI on top of everything else ---
    // Draw the player's Health Bar Text in the top left corner!
    SDL_Color hpColor = {255, 50, 50, 255}; // Red
    UIManager::drawText(game->getRenderer(), font, "Player HP: " + std::to_string(player->getHealth()), 20, 20, hpColor);

    if (isDialogueActive) {
        UIManager::drawTextBox(game->getRenderer(), font, currentDialogue);
    }
    
    SDL_RenderPresent(game->getRenderer());
}