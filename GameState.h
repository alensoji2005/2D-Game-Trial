#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>

class GameState {
public:
    virtual ~GameState() {}
    
    // Core loop methods
    virtual void update() = 0;
    virtual void render() = 0;
    
    // Lifecycle methods (called when the state is loaded or destroyed)
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;
    
    // Used to prevent loading the exact same state twice in a row
    virtual std::string getStateID() const = 0;
};

#endif