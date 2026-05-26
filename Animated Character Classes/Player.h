#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"

// ============================================================
//  Player  -  Chapter 17
// ============================================================
//
//  A character that lives at a fixed position on the screen
//  and renders as a colored rectangle whose tint cycles via
//  its internal Animator.
//
//  Composition: Player HAS-A Animator. The Animator is stored
//  by value as a member, so it is created automatically when
//  the Player is created and destroyed automatically when the
//  Player is destroyed. No `new`, no `delete`.
// ============================================================

class Player
{
public:
    Player(float screenWidth, float screenHeight,
           int   frameCount    = 6,
           float frameDuration = 1.0f / 8.0f);

    // No copy: our Animator member forbids copy, so the
    // compiler would refuse a copy of Player anyway. We mark
    // it explicit to make the intent obvious.
    Player(const Player&)            = delete;
    Player& operator=(const Player&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    Animator animator_;     // HAS-A
    float    x_, y_;
    float    width_, height_;
};
