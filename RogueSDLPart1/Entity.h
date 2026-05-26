#pragma once
#include <SDL3/SDL.h>
#include "Common.h"

class GlyphCache;

// ============================================================
//  Entity  -  shared base for anything that lives on a tile.
// ============================================================
//
//  Just the Player in Chapter 28. Enemy and Item subclasses
//  join in Chapter 29 — having the base ready means no
//  refactor is needed when they show up.
// ============================================================

class Entity
{
public:
    Entity(Point pos, char glyph, SDL_Color color);
    virtual ~Entity() = default;

    Entity(const Entity&)            = delete;
    Entity& operator=(const Entity&) = delete;

    Point  position() const { return pos_; }
    char   glyph()    const { return glyph_; }
    SDL_Color color() const { return color_; }

    void   setPosition(Point p) { pos_ = p; }

    virtual void render(SDL_Renderer* renderer, const GlyphCache& glyphs) const;

protected:
    Point     pos_;
    char      glyph_;
    SDL_Color color_;
};
