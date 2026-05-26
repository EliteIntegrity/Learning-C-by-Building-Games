#pragma once
#include <SDL3/SDL.h>

class Game;

// ============================================================
//  GameState  -  abstract base for the state stack.
// ============================================================
//
//  Each state owns its own input handling and rendering. The
//  Game keeps a stack of these; events go to the top state
//  only; rendering walks bottom-to-top so modal states layer
//  cleanly over the gameplay underneath.
// ============================================================

struct InputResult
{
    bool handled  = false;   // did this state recognize the event?
    bool tookTurn = false;   // should the world advance one turn?
};

class GameState
{
public:
    virtual ~GameState() = default;

    GameState(const GameState&)            = delete;
    GameState& operator=(const GameState&) = delete;

    // Called once when this state becomes the top of the stack.
    virtual void onEnter(Game& /*game*/) {}
    // Called once when this state is popped.
    virtual void onExit (Game& /*game*/) {}

    virtual InputResult handleEvent(Game& game, const SDL_Event& ev) = 0;
    virtual void        render     (Game& game, SDL_Renderer* renderer) = 0;

    // If true, the state below us is rendered first (we draw on top).
    virtual bool transparent() const { return false; }

protected:
    GameState() = default;
};
