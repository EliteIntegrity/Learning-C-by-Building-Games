#include "HUD.h"

HUD::HUD(int sampleCount)
    : samples_(new float[sampleCount])
    , sampleCount_(sampleCount)
    , nextIndex_(0)
    , filledCount_(0)
{
    for (int i = 0; i < sampleCount_; ++i)
        samples_[i] = 0.0f;
}

HUD::~HUD()
{
    delete[] samples_;
}

void HUD::update(float dt)
{
    samples_[nextIndex_] = dt;
    nextIndex_ = (nextIndex_ + 1) % sampleCount_;
    if (filledCount_ < sampleCount_) ++filledCount_;
}

float HUD::smoothedFps() const
{
    if (filledCount_ == 0) return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < filledCount_; ++i)
        sum += samples_[i];

    float avgDt = sum / (float)filledCount_;
    if (avgDt <= 0.0f) return 0.0f;
    return 1.0f / avgDt;
}

void HUD::render(SDL_Renderer* renderer) const
{
    // Draw a small FPS bar in the top-left corner.
    // 0 fps -> empty bar, 120+ fps -> full bar.
    float fps = smoothedFps();
    if (fps > 120.0f) fps = 120.0f;

    float fillFrac = fps / 120.0f;

    // Frame
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_FRect frame { 10.0f, 10.0f, 200.0f, 14.0f };
    SDL_RenderRect(renderer, &frame);

    // Fill
    SDL_SetRenderDrawColor(renderer, 80, 200, 100, 255);
    SDL_FRect fill { 12.0f, 12.0f, 196.0f * fillFrac, 10.0f };
    SDL_RenderFillRect(renderer, &fill);
}
