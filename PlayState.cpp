#include "PlayState.h"
#include "Game.h"
#include "SoundManager.h"
#include "UIManager.h"
#include <iostream>
#include <fstream> 

const std::string PlayState::stateID = "PLAY";

// Cooldowns
int attackCooldown = 0;
int magicCooldown = 0; // NEW: Cooldown for the fireball

bool PlayState::checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w <= b.x) return false;
    if (a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y) return false;
    if (a.y >= b.y + b.h) return false;
    return true;
}

bool PlayState::onEnter() {
    std::cout << "Entering Play State" << std::endl;
    
    camera = {0, 0, 800, 600};
    
    map = new Map(game->getRenderer());
    map->LoadMap("level1.txt"); 
    
    player = new GameObject("player.png", game->getRenderer(), 350, 250);
    
    std::ifstream file("entities.txt");
    if (file.is_open()) {
        std::string type, texture;
        int startX, startY;
        
        while (file >> type >> texture >> startX >> startY) {
            if (type == "NPC") {
                enemies.push_back(new NPC(texture.c_str(), game->getRenderer(), startX, startY));
            }
        }
        file.close();
    } else {
        std::cout << "Could not load entities.txt" << std::endl;
    }
    
    bgMusic = SoundManager::loadMusic("bgm.mp3");
    bumpSound = SoundManager::loadSound("bump.wav");
    if (bgMusic) SoundManager::playMusic(bgMusic);

    font = UIManager::loadFont("font.ttf", 24);
    isDialogueActive = false;
    currentDialogue = "";

    return true;
}

bool PlayState::onExit() {
    std::cout << "Exiting Play State" << std::endl;
    
    delete player;
    
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear(); 
    
    // NEW: Safely delete all remaining projectiles
    for (auto p : projectiles) {
        delete p;
    }
    projectiles.clear();
    
    delete map;
    
    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(bumpSound);
    if (font) TTF_CloseFont(font);
    
    return true;
}

void PlayState::update() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (isDialogueActive) {
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            isDialogueActive = false; 
        }
        return; 
    }

    // 1. Handle Input locally
    int dx = 0; int dy = 0;
    if (currentKeyStates[SDL_SCANCODE_W]) dy = -1;
    if (currentKeyStates[SDL_SCANCODE_S]) dy = 1;
    if (currentKeyStates[SDL_SCANCODE_A]) dx = -1;
    if (currentKeyStates[SDL_SCANCODE_D]) dx = 1;
    player->setVelocity(dx, dy);

    // --- MELEE COMBAT (Press 'J') ---
    if (attackCooldown > 0) attackCooldown--; 
    
    if (currentKeyStates[SDL_SCANCODE_J] && attackCooldown == 0) {
        attackCooldown = 30; 
        
        SDL_Rect attackBox = player->getAttackCollider();
        
        for (auto enemy : enemies) {
            if (enemy->getIsActive() && checkCollision(attackBox, enemy->getCollider())) {
                enemy->takeDamage(10); 
                if (bumpSound) SoundManager::playSound(bumpSound);
                std::cout << "MELEE HIT! Enemy Health: " << enemy->getHealth() << std::endl;
            }
        }
    }

    // --- NEW: MAGIC COMBAT (Press 'K') ---
    if (magicCooldown > 0) magicCooldown--;
    
    if (currentKeyStates[SDL_SCANCODE_K] && magicCooldown == 0) {
        magicCooldown = 45; // 0.75 second cooldown for magic
        
        SDL_Rect pCol = player->getCollider();
        SDL_Rect aBox = player->getAttackCollider();
        int dirX = 0, dirY = 0;
        
        // Cleverly figure out which way the player is facing based on the attack box
        if (aBox.x < pCol.x) dirX = -1;
        else if (aBox.x > pCol.x) dirX = 1;
        else if (aBox.y < pCol.y) dirY = -1;
        else if (aBox.y > pCol.y) dirY = 1;
        
        // Spawn a fireball slightly offset to come out of the center of the player
        projectiles.push_back(new Projectile(game->getRenderer(), pCol.x + 8, pCol.y + 8, dirX, dirY));
        if (bumpSound) SoundManager::playSound(bumpSound); // Reuse sound for shooting
    }

    // 2. Update logic for Player, Enemies, and Projectiles
    player->update();
    for (auto enemy : enemies) {
        if (enemy->getIsActive()) enemy->update();
    }
    
    // --- NEW: PROJECTILE LOGIC & MEMORY CLEANUP ---
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        Projectile* p = *it;
        if (p->getIsActive()) {
            p->update();
            
            // Check Projectile Wall Collision
            SDL_Rect pCol = p->getCollider();
            if (map->isSolid(pCol.x, pCol.y) || map->isSolid(pCol.x + pCol.w, pCol.y) || 
                map->isSolid(pCol.x, pCol.y + pCol.h) || map->isSolid(pCol.x + pCol.w, pCol.y + pCol.h)) {
                p->destroy(); // Hit a wall
            }
            
            // Check Projectile Enemy Collision
            for (auto enemy : enemies) {
                if (enemy->getIsActive() && checkCollision(pCol, enemy->getCollider())) {
                    enemy->takeDamage(15); // Magic does 15 damage!
                    p->destroy(); // Hit an enemy
                    if (bumpSound) SoundManager::playSound(bumpSound);
                    std::cout << "MAGIC HIT! Enemy Health: " << enemy->getHealth() << std::endl;
                }
            }
            ++it;
        } else {
            // Safely delete destroyed projectiles from memory
            delete p;
            it = projectiles.erase(it);
        }
    }

    // 3. Wall Collisions (Player)
    SDL_Rect col = player->getCollider();
    if (map->isSolid(col.x, col.y) || map->isSolid(col.x + col.w, col.y) || 
        map->isSolid(col.x, col.y + col.h) || map->isSolid(col.x + col.w, col.y + col.h)) {
        player->revertMovement();
    }

    // 4. Wall Collisions (NPC)
    for (auto enemy : enemies) {
        if (enemy->getIsActive()) {
            SDL_Rect npcCol = enemy->getCollider();
            if (map->isSolid(npcCol.x, npcCol.y) || map->isSolid(npcCol.x + npcCol.w, npcCol.y) || 
                map->isSolid(npcCol.x, npcCol.y + npcCol.h) || map->isSolid(npcCol.x + npcCol.w, npcCol.y + npcCol.h)) {
                enemy->revertMovement();
                enemy->setVelocity(0, 0);
            }
        }
    }

    // 5. Entity Collisions & Dialogue Trigger
    for (auto enemy : enemies) {
        if (enemy->getIsActive() && checkCollision(player->getCollider(), enemy->getCollider())) {
            player->revertMovement();
            enemy->revertMovement();
            enemy->setVelocity(0, 0);
            
            isDialogueActive = true;
            currentDialogue = "NPC: Ouch! Stop pushing me!";
        }
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
    
    for (auto enemy : enemies) {
        if (enemy->getIsActive()) enemy->render(camera);
    }
    
    // --- NEW: Draw all active projectiles ---
    for (auto p : projectiles) {
        p->render(camera);
    }
    
    player->render(camera);
    
    SDL_Color hpColor = {255, 50, 50, 255}; 
    UIManager::drawText(game->getRenderer(), font, "Player HP: " + std::to_string(player->getHealth()), 20, 20, hpColor);

    if (isDialogueActive) {
        UIManager::drawTextBox(game->getRenderer(), font, currentDialogue);
    }
    
    SDL_RenderPresent(game->getRenderer());
}