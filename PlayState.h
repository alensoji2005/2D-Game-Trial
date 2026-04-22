#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include "NPC.h"
#include "Boss.h" // NEW: Include the Boss class
#include "Map.h"
#include "Projectile.h" 
#include "Item.h" 
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h> 

class PlayState : public GameState {
public:
    virtual void update();
    virtual void render();
    virtual bool onEnter();
    virtual bool onExit();
    virtual std::string getStateID() const { return stateID; }
    
private:
    static const std::string stateID;
    
    GameObject* player;
    std::vector<NPC*> enemies;
    std::vector<Boss*> bosses; // NEW: Track bosses
    
    std::vector<Projectile*> projectiles; 
    std::vector<Projectile*> enemyProjectiles; // NEW: Track fireballs shot BY enemies
    
    std::vector<Item*> items; 
    Map* map;
    
    Mix_Music* bgMusic;
    Mix_Chunk* bumpSound;
    
    // --- UI Variables ---
    TTF_Font* font;
    bool isDialogueActive;
    std::string currentDialogue;
    
    SDL_Rect camera;
    
    // --- Map Transition Variables ---
    int currentLevel;
    void loadLevel(int levelNumber);
    
    bool checkCollision(SDL_Rect a, SDL_Rect b);
};

#endif