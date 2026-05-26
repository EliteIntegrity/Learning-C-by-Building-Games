#pragma once
#include <SDL3/SDL.h>

// HUD - unchanged from Chapter 17. Smoothed FPS bar.
class HUD
{
public:
    explicit HUD(int sampleCount = 60);
    ~HUD();

    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

    float smoothedFps() const;

private:
    float* samples_;
    int    sampleCount_;
    int    nextIndex_;
    int    filledCount_;
};
