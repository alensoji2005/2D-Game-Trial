#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "GameStateMachine.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }
    
    // Getters for our states to use
    GameStateMachine* getStateMachine() { return stateMachine; }
    SDL_Renderer* getRenderer() { return renderer; }

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // The core manager of the game
    GameStateMachine* stateMachine;
};

// EXTREMELY IMPORTANT: This allows any file to access the global "game" instance
extern Game* game;

#endif /* GAME_H */