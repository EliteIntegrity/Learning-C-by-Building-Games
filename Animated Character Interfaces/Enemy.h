#pragma once
#include "Entity.h"

// ============================================================
//  Enemy  -  Chapter 21
// ============================================================
//
//  Same Enemy as Chapter 19, but now its update() and render()
//  are PROPER OVERRIDES (the base methods are virtual). That
//  means calls through an Entity* / IUpdatable* / IDrawable*
//  pointer all reach Enemy's versions correctly. The slicing
//  problem we worked around in Chapter 19 is fixed.
// ============================================================

class Enemy : public Entity
{
public:
    Enemy(float startX, float y,
          float width, float height,
          float speed,
          float screenWidth,
          SDL_Color baseColor);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

private:
    float     speed_;
    float     screenWidth_;
    SDL_Color baseColor_;
};
