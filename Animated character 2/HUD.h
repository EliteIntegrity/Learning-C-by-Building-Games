#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/* ============================================================================
 *  HUD  -  REFRESHER from the previous project
 * ============================================================================
 *
 *  HUD draws a smoothed FPS counter in the top-left corner. It is
 *  unchanged from the first project. The two ideas it shows:
 *
 *    - hidden state (smoothing + throttling) lives behind a public
 *      interface of just update() and render()
 *    - RAII: HUD owns the TTF_Font and closes it in its destructor
 *
 *  HUD is NOT part of the inheritance hierarchy in this project. It
 *  doesn't need to be - it has nothing in common with the characters,
 *  so making it inherit from Entity would be silly. A common beginner
 *  trap is to over-use inheritance once you've learned it; resist that
 *  urge. Inherit only when there's a real "is-a" relationship.
 * ========================================================================== */

class HUD {
public:
    HUD(TTF_Font* font, SDL_Renderer* renderer);
    ~HUD();

    HUD(const HUD&)            = delete;
    HUD& operator=(const HUD&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    TTF_Font*     font_;
    SDL_Renderer* renderer_;
    float         smoothedFps_;
    float         updateTimer_;
    int           displayedFps_;
};
