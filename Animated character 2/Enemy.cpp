#include "Enemy.h"

// ----- Constructor -----------------------------------------------------------
//
// Like Player, we forward most arguments to the Entity base. Then we
// store our own extra members (speed_, screenWidth_) and apply the
// permanent tint to our owned texture.
Enemy::Enemy(SDL_Texture* sheet,
             int   frameCount,
             float frameDuration,
             float startX,
             float startY,
             float drawWidth,
             float drawHeight,
             float speed,
             float screenWidth,
             SDL_Color tint)
    : Entity(sheet, frameCount, frameDuration,
             startX, startY, drawWidth, drawHeight),
      speed_(speed),
      screenWidth_(screenWidth)
{
    // SDL_SetTextureColorMod multiplies every drawn pixel by this colour.
    // Set once and forget - it sticks until we change it. Because each
    // Enemy owns its own copy of the texture (via its own Animator),
    // tinting one enemy never affects another or the player.
    SDL_SetTextureColorMod(animator_.getTexture(), tint.r, tint.g, tint.b);
}

// ----- update ----------------------------------------------------------------
//
// Pattern: "do what the base class does, then add my own behaviour."
void Enemy::update(float dt) {
    // Call the base version explicitly - without `Entity::` we'd
    // recurse into ourselves forever.
    Entity::update(dt);

    // Move leftward at our configured speed.
    x_ -= speed_ * dt;

    // Wrap around: when fully off the left edge, teleport to just past
    // the right edge. This gives an endless-runner feeling.
    if (x_ + drawWidth_ < 0.0f) {
        x_ = screenWidth_;
    }
}

// ----- render ----------------------------------------------------------------
//
// Same as Entity::render except we use SDL_RenderTextureRotated so we
// can pass SDL_FLIP_HORIZONTAL. We didn't need to call Entity::render -
// we're replacing the drawing entirely.
void Enemy::render(SDL_Renderer* renderer) const {
    SDL_FRect src = animator_.getSourceRect();
    SDL_FRect dst{ x_, y_, drawWidth_, drawHeight_ };
    SDL_RenderTextureRotated(
        renderer,
        animator_.getTexture(),
        &src, &dst,
        0.0,                    // no rotation
        nullptr,                // default centre
        SDL_FLIP_HORIZONTAL);   // <-- this is the only behaviour we add
}
