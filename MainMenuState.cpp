#include "MainMenuState.h"
#include "PlayState.h"
#include "Game.h"
#include <iostream>

const std::string MainMenuState::stateID = "MENU";

bool MainMenuState::onEnter() {
    std::cout << "Entering Main Menu. Press ENTER to play!" << std::endl;
    return true;
}

bool MainMenuState::onExit() {
    std::cout << "Exiting Main Menu" << std::endl;
    return true;
}

void MainMenuState::update() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // If the player presses ENTER, switch to the Play State!
    if (currentKeyStates[SDL_SCANCODE_RETURN]) { 
        game->getStateMachine()->changeState(new PlayState());
    }
}

void MainMenuState::render() {
    // For now, our menu is just a dark blue screen
    SDL_SetRenderDrawColor(game->getRenderer(), 10, 20, 50, 255);
    SDL_RenderClear(game->getRenderer());
    SDL_RenderPresent(game->getRenderer());
}