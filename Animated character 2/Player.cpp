#include "Player.h"

// Helper that figures out a sensible draw size and on-screen position
// for the player, then forwards everything to Entity's constructor.
//
// Notice the construction is a TWO-STEP MENTAL DANCE:
//
//   1. We compute scale / position values on the right-hand side.
//   2. We pass them up to the Entity base class via the initializer list.
//
// Because the Entity base must be fully constructed before any Player
// code runs, we can't compute these values inside the body and assign
// them to the base members afterwards. We compute them in helper
// expressions and feed them straight into the Entity constructor.
//
// To keep this readable we use small helper free-functions that take
// the screen size and return one number each.

namespace {
    // 70% of screen height, preserving aspect ratio. We need to know the
    // sheet's frame size to do the scaling - rather than load it twice
    // we just hard-code the source aspect ratio (frameW / frameH).
    // 2288/6 = 381.33 wide, 456 tall. Aspect = 381.33 / 456 ~= 0.836.
    constexpr float kFrameAspect = (2288.0f / 6.0f) / 456.0f;

    float playerHeight(float screenH)        { return screenH * 0.70f; }
    float playerWidth (float screenH)        { return playerHeight(screenH) * kFrameAspect; }
    float playerX     (float screenW, float screenH) {
        return (screenW - playerWidth(screenH)) * 0.5f;
    }
    float playerY     (float screenH) {
        return screenH - playerHeight(screenH) - screenH * 0.05f;
    }
}

Player::Player(SDL_Texture* sheet,
               int   frameCount,
               float frameDuration,
               float screenWidth,
               float screenHeight)
    : Entity(sheet,
             frameCount,
             frameDuration,
             playerX(screenWidth, screenHeight),
             playerY(screenHeight),
             playerWidth(screenHeight),
             playerHeight(screenHeight))
{
    // Player has no extra construction work - the base did everything.
}
