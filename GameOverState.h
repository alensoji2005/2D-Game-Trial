#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "GameState.h"
#include <SDL2/SDL_ttf.h>

class GameOverState : public GameState {
public:
    virtual void update();
    virtual void render();
    virtual bool onEnter();
    virtual bool onExit();
    virtual std::string getStateID() const { return stateID; }
private:
    static const std::string stateID;
    TTF_Font* font;
};

#endif