#include "Player.h"

namespace {
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
{}
