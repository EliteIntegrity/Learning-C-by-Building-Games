#pragma once
#include <SDL3/SDL.h>

// ============================================================
//  IDrawable  -  Chapter 21
// ============================================================
//
//  Companion to IUpdatable. Promises only "I know how to draw
//  myself onto an SDL_Renderer."
//
//  Why two interfaces instead of one combined "IGameObject"?
//  Because a static background might want render() but not
//  update(), and a silent audio emitter might want update()
//  but not render(). Splitting keeps both options open. This
//  is the "interface segregation principle": prefer many
//  small contracts over a few fat ones.
// ============================================================

class IDrawable
{
public:
    virtual ~IDrawable() = default;

    // `const` because rendering should not mutate the object —
    // a HUD doesn't change its smoothed FPS reading while drawing.
    virtual void render(SDL_Renderer* renderer) const = 0;
};
