#include "PlayState.h"
#include "Game.h"
#include "SoundManager.h"
#include "UIManager.h"
#include <iostream>
#include <fstream> // NEW: For reading the entities file

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
    
    // --- NEW: Dynamic Entity Loading ---
    std::ifstream file("entities.txt");
    if (file.is_open()) {
        std::string type, texture;
        int startX, startY;
        
        // Read the file line by line (Type, Texture, X, Y)
        while (file >> type >> texture >> startX >> startY) {
            if (type == "NPC") {
                enemies.push_back(new NPC(texture.c_str(), game->getRenderer(), startX, startY));
            }
        }
        file.close();
    } else {
        std::cout << "Could not load entities.txt" << std::endl;
    }
    
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
    
    // Safely delete every enemy in our dynamic list
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear(); // Empty the list
    
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
        
        // Check sword hit against EVERY enemy in the list!
        for (auto enemy : enemies) {
            if (enemy->getIsActive() && checkCollision(attackBox, enemy->getCollider())) {
                enemy->takeDamage(10); // Deal 10 damage!
                if (bumpSound) SoundManager::playSound(bumpSound);
                std::cout << "HIT! Enemy Health: " << enemy->getHealth() << std::endl;
            }
        }
    }

    // 2. Update logic
    player->update();
    for (auto enemy : enemies) {
        if (enemy->getIsActive()) enemy->update();
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
    
    // Draw all enemies!
    for (auto enemy : enemies) {
        enemy->render(camera);
    }
    
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