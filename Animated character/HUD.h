#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/* ============================================================================
 *  HUD  -  TUTORIAL: Hiding messy state behind a tidy interface
 * ============================================================================
 *
 *  HUD stands for "Heads-Up Display" - the on-screen text overlay common
 *  in games (score, ammo, FPS counter, etc). Our HUD draws an FPS readout
 *  in the top-left corner.
 *
 *  Look at the public interface below. From the outside, HUD does only
 *  three things: construct, update, render. SIMPLE.
 *
 *  But inside, HUD does some real work:
 *    - it owns a TTF_Font* and must close it at destruction
 *    - it smooths the FPS reading so it doesn't flicker every frame
 *    - it throttles how often the on-screen number updates
 *
 *  All of that complexity lives behind the public interface. The code in
 *  main.cpp doesn't know - or care - HOW HUD calculates its number. If
 *  we wanted to switch to a moving-average smoother tomorrow, we'd
 *  change HUD.cpp and nothing else in the program.
 *
 *  This separation between WHAT a class does (its interface) and HOW it
 *  does it (its implementation) is the central idea of object-oriented
 *  programming. Get it right and your code stays easy to change.
 * ========================================================================== */

class HUD {
public:
    // Constructor takes the resources HUD will need.
    // HUD will TAKE OWNERSHIP of `font` (it will close it on destruction).
    // HUD will NOT take ownership of `renderer` (main.cpp owns it).
    HUD(TTF_Font* font, SDL_Renderer* renderer);

    // Destructor closes the font (see HUD.cpp).
    ~HUD();

    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    TTF_Font*     font_;          // OWNED - destructor closes it
    SDL_Renderer* renderer_;      // not owned - main.cpp owns it
    float         smoothedFps_;   // running average for stability
    float         updateTimer_;   // throttles how often the number changes
    int           displayedFps_;  // the integer the user actually sees
};
