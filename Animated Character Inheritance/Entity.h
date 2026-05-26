#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"

// ============================================================
//  Entity  -  Chapter 19
// ============================================================
//
//  Base class for any animated character on screen. Bundles
//  an Animator with a position and size, plus a default
//  update() and render() that subclasses can use as-is.
//
//  Notice what is NOT here:
//      - no `virtual` keywords (those are Chapter 20)
//      - no abstract methods
//      - no virtual destructor (because we never store these
//        in a base-typed container in this project — that's
//        the slicing problem from Chapter 18, also fixed in
//        Chapter 20)
//
//  Members are PROTECTED so derived classes (Player, Enemy)
//  can read and write them — Enemy adjusts x_ each frame to
//  move across the screen.
// ============================================================

class Entity
{
public:
    Entity(float x, float y,
           float width, float height,
           int   frameCount    = 6,
           float frameDuration = 1.0f / 8.0f);

    ~Entity() = default;

    Entity(const Entity&)            = delete;
    Entity& operator=(const Entity&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

protected:
    Animator animator_;
    float    x_, y_;
    float    width_, height_;
};
