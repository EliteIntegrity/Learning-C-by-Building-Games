#pragma once
#include <SDL3/SDL.h>

/* ============================================================================
 *  Animator  -  TUTORIAL: A first look at a C++ class
 * ============================================================================
 *
 *  A class is a bundle of DATA + FUNCTIONS that operate on that data.
 *  Animator is responsible for ONE thing: knowing which frame of a sprite
 *  sheet to show at any given moment.
 *
 *  Three ideas to notice in this header:
 *
 *  1. ENCAPSULATION
 *     The members under `private:` cannot be touched by any code outside
 *     this class. Other code can only USE the public methods. This means
 *     we can change HOW the animation works internally without breaking
 *     any code that uses Animator.
 *
 *  2. CONSTRUCTOR / DESTRUCTOR
 *     Animator(...)   runs automatically when an object is created.
 *     ~Animator()     runs automatically when an object is destroyed.
 *     Use the constructor to set up state; use the destructor to release
 *     resources. This pattern is called RAII (Resource Acquisition Is
 *     Initialization) and it is the heart of safe modern C++.
 *
 *  3. OWNERSHIP
 *     Animator OWNS the SDL_Texture* it is given. Owning means "I will
 *     free it when I am destroyed." Any pointer a class stores should
 *     have a clear answer to the question "who frees this?"
 *
 *  Read order for the project: Animator.h -> Player.h -> HUD.h -> main.cpp
 * ========================================================================== */

class Animator {
public:
    // ----- public interface (what the rest of the program is allowed to call)

    // Constructor. The colon starts a "member initializer list" that
    // initializes each member directly - more efficient and clearer than
    // assigning inside the body of the constructor.
    Animator(SDL_Texture* sheet, int frameCount, float frameDuration);

    // Destructor. SDL_DestroyTexture is called here (see Animator.cpp).
    ~Animator();

    // We disable COPYING because copying would duplicate the texture
    // pointer and then BOTH copies would try to free it - a classic bug.
    // Marking these `= delete` makes the compiler reject any copy attempt.
    Animator(const Animator&)            = delete;
    Animator& operator=(const Animator&) = delete;

    // Advance the animation by `dt` seconds (the time since last frame).
    void update(float dt);

    // Where in the sprite sheet is the current frame?
    SDL_FRect getSourceRect() const;

    // Read-only "getters". The trailing `const` is a promise to the
    // compiler that this method does not modify the object.
    SDL_Texture* getTexture()   const { return sheet_; }
    float        frameWidth()   const { return frameWidth_; }
    float        frameHeight()  const { return frameHeight_; }
    int          currentFrame() const { return currentFrame_; }

private:
    // ----- private state (internal details, hidden from the outside world)
    //
    // Convention: trailing underscore on member variables. This is a popular
    // style that helps you distinguish a member from a local variable at a
    // glance. Other codebases use m_name or just name - pick one and stick.

    SDL_Texture* sheet_;          // OWNED - destructor frees it
    int          frameCount_;
    float        frameDuration_;  // seconds each frame should be shown
    float        frameWidth_;
    float        frameHeight_;
    int          currentFrame_;
    float        accumulator_;    // time elapsed since last frame change
};
