#ifndef GAMESTATEMACHINE_H
#define GAMESTATEMACHINE_H

#include "GameState.h"
#include <vector>

class GameStateMachine {
public:
    void pushState(GameState* state);
    void changeState(GameState* state);
    void popState();

    void update();
    void render();

private:
    std::vector<GameState*> gameStates;
};

#endif