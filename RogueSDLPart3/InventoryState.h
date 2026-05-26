#pragma once
#include "GameState.h"

class InventoryState : public GameState
{
public:
    InputResult handleEvent(Game& game, const SDL_Event& ev) override;
    void        render     (Game& game, SDL_Renderer* renderer) override;
    bool        transparent() const override { return true; }
};
