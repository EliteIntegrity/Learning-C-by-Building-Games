#include "HUD.h"
#include <string>

// ----- Constructor -----------------------------------------------------------
HUD::HUD(TTF_Font* font, SDL_Renderer* renderer)
    : font_(font),
      renderer_(renderer),
      smoothedFps_(60.0f),    // start with a sensible guess
      updateTimer_(0.0f),
      displayedFps_(60)
{}

// ----- Destructor ------------------------------------------------------------
//
// HUD owns its font, so it closes the font here. Notice the symmetry:
// the constructor receives the resource, the destructor releases it.
// This is RAII - again the pattern that makes resource management
// safe and automatic in modern C++.
HUD::~HUD() {
    if (font_) {
        TTF_CloseFont(font_);
    }
}

// ----- update ----------------------------------------------------------------
//
// Two things happen here, both invisible to the outside world:
//
// 1. EXPONENTIAL SMOOTHING: the instantaneous FPS reading 1/dt jumps
//    around wildly from frame to frame. We blend each new sample into
//    a running average so the number we display is stable.
//
// 2. THROTTLING: we only refresh the displayed integer 4 times per
//    second so the user can actually read it.
//
// If you wanted to change either of these strategies, you would change
// only this function - main.cpp would never know.
void HUD::update(float dt) {
    if (dt <= 0.0f) return;     // safety: avoid divide-by-zero

    const float instantFps = 1.0f / dt;
    const float alpha      = 0.1f;       // 0..1: higher = snappier
    smoothedFps_ = smoothedFps_ * (1.0f - alpha) + instantFps * alpha;

    updateTimer_ += dt;
    if (updateTimer_ >= 0.25f) {
        updateTimer_  = 0.0f;
        displayedFps_ = static_cast<int>(smoothedFps_ + 0.5f);  // round
    }
}

// ----- render ----------------------------------------------------------------
//
// Draw the FPS string in the top-left. The drawing pipeline for text
// in SDL_ttf is:
//   1. ask SDL_ttf to render the string into a CPU-side SDL_Surface
//   2. upload that surface to the GPU as an SDL_Texture
//   3. draw the texture
//   4. throw both away
// (For a real game you'd cache the texture - we keep it simple here.)
void HUD::render(SDL_Renderer* renderer) const {
    if (!font_) return;

    const std::string text = "FPS: " + std::to_string(displayedFps_);
    const SDL_Color   white{ 255, 255, 255, 255 };

    // SDL3_ttf wants the string and its length explicitly.
    SDL_Surface* surface = TTF_RenderText_Blended(
        font_, text.c_str(), text.size(), white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect dst{
            20.0f, 20.0f,                          // 20px from top-left
            static_cast<float>(surface->w),
            static_cast<float>(surface->h)
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}
