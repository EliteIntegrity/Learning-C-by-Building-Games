#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "IUpdatable.h"
#include "IDrawable.h"

/* ============================================================================
 *  HUD  -  TUTORIAL: An UNRELATED class that implements the same interfaces
 * ============================================================================
 *
 *  This is where interfaces show their real value.
 *
 *  HUD is NOT an Entity. It has no animator, no position in world
 *  coordinates, no sprite. It would be wrong to make it inherit from
 *  Entity just to get update() and render() called.
 *
 *  But HUD IS something that knows how to be ticked over time, and
 *  IS something that knows how to draw itself. Those are exactly the
 *  two interface contracts we defined. So we make HUD ALSO implement
 *  IUpdatable and IDrawable - even though HUD has nothing else in
 *  common with Entity.
 *
 *      class HUD : public IUpdatable, public IDrawable { ... };
 *
 *  Now in main.cpp we can stuff a Player, several Enemies, AND the
 *  HUD into the same vector of IUpdatable* (or IDrawable*) and
 *  iterate them all with one line of code. The HUD object and the
 *  Player object share NO BASE CLASS at all - and yet they cooperate
 *  inside the same loop. That's the whole point of interfaces.
 *
 *
 *  THE OPEN/CLOSED PRINCIPLE
 *
 *  Imagine adding a particle system, a damage-flash overlay, or a
 *  background scroller. Each of those is a class with no Entity-like
 *  state. As long as it implements IUpdatable and IDrawable, the
 *  main loop drives it for free. main.cpp never needs to be modified
 *  to know about the new type. The system is open to EXTENSION
 *  (via new classes that implement the interfaces) but the existing
 *  code stays closed to MODIFICATION.
 *
 *  This is the open/closed principle, and interfaces are how you
 *  achieve it in practice.
 * ========================================================================== */

class HUD : public IUpdatable, public IDrawable {
public:
    HUD(TTF_Font* font, SDL_Renderer* renderer);
    ~HUD();

    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    // These override the pure-virtual declarations from the interfaces.
    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

private:
    TTF_Font*     font_;
    SDL_Renderer* renderer_;
    float         smoothedFps_;
    float         updateTimer_;
    int           displayedFps_;
};
