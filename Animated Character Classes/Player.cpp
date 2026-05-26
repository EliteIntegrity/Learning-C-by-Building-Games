#include "Player.h"

Player::Player(float screenWidth, float screenHeight,
               int frameCount, float frameDuration)
    : animator_(frameCount, frameDuration)
    , width_(120.0f)
    , height_(120.0f)
    , x_((screenWidth  - width_)  * 0.5f)
    , y_((screenHeight - height_) * 0.5f)
{
}

void Player::update(float dt)
{
    animator_.update(dt);
}

void Player::render(SDL_Renderer* renderer) const
{
    SDL_Color tint = animator_.currentTint();
    SDL_SetRenderDrawColor(renderer, tint.r, tint.g, tint.b, tint.a);

    SDL_FRect rect { x_, y_, width_, height_ };
    SDL_RenderFillRect(renderer, &rect);
}
