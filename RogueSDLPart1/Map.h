#pragma once
#include <vector>
#include "Tile.h"
#include "Common.h"

class GlyphCache;

// ============================================================
//  Map  -  the dungeon grid.
// ============================================================
//
//  Storage is one big std::vector<Tile> in row-major order
//  (every row's tiles stored contiguously). That's cache-
//  friendly for the kind of dense per-tile loops the FOV and
//  render passes do every turn — see Chapter 27 on data
//  locality for why.
// ============================================================

class Map
{
public:
    Map(int width, int height);

    int width()  const { return width_; }
    int height() const { return height_; }

    Tile&       at(int x, int y);
    const Tile& at(int x, int y) const;

    bool inBounds(int x, int y) const;
    bool isBlocked(int x, int y) const;  // wall blocks movement and sight

    // Clear all visible flags. Call before recomputing FOV each turn.
    void clearVisible();

    // Fill every tile with wall. Used at the start of generation.
    void fillWall();

    // Render every tile, respecting visible / explored flags.
    void render(SDL_Renderer* renderer, const GlyphCache& glyphs) const;

private:
    int               width_;
    int               height_;
    std::vector<Tile> cells_;
};
