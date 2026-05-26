#include "Animator.h"
#include <cmath>

static SDL_Color hueToColor(float h)
{
    float r = 0, g = 0, b = 0;
    float i = std::floor(h * 6.0f);
    float f = h * 6.0f - i;
    float q = 1.0f - f;

    switch (((int)i) % 6)
    {
        case 0: r = 1; g = f; b = 0; break;
        case 1: r = q; g = 1; b = 0; break;
        case 2: r = 0; g = 1; b = f; break;
        case 3: r = 0; g = q; b = 1; break;
        case 4: r = f; g = 0; b = 1; break;
        case 5: r = 1; g = 0; b = q; break;
    }

    return SDL_Color{
        (Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), 255
    };
}

Animator::Animator(int frameCount, float frameDuration)
    : tints_(new SDL_Color[frameCount])
    , frameCount_(frameCount)
    , frameDuration_(frameDuration)
    , currentFrame_(0)
    , accumulator_(0.0f)
{
    for (int i = 0; i < frameCount_; ++i)
        tints_[i] = hueToColor((float)i / (float)frameCount_);
}

Animator::~Animator()
{
    delete[] tints_;
}

void Animator::update(float dt)
{
    accumulator_ += dt;
    while (accumulator_ >= frameDuration_)
    {
        accumulator_ -= frameDuration_;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
    }
}

SDL_Color Animator::currentTint() const
{
    return tints_[currentFrame_];
}
