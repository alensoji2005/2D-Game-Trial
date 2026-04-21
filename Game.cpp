#include "Game.h"
#include "MainMenuState.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h> // NEW
#include <iostream>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), stateMachine(nullptr) {}
Game::~Game() {}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0);
        
        // Initialize libraries once for the whole engine
        IMG_Init(IMG_INIT_PNG);
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cout << "Mixer Error: " << Mix_GetError() << std::endl;
        }
        
        // --- NEW: Initialize Fonts ---
        if (TTF_Init() == -1) {
            std::cout << "TTF Error: " << TTF_GetError() << std::endl;
        }

        // Boot up the state machine and load the Main Menu
        stateMachine = new GameStateMachine();
        stateMachine->changeState(new MainMenuState());

        isRunning = true;
    } else {
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    // The only thing Game.cpp handles now is closing the window
    if (event.type == SDL_QUIT) {
        isRunning = false;
    }
}

void Game::update() {
    stateMachine->update();
}

void Game::render() {
    // The state machine tells the active state to render itself
    stateMachine->render();
}

void Game::clean() {
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit(); // NEW: Clean up fonts

    delete stateMachine;
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}