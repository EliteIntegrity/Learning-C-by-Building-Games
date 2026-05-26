#pragma once
#include "GameState.h"
#include "Common.h"

enum class TargetAction
{
    Fireball     // consumes one Fireball scroll from inventory[slot]
};

class TargetingState : public GameState
{
public:
    TargetingState(TargetAction action, int consumeSlot, Point startCursor);

    void onEnter(Game& game) override;
    InputResult handleEvent(Game& game, const SDL_Event& ev) override;
    void        render     (Game& game, SDL_Renderer* renderer) override;
    bool        transparent() const override { return true; }

private:
    TargetAction action_;
    int          consumeSlot_;
    Point        cursor_;
};
