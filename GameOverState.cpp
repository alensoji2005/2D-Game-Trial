#include "GameOverState.h"
#include "MainMenuState.h"
#include "Game.h"
#include "UIManager.h"
#include <iostream>

const std::string GameOverState::stateID = "GAMEOVER";

bool GameOverState::onEnter() {
    std::cout << "Entering Game Over State" << std::endl;
    font = UIManager::loadFont("font.ttf", 48); // Load a large font for the game over text
    return true;
}

bool GameOverState::onExit() {
    std::cout << "Exiting Game Over State" << std::endl;
    if (font) TTF_CloseFont(font);
    return true;
}

void GameOverState::update() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // If the player presses ENTER, restart the game by going back to the Main Menu
    if (currentKeyStates[SDL_SCANCODE_RETURN]) { 
        game->getStateMachine()->changeState(new MainMenuState());
    }
}

void GameOverState::render() {
    // Fill the screen with a dark, bloody red
    SDL_SetRenderDrawColor(game->getRenderer(), 150, 0, 0, 255);
    SDL_RenderClear(game->getRenderer());
    
    SDL_Color textColor = {255, 255, 255, 255};
    
    // Draw the main text
    UIManager::drawText(game->getRenderer(), font, "YOU DIED", 300, 200, textColor);
    
    // Draw the instructions using a temporarily smaller font size
    TTF_Font* smallFont = UIManager::loadFont("font.ttf", 24);
    UIManager::drawText(game->getRenderer(), smallFont, "Press ENTER to return to Menu", 220, 300, textColor);
    TTF_CloseFont(smallFont);

    SDL_RenderPresent(game->getRenderer());
}