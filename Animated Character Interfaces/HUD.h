#pragma once
#include <SDL3/SDL.h>
#include "IUpdatable.h"
#include "IDrawable.h"

// ============================================================
//  HUD  -  Chapter 21
// ============================================================
//
//  Same FPS-bar HUD from Chapters 17 and 19, but now it
//  IMPLEMENTS the IUpdatable and IDrawable interfaces. HUD
//  shares no other code with Entity — no Animator, no
//  position, no inheritance chain at all — yet a polymorphic
//  loop can tick it through an IUpdatable* and draw it
//  through an IDrawable* just like it can an Entity.
//
//  That's the whole power of interface-based design: classes
//  with nothing structurally in common can still cooperate
//  with code that only knows about the contracts.
// ============================================================

class HUD : public IUpdatable, public IDrawable
{
public:
    explicit HUD(int sampleCount = 60);
    ~HUD() override;

    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

    float smoothedFps() const;

private:
    float* samples_;
    int    sampleCount_;
    int    nextIndex_;
    int    filledCount_;
};
