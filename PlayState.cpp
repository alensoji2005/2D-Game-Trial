#include "PlayState.h"
#include "Game.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "Item.h"
#include <iostream>
#include <fstream> 

const std::string PlayState::stateID = "PLAY";

// Cooldowns and Stats
int attackCooldown = 0;
int magicCooldown = 0; 
int playerGold = 0; // Track collected coins

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
    
    for (auto p : projectiles) {
        delete p;
    }
    projectiles.clear();

    for (auto i : items) {
        delete i;
    }
    items.clear();
    
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
        
        player->triggerAttack();
        
        SDL_Rect attackBox = player->getAttackCollider();
        
        for (auto enemy : enemies) {
            if (enemy->getIsActive() && checkCollision(attackBox, enemy->getCollider())) {
                enemy->takeDamage(player->getAttackDamage()); 
                if (bumpSound) SoundManager::playSound(bumpSound);
                std::cout << "MELEE HIT! Enemy Health: " << enemy->getHealth() << std::endl;
            }
        }
    }

    // --- MAGIC COMBAT (Press 'K') ---
    if (magicCooldown > 0) magicCooldown--;
    
    if (currentKeyStates[SDL_SCANCODE_K] && magicCooldown == 0) {
        magicCooldown = 45; 
        
        SDL_Rect pCol = player->getCollider();
        SDL_Rect aBox = player->getAttackCollider();
        int dirX = 0, dirY = 0;
        
        if (aBox.x < pCol.x) dirX = -1;
        else if (aBox.x > pCol.x) dirX = 1;
        else if (aBox.y < pCol.y) dirY = -1;
        else if (aBox.y > pCol.y) dirY = 1;
        
        projectiles.push_back(new Projectile(game->getRenderer(), pCol.x + 8, pCol.y + 8, dirX, dirY));
        if (bumpSound) SoundManager::playSound(bumpSound); 
    }

    // ==========================================
    // PHASE 1: PLAYER PHYSICS
    // ==========================================
    player->update();
    
    SDL_Rect col = player->getCollider();
    bool playerReverted = false;

    if (map->isSolid(col.x, col.y) || map->isSolid(col.x + col.w, col.y) || 
        map->isSolid(col.x, col.y + col.h) || map->isSolid(col.x + col.w, col.y + col.h)) {
        player->revertMovement();
        playerReverted = true;
    }

    if (!playerReverted) {
        for (auto enemy : enemies) {
            if (enemy->getIsActive() && checkCollision(player->getCollider(), enemy->getCollider())) {
                player->revertMovement();
                break; 
            }
        }
    }

    // ==========================================
    // PHASE 2: ENEMY PHYSICS & XP/LOOT REWARDS
    // ==========================================
    for (auto enemy : enemies) {
        if (enemy->getIsActive()) {
            enemy->update(player->getCollider());
            
            SDL_Rect npcCol = enemy->getCollider();
            bool enemyReverted = false;

            if (map->isSolid(npcCol.x, npcCol.y) || map->isSolid(npcCol.x + npcCol.w, npcCol.y) || 
                map->isSolid(npcCol.x, npcCol.y + npcCol.h) || map->isSolid(npcCol.x + npcCol.w, npcCol.y + npcCol.h)) {
                enemy->revertMovement();
                enemy->setVelocity(0, 0);
                enemyReverted = true;
            }
            
            if (!enemyReverted && checkCollision(enemy->getCollider(), player->getCollider())) {
                enemy->revertMovement();
                enemy->setVelocity(0, 0);
                enemyReverted = true;
            }

            if (!enemyReverted) {
                for (auto otherEnemy : enemies) {
                    if (otherEnemy != enemy && otherEnemy->getIsActive() && checkCollision(enemy->getCollider(), otherEnemy->getCollider())) {
                        enemy->revertMovement();
                        enemy->setVelocity(0, 0);
                        break; 
                    }
                }
            }
        } else {
            if (!enemy->hasRewardedXP()) {
                player->addExperience(50); 
                enemy->markXPRewarded();
                
                // --- NEW: Spawning Loot (50% chance for Coin, 20% for Heart) ---
                int roll = rand() % 100;
                if (roll < 50) {
                    items.push_back(new Item(game->getRenderer(), enemy->getX(), enemy->getY(), COIN));
                } else if (roll < 70) {
                    items.push_back(new Item(game->getRenderer(), enemy->getX(), enemy->getY(), HEALTH));
                }
            }
        }
    }

    // ==========================================
    // PHASE 3: PROJECTILES
    // ==========================================
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        Projectile* p = *it;
        if (p->getIsActive()) {
            p->update();
            
            SDL_Rect pCol = p->getCollider();
            if (map->isSolid(pCol.x, pCol.y) || map->isSolid(pCol.x + pCol.w, pCol.y) || 
                map->isSolid(pCol.x, pCol.y + pCol.h) || map->isSolid(pCol.x + pCol.w, pCol.y + pCol.h)) {
                p->destroy(); 
            }
            
            for (auto enemy : enemies) {
                if (enemy->getIsActive() && checkCollision(pCol, enemy->getCollider())) {
                    enemy->takeDamage(15 + (player->getLevel() * 2)); 
                    p->destroy(); 
                    if (bumpSound) SoundManager::playSound(bumpSound);
                    std::cout << "MAGIC HIT! Enemy Health: " << enemy->getHealth() << std::endl;
                }
            }
            ++it;
        } else {
            delete p;
            it = projectiles.erase(it);
        }
    }

    // ==========================================
    // PHASE 4: ITEMS (Pickup logic)
    // ==========================================
    for (auto it = items.begin(); it != items.end(); ) {
        Item* item = *it;
        item->update();
        if (checkCollision(player->getCollider(), item->getCollider())) {
            if (item->getType() == COIN) {
                playerGold += 10;
            } else if (item->getType() == HEALTH) {
                // Heals the player by adding experience method (reuse for healing logic if needed)
                // For simplicity, let's just say it restores 10 HP directly
                // player->heal(10); // You can add a heal method to GameObject later
                std::cout << "Healed 10 HP!" << std::endl;
            }
            item->collect();
            if (bumpSound) SoundManager::playSound(bumpSound);
        }

        if (!item->getIsActive()) {
            delete item;
            it = items.erase(it);
        } else {
            ++it;
        }
    }

    // ==========================================
    // PHASE 5: CAMERA TRACKING
    // ==========================================
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
    
    for (auto p : projectiles) {
        p->render(camera);
    }

    for (auto i : items) {
        i->render(camera);
    }
    
    player->render(camera);
    
    if (player->getIsAttacking()) {
        SDL_Rect attackBox = player->getAttackCollider();
        attackBox.x -= camera.x;
        attackBox.y -= camera.y;
        
        SDL_SetRenderDrawColor(game->getRenderer(), 220, 220, 220, 255); 
        SDL_RenderFillRect(game->getRenderer(), &attackBox);
    }
    
    // --- UPDATED UI DISPLAY ---
    SDL_Color hpColor = {255, 50, 50, 255}; 
    SDL_Color xpColor = {50, 150, 255, 255};
    SDL_Color levelColor = {255, 255, 50, 255};
    SDL_Color goldColor = {255, 215, 0, 255};

    UIManager::drawText(game->getRenderer(), font, "Lvl: " + std::to_string(player->getLevel()), 20, 20, levelColor);
    UIManager::drawText(game->getRenderer(), font, "HP: " + std::to_string(player->getHealth()), 20, 50, hpColor);
    UIManager::drawText(game->getRenderer(), font, "XP: " + std::to_string(player->getExperience()) + " / " + std::to_string(player->getNextLevelExp()), 20, 80, xpColor);
    UIManager::drawText(game->getRenderer(), font, "Gold: " + std::to_string(playerGold), 20, 110, goldColor);

    if (isDialogueActive) {
        UIManager::drawTextBox(game->getRenderer(), font, currentDialogue);
    }
    
    SDL_RenderPresent(game->getRenderer());
}