#include "HUD.h"
#include <string>

HUD::HUD(TTF_Font* font, SDL_Renderer* renderer)
    : font_(font),
      renderer_(renderer),
      smoothedFps_(60.0f),
      updateTimer_(0.0f),
      displayedFps_(60)
{}

HUD::~HUD() {
    if (font_) {
        TTF_CloseFont(font_);
    }
}

void HUD::update(float dt) {
    if (dt <= 0.0f) return;

    const float instantFps = 1.0f / dt;
    const float alpha      = 0.1f;
    smoothedFps_ = smoothedFps_ * (1.0f - alpha) + instantFps * alpha;

    updateTimer_ += dt;
    if (updateTimer_ >= 0.25f) {
        updateTimer_  = 0.0f;
        displayedFps_ = static_cast<int>(smoothedFps_ + 0.5f);
    }
}

void HUD::render(SDL_Renderer* renderer) const {
    if (!font_) return;

    const std::string text = "FPS: " + std::to_string(displayedFps_);
    const SDL_Color   white{ 255, 255, 255, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(
        font_, text.c_str(), text.size(), white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect dst{
            20.0f, 20.0f,
            static_cast<float>(surface->w),
            static_cast<float>(surface->h)
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}
