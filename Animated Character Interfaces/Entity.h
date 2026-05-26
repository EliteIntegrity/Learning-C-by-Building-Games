#pragma once
#include <SDL3/SDL.h>
#include "IUpdatable.h"
#include "IDrawable.h"
#include "Animator.h"

// ============================================================
//  Entity  -  Chapter 21
// ============================================================
//
//  Same base class as Chapter 19, now adjusted for proper
//  polymorphism:
//
//      class Entity : public IUpdatable, public IDrawable
//
//  Entity now PROMISES to implement both interfaces — which
//  it does, by providing concrete update() and render()
//  bodies. Those bodies are still virtual (because they came
//  from pure virtuals in the interfaces), so Enemy can
//  override them and have the right version called through
//  any IUpdatable* / IDrawable* / Entity* pointer.
//
//  We no longer need to write `virtual ~Entity()` ourselves —
//  the interfaces' virtual destructors propagate. Less code,
//  same correctness.
// ============================================================

class Entity : public IUpdatable, public IDrawable
{
public:
    Entity(float x, float y,
           float width, float height,
           int   frameCount    = 6,
           float frameDuration = 1.0f / 8.0f);

    Entity(const Entity&)            = delete;
    Entity& operator=(const Entity&) = delete;

    // `override` lets the compiler verify these really do
    // override the pure virtuals declared in IUpdatable/IDrawable.
    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

protected:
    Animator animator_;
    float    x_, y_;
    float    width_, height_;
};
