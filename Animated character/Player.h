#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"

/* ============================================================================
 *  Player  -  TUTORIAL: Composition ("has-a")
 * ============================================================================
 *
 *  In the previous header (Animator.h) we built one self-contained class.
 *  Now we'll build a SECOND class that USES the first. This is how real
 *  programs are organised: small classes are combined to make bigger ones.
 *
 *  The relationship between Player and Animator is COMPOSITION:
 *
 *      "A Player HAS-A Animator."
 *
 *  Notice the line near the bottom:
 *
 *      Animator animator_;
 *
 *  We are storing the Animator BY VALUE - it lives INSIDE the Player.
 *  This has two big consequences:
 *    - The Animator is created automatically when a Player is created.
 *    - The Animator is destroyed automatically when the Player is destroyed.
 *  No `new`, no `delete`, no manual cleanup. The language handles object
 *  lifetimes for us.
 *
 *  Composition is the workhorse of object-oriented design. You may have
 *  heard of "inheritance" (one class IS-A kind of another). Inheritance
 *  is a more advanced tool that we will look at in the next project. For
 *  most jobs, composition ("has-a") is the simpler and better choice.
 * ========================================================================== */

class Player {
public:
    // The constructor takes the resources Player needs and the screen size,
    // so it can position and scale itself sensibly.
    Player(SDL_Texture* spriteSheet,
           int          frameCount,
           float        frameDuration,
           float        screenWidth,
           float        screenHeight);

    // We don't write a destructor here! Player owns no raw resource of
    // its own. Its `animator_` member will be destroyed automatically
    // when the Player is destroyed, and Animator's destructor will free
    // the texture. Let the compiler generate the destructor for us.
    //
    // We still disable copy to be explicit (Animator forbids copy, so
    // copying a Player would fail to compile anyway).
    Player(const Player&)            = delete;
    Player& operator=(const Player&) = delete;

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    Animator animator_;     // <-- COMPOSITION: Player HAS-A Animator
    float    x_;            // top-left destination position on screen
    float    y_;
    float    drawWidth_;    // size to draw at (scaled up from frame size)
    float    drawHeight_;
};
