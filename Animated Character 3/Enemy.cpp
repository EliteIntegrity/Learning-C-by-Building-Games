#include "Enemy.h"

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
    // Permanent colour multiply on this enemy's owned texture.
    SDL_SetTextureColorMod(animator_.getTexture(), tint.r, tint.g, tint.b);
}

void Enemy::update(float dt) {
    // Pattern: keep base behaviour, then add our own.
    Entity::update(dt);

    x_ -= speed_ * dt;
    if (x_ + drawWidth_ < 0.0f) {
        x_ = screenWidth_;
    }
}

void Enemy::render(SDL_Renderer* renderer) const {
    SDL_FRect src = animator_.getSourceRect();
    SDL_FRect dst{ x_, y_, drawWidth_, drawHeight_ };
    SDL_RenderTextureRotated(
        renderer,
        animator_.getTexture(),
        &src, &dst,
        0.0,
        nullptr,
        SDL_FLIP_HORIZONTAL);
}
