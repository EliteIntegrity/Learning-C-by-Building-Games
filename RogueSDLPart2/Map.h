#pragma once
#include <vector>
#include "Tile.h"
#include "Common.h"

class GlyphCache;

class Map
{
public:
    Map(int width, int height);

    int width()  const { return width_; }
    int height() const { return height_; }

    Tile&       at(int x, int y);
    const Tile& at(int x, int y) const;

    bool inBounds(int x, int y) const;
    bool isBlocked(int x, int y) const;

    void clearVisible();
    void fillWall();

    void render(SDL_Renderer* renderer, const GlyphCache& glyphs) const;

private:
    int               width_;
    int               height_;
    std::vector<Tile> cells_;
};
