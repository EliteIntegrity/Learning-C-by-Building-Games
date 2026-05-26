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
    Entity::update(dt);

    x_ -= speed_ * dt;
    if (x_ + width_ < 0.0f)
        x_ = screenWidth_;
}

void Enemy::render(SDL_Renderer* renderer) const
{
    int   frame  = animator_.currentFrame();
    float pulse  = 1.0f + 0.05f * (float)(frame % 2);
    float w      = width_  * pulse;
    float h      = height_ * pulse;
    float drawX  = x_ - (w - width_)  * 0.5f;
    float drawY  = y_ - (h - height_) * 0.5f;

    SDL_SetRenderDrawColor(renderer,
                           baseColor_.r, baseColor_.g, baseColor_.b, baseColor_.a);
    SDL_FRect rect { drawX, drawY, w, h };
    SDL_RenderFillRect(renderer, &rect);
}
