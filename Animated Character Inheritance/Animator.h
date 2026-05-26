#pragma once
#include <SDL3/SDL.h>

// ============================================================
//  Animator  -  unchanged from Chapter 17
// ============================================================
//  Owns a heap-allocated SDL_Color[] of per-frame tints.
//  Advances the current frame over time. RAII handles
//  cleanup. See Chapter 17 for the full walkthrough.
// ============================================================

class Animator
{
public:
    Animator(int frameCount, float frameDuration);
    ~Animator();

    Animator(const Animator&)            = delete;
    Animator& operator=(const Animator&) = delete;

    void update(float dt);

    SDL_Color currentTint() const;
    int       currentFrame() const { return currentFrame_; }

private:
    SDL_Color* tints_;
    int        frameCount_;
    float      frameDuration_;
    int        currentFrame_;
    float      accumulator_;
};
