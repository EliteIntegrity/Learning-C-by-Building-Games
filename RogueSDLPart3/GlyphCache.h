#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>

class GlyphCache
{
public:
    GlyphCache(SDL_Renderer* renderer, const char* fontPath, int ptSize);
    ~GlyphCache();

    GlyphCache(const GlyphCache&)            = delete;
    GlyphCache& operator=(const GlyphCache&) = delete;

    // Draw `ch` in `color` at tile-grid (cellX, cellY).
    void draw(SDL_Renderer* renderer,
              char ch, int cellX, int cellY,
              SDL_Color color) const;

    // Draw a full string starting at (cellX, cellY) and proceeding right.
    void drawText(SDL_Renderer* renderer,
                  const char* text, int cellX, int cellY,
                  SDL_Color color) const;

    int glyphPixelWidth()  const { return glyphW_; }
    int glyphPixelHeight() const { return glyphH_; }

private:
    std::unordered_map<char, SDL_Texture*> textures_;
    int glyphW_ = 0;
    int glyphH_ = 0;
};
