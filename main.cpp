#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[]) {

    // Cap at 60 FPS
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    game = new Game();
    
    // Initialize window: Title, X, Y, Width, Height, Fullscreen
    game->init("Action Adventure RPG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

    while (game->running()) {
        frameStart = SDL_GetTicks(); // How many ms since SDL initialized

        game->handleEvents();
        game->update();
        game->render();

        frameTime = SDL_GetTicks() - frameStart; // How long this frame took

        // If the frame processed faster than 1/60th of a second, delay the engine
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game->clean();
    delete game;

    return 0;
}