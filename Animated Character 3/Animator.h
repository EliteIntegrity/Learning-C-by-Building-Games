#pragma once
#include <SDL3/SDL.h>

/* ============================================================================
 *  Animator  -  unchanged from previous projects
 * ============================================================================
 *
 *  No changes. Animator is still a self-contained class that owns its
 *  texture and walks through frames. It does not participate in the
 *  interface hierarchy because nothing in main wants to call update()
 *  or render() on a bare Animator - Animator is an internal helper
 *  used by Entity, never a top-level object.
 *
 *  Notice that this is a perfectly fine outcome of OOP design: not
 *  every class needs to fit into your inheritance tree. If a class
 *  works on its own, leave it alone.
 * ========================================================================== */

class Animator {
public:
    Animator(SDL_Texture* sheet, int frameCount, float frameDuration);
    ~Animator();

    Animator(const Animator&)            = delete;
    Animator& operator=(const Animator&) = delete;

    void update(float dt);
    SDL_FRect getSourceRect() const;

    SDL_Texture* getTexture()   const { return sheet_; }
    float        frameWidth()   const { return frameWidth_; }
    float        frameHeight()  const { return frameHeight_; }
    int          currentFrame() const { return currentFrame_; }

private:
    SDL_Texture* sheet_;
    int          frameCount_;
    float        frameDuration_;
    float        frameWidth_;
    float        frameHeight_;
    int          currentFrame_;
    float        accumulator_;
};
