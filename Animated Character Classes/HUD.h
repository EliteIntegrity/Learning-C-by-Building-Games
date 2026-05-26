#pragma once
#include <SDL3/SDL.h>

// ============================================================
//  HUD  -  Chapter 17
// ============================================================
//
//  Heads-up display. Draws a small frame-rate bar in the top
//  corner of the window. Internally tracks a smoothed FPS so
//  the bar doesn't jitter every frame.
//
//  Owns a heap-allocated float[] used as a circular buffer of
//  recent frame deltas. Constructor allocates it, destructor
//  frees it — a second worked example of RAII.
// ============================================================

class HUD
{
public:
    explicit HUD(int sampleCount = 60);
    ~HUD();

    // No copy: HUD owns a heap buffer.
    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

    float smoothedFps() const;

private:
    float* samples_;        // OWNED — destructor frees it
    int    sampleCount_;
    int    nextIndex_;      // where the next sample is written
    int    filledCount_;    // how many of the slots are valid (caps at sampleCount_)
};
