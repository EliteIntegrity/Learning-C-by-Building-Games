#include "Entity.h"
#include "GlyphCache.h"

Entity::Entity(Point pos, char glyph, SDL_Color color)
    : pos_(pos), glyph_(glyph), color_(color) {}

void Entity::render(SDL_Renderer* renderer, const GlyphCache& glyphs) const
{
    glyphs.draw(renderer, glyph_, pos_.x, pos_.y, color_);
}
