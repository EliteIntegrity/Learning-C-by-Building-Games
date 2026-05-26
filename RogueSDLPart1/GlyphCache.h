#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>

// ============================================================
//  GlyphCache
// ============================================================
//
//  At startup we render every printable ASCII character into
//  its own SDL_Texture using SDL3_ttf. After that, drawing a
//  cell is one texture blit + one color modulation — fast,
//  and the same character can appear in different colors
//  without having to re-rasterise the font.
// ============================================================

class GlyphCache
{
public:
    GlyphCache(SDL_Renderer* renderer, const char* fontPath, int ptSize);
    ~GlyphCache();

    GlyphCache(const GlyphCache&)            = delete;
    GlyphCache& operator=(const GlyphCache&) = delete;

    // Draw `ch` in `color` at tile-grid coordinates (cellX, cellY).
    void draw(SDL_Renderer* renderer,
              char ch, int cellX, int cellY,
              SDL_Color color) const;

    int glyphPixelWidth()  const { return glyphW_; }
    int glyphPixelHeight() const { return glyphH_; }

private:
    // We own every texture in this map.
    std::unordered_map<char, SDL_Texture*> textures_;
    int glyphW_ = 0;
    int glyphH_ = 0;
};
