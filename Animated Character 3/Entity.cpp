#include "Entity.h"

Entity::Entity(SDL_Texture* sheet,
               int   frameCount,
               float frameDuration,
               float x,
               float y,
               float drawWidth,
               float drawHeight)
    : animator_(sheet, frameCount, frameDuration),
      x_(x),
      y_(y),
      drawWidth_(drawWidth),
      drawHeight_(drawHeight)
{}

// Default behaviour - just tick the animation. Player uses this as is;
// Enemy overrides to also move x_.
void Entity::update(float dt) {
    animator_.update(dt);
}

// Default behaviour - draw the current frame at our position.
// Enemy overrides to flip horizontally.
void Entity::render(SDL_Renderer* renderer) const {
    SDL_FRect src = animator_.getSourceRect();
    SDL_FRect dst{ x_, y_, drawWidth_, drawHeight_ };
    SDL_RenderTexture(renderer, animator_.getTexture(), &src, &dst);
}
