#include "Animator.h"

// ----- Constructor -----------------------------------------------------------
//
// The colon below begins the MEMBER INITIALIZER LIST. Each member of the
// class is initialized directly here, before the body { ... } runs. This
// is preferred over assigning inside the body because it's faster and
// works for members that cannot be reassigned (e.g. references, const).
Animator::Animator(SDL_Texture* sheet, int frameCount, float frameDuration)
    : sheet_(sheet),
      frameCount_(frameCount),
      frameDuration_(frameDuration),
      frameWidth_(0.0f),
      frameHeight_(0.0f),
      currentFrame_(0),
      accumulator_(0.0f)
{
    // Ask SDL for the texture's pixel dimensions.
    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(sheet_, &w, &h);

    // The sheet is one row of equal-width frames laid out left-to-right.
    // Note 2288 / 6 = 381.33 - NOT an integer. Using floats lets us step
    // through the sheet exactly without drifting off the right edge.
    // SDL renders sub-pixel source rectangles just fine.
    frameWidth_  = w / static_cast<float>(frameCount_);
    frameHeight_ = h;
}

// ----- Destructor ------------------------------------------------------------
//
// Runs automatically when an Animator object goes out of scope (or is
// deleted, if it was created with `new`). Because Player owns its
// Animator BY VALUE, this destructor will run when the Player itself
// goes out of scope at the end of main.
Animator::~Animator() {
    if (sheet_) {
        SDL_DestroyTexture(sheet_);
    }
}

// ----- update ----------------------------------------------------------------
//
// dt = "delta time" = seconds since last frame. This is how we make the
// animation play at a consistent SPEED regardless of how fast (or slow)
// the program's render loop is running. If we just advanced the frame
// once per render call, the character would run faster on a 144 Hz
// monitor than on a 60 Hz monitor - bad!
void Animator::update(float dt) {
    accumulator_ += dt;

    // Use a `while`, not `if`: if dt is unusually large (e.g. the program
    // stuttered), we may need to advance more than one animation frame.
    while (accumulator_ >= frameDuration_) {
        accumulator_ -= frameDuration_;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;  // wrap to 0
    }
}

// ----- getSourceRect ---------------------------------------------------------
//
// Returns the rectangle within the sprite sheet that should be drawn
// right now. SDL_FRect is just { x, y, w, h } as floats.
SDL_FRect Animator::getSourceRect() const {
    return SDL_FRect{
        currentFrame_ * frameWidth_,   // frame N starts at x = N * width
        0.0f,                          // single row of frames -> y = 0
        frameWidth_,
        frameHeight_
    };
}
