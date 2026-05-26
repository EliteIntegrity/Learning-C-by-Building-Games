#include "Entity.h"

// ----- Constructor -----------------------------------------------------------
//
// Just like before, members are constructed via the initializer list.
// The interesting addition in this project is what happens in the
// derived classes: their constructors will call OUR constructor first
// (implicitly, with the arguments they pass through), and THEN run
// their own bodies. Construction order is base -> derived.
Entity::Entity(SDL_Texture* sheet,
               int   frameCount,
               float frameDuration,
               float x,
               float y,
               float drawWidth,
               float drawHeight)
    : animator_(sheet, frameCount, frameDuration),
      x_(x),
      y_(y),
      drawWidth_(drawWidth),
      drawHeight_(drawHeight)
{}

// ----- update ----------------------------------------------------------------
//
// The default behaviour: just advance the animation. Player uses this
// unchanged. Enemy overrides this to also move itself across the screen.
void Entity::update(float dt) {
    animator_.update(dt);
}

// ----- render ----------------------------------------------------------------
//
// The default behaviour: draw the current frame at (x_, y_), un-flipped,
// at the configured size. Player uses this unchanged. Enemy overrides
// this to flip the sprite horizontally.
void Entity::render(SDL_Renderer* renderer) const {
    SDL_FRect src = animator_.getSourceRect();
    SDL_FRect dst{ x_, y_, drawWidth_, drawHeight_ };
    SDL_RenderTexture(renderer, animator_.getTexture(), &src, &dst);
}
