#include "Enemy.h"

Enemy::Enemy(float startX, float y,
             float width, float height,
             float speed, float screenWidth,
             SDL_Color baseColor)
    : Entity(startX, y, width, height)
    , speed_(speed)
    , screenWidth_(screenWidth)
    , baseColor_(baseColor)
{
}

void Enemy::update(float dt)
{
    // Do what the base does (advance the animator)...
    Entity::update(dt);

    // ...then add our own behaviour.
    x_ -= speed_ * dt;
    if (x_ + width_ < 0.0f)        // off the left edge?
        x_ = screenWidth_;          // wrap to the right
}

void Enemy::render(SDL_Renderer* renderer) const
{
    // Use the enemy's own fixed color instead of the rainbow.
    // We still read the Animator's current frame to add a tiny size
    // pulse, just to show the inherited Animator is alive.
    int   frame  = animator_.currentFrame();
    float pulse  = 1.0f + 0.05f * (float)(frame % 2);   // 1.0 or 1.05
    float w      = width_  * pulse;
    float h      = height_ * pulse;
    float drawX  = x_ - (w - width_)  * 0.5f;
    float drawY  = y_ - (h - height_) * 0.5f;

    SDL_SetRenderDrawColor(renderer,
                           baseColor_.r, baseColor_.g, baseColor_.b, baseColor_.a);
    SDL_FRect rect { drawX, drawY, w, h };
    SDL_RenderFillRect(renderer, &rect);
}
