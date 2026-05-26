#pragma once
#include "Entity.h"

// ============================================================
//  Enemy  -  Chapter 19
// ============================================================
//
//  An Enemy IS-A Entity, but it differs in two ways: it moves
//  leftward across the screen (wrapping when it leaves), and
//  it renders in a fixed color of its own rather than using
//  the Animator's rainbow.
//
//  Note the design constraint. Chapter 18 explained that
//  without `virtual`, a method defined in a derived class
//  HIDES the base method rather than OVERRIDING it. That's
//  fine when we call methods through the derived type (which
//  is what main does in this project) — Enemy::update is
//  reached exactly as we'd want. The slicing problem from
//  Chapter 18 only bites when you mix Player and Enemy in a
//  base-typed container, which we deliberately avoid here.
//  Chapter 20 fixes that with `virtual` and Chapter 21's
//  project shows the polymorphic payoff.
// ============================================================

class Enemy : public Entity
{
public:
    Enemy(float startX, float y,
          float width, float height,
          float speed,            // pixels per second, leftward
          float screenWidth,      // for wrap-around
          SDL_Color baseColor);

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    float     speed_;
    float     screenWidth_;
    SDL_Color baseColor_;
};
