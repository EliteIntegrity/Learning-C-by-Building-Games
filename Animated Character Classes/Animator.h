#pragma once
#include <SDL3/SDL.h>

// ============================================================
//  Animator  -  Chapter 17
// ============================================================
//
//  Tiny first class. Knows which "frame" is currently showing
//  and what color the player should be tinted at this moment.
//
//  Owns a heap-allocated SDL_Color[] of per-frame tints, which
//  it allocates in the constructor and frees in the destructor.
//  That makes it our minimum-viable RAII demonstration: every
//  `new` here is matched by a `delete[]` you can see.
//
//  Encapsulation: every data member is private. The rest of
//  the program can only call the public methods.
// ============================================================

class Animator
{
public:
    // Constructor: takes how many frames in the cycle, and how
    // long (in seconds) each frame should display.
    Animator(int frameCount, float frameDuration);

    // Destructor: frees the tints[] array.
    ~Animator();

    // Copying a class that owns a heap buffer is dangerous —
    // both copies would try to delete[] the same memory. We
    // disable copy entirely to make the compiler refuse it.
    Animator(const Animator&)            = delete;
    Animator& operator=(const Animator&) = delete;

    // Advance the animation by `dt` seconds.
    void update(float dt);

    // What color should the player be tinted this frame?
    SDL_Color currentTint() const;

    int currentFrame() const { return currentFrame_; }

private:
    SDL_Color* tints_;          // OWNED — destructor frees it
    int        frameCount_;
    float      frameDuration_;
    int        currentFrame_;
    float      accumulator_;    // seconds since last frame change
};
