#include "Player.h"

// ----- Constructor -----------------------------------------------------------
//
// Look carefully at the member initializer list. The line:
//
//     animator_(spriteSheet, frameCount, frameDuration)
//
// CONSTRUCTS the Animator member directly, in place, by calling its
// constructor with these arguments. No copies, no temporaries. This is
// the standard way to initialize "has-a" members.
Player::Player(SDL_Texture* spriteSheet,
               int          frameCount,
               float        frameDuration,
               float        screenWidth,
               float        screenHeight)
    : animator_(spriteSheet, frameCount, frameDuration),
      x_(0.0f),
      y_(0.0f),
      drawWidth_(0.0f),
      drawHeight_(0.0f)
{
    // Scale the character so it's about 70% of the screen height,
    // preserving the sprite's original aspect ratio.
    const float targetHeight = screenHeight * 0.70f;
    const float scale        = targetHeight / animator_.frameHeight();
    drawWidth_  = animator_.frameWidth()  * scale;
    drawHeight_ = animator_.frameHeight() * scale;

    // Centre horizontally, with feet 5% above the bottom of the screen.
    x_ = (screenWidth  - drawWidth_)  * 0.5f;
    y_ =  screenHeight - drawHeight_ - screenHeight * 0.05f;
}

// ----- update ----------------------------------------------------------------
//
// Player.update is short - it just forwards to the Animator. This is
// called DELEGATION. In a bigger game, Player.update might also handle
// input, physics, health, etc., but right now there's only one thing
// to do: tick the animation.
void Player::update(float dt) {
    animator_.update(dt);
}

// ----- render ----------------------------------------------------------------
//
// Ask the Animator which slice of the sprite sheet to show, then tell
// SDL to draw that slice at our (x, y) at our scaled size. The two
// rectangles passed to SDL_RenderTexture are:
//   src - WHERE in the texture to read pixels FROM
//   dst - WHERE on the screen to draw them TO
void Player::render(SDL_Renderer* renderer) const {
    SDL_FRect src = animator_.getSourceRect();
    SDL_FRect dst{ x_, y_, drawWidth_, drawHeight_ };
    SDL_RenderTexture(renderer, animator_.getTexture(), &src, &dst);
}
