#pragma once
#include <SDL3/SDL.h>

/* ============================================================================
 *  Animator  -  REFRESHER from the previous project
 * ============================================================================
 *
 *  Animator is unchanged from the first project ("Animated character"). It
 *  owns a sprite sheet texture, advances through frames over time, and
 *  reports which slice of the sheet to draw right now.
 *
 *  Quick recap of the ideas it shows:
 *    - ENCAPSULATION: members are private; outside code uses the public methods
 *    - RAII: constructor takes the texture, destructor frees it
 *    - COPY DISABLED: copying would double-free the texture
 *
 *  In THIS project each character (Player and each Enemy) owns its own
 *  Animator and therefore its own texture. That's deliberately wasteful -
 *  it keeps lifetimes simple and lets each enemy carry its own permanent
 *  colour tint without affecting the others.
 *
 *  The interesting NEW material in this project lives in Entity.h.
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
    SDL_Texture* sheet_;          // OWNED - destructor frees it
    int          frameCount_;
    float        frameDuration_;
    float        frameWidth_;
    float        frameHeight_;
    int          currentFrame_;
    float        accumulator_;
};
