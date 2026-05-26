#include "Animator.h"

Animator::Animator(SDL_Texture* sheet, int frameCount, float frameDuration)
    : sheet_(sheet),
      frameCount_(frameCount),
      frameDuration_(frameDuration),
      frameWidth_(0.0f),
      frameHeight_(0.0f),
      currentFrame_(0),
      accumulator_(0.0f)
{
    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(sheet_, &w, &h);
    frameWidth_  = w / static_cast<float>(frameCount_);
    frameHeight_ = h;
}

Animator::~Animator() {
    if (sheet_) {
        SDL_DestroyTexture(sheet_);
    }
}

void Animator::update(float dt) {
    accumulator_ += dt;
    while (accumulator_ >= frameDuration_) {
        accumulator_ -= frameDuration_;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
    }
}

SDL_FRect Animator::getSourceRect() const {
    return SDL_FRect{
        currentFrame_ * frameWidth_,
        0.0f,
        frameWidth_,
        frameHeight_
    };
}
