#include "Entity.h"

Entity::Entity(float x, float y,
               float width, float height,
               int frameCount, float frameDuration)
    : animator_(frameCount, frameDuration)
    , x_(x), y_(y)
    , width_(width), height_(height)
{
}

void Entity::update(float dt)
{
    animator_.update(dt);
}

void Entity::render(SDL_Renderer* renderer) const
{
    SDL_Color tint = animator_.currentTint();
    SDL_SetRenderDrawColor(renderer, tint.r, tint.g, tint.b, tint.a);

    SDL_FRect rect { x_, y_, width_, height_ };
    SDL_RenderFillRect(renderer, &rect);
}
