#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include "NPC.h"
#include "Map.h"
#include "Projectile.h" // NEW: Include our new class
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
    std::vector<Projectile*> projectiles; // NEW: Track active fireballs
    Map* map;
    
    Mix_Music* bgMusic;
    Mix_Chunk* bumpSound;
    
    // --- NEW: UI Variables ---
    TTF_Font* font;
    bool isDialogueActive;
    std::string currentDialogue;
    
    SDL_Rect camera;
    
    bool checkCollision(SDL_Rect a, SDL_Rect b);
};

#endif