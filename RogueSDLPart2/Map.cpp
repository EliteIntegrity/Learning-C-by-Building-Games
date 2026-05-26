#include "Map.h"
#include "GlyphCache.h"

Map::Map(int width, int height)
    : width_(width), height_(height), cells_((size_t)width * (size_t)height)
{
}

Tile&       Map::at(int x, int y)       { return cells_[(size_t)y * width_ + x]; }
const Tile& Map::at(int x, int y) const { return cells_[(size_t)y * width_ + x]; }

bool Map::inBounds(int x, int y) const
{
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

bool Map::isBlocked(int x, int y) const
{
    if (!inBounds(x, y)) return true;
    return at(x, y).terrain == Terrain::Wall;
}

void Map::clearVisible() { for (Tile& t : cells_) t.visible = false; }

void Map::fillWall()
{
    for (Tile& t : cells_)
    {
        t.terrain = Terrain::Wall;
        t.explored = false;
        t.visible  = false;
    }
}

void Map::render(SDL_Renderer* renderer, const GlyphCache& glyphs) const
{
    for (int y = 0; y < height_; ++y)
    {
        for (int x = 0; x < width_; ++x)
        {
            const Tile& t = at(x, y);
            if (!t.explored) continue;

            char      ch  = ' ';
            SDL_Color col = Palette::FLOOR_DARK;
            switch (t.terrain)
            {
                case Terrain::Wall:
                    ch  = '#';
                    col = t.visible ? Palette::WALL_VIS : Palette::WALL_DARK;
                    break;
                case Terrain::Floor:
                    ch  = '.';
                    col = t.visible ? Palette::FLOOR_VIS : Palette::FLOOR_DARK;
                    break;
                case Terrain::StairsDown:
                    ch  = '>';
                    col = t.visible ? Palette::STAIRS : Palette::FLOOR_DARK;
                    break;
            }
            glyphs.draw(renderer, ch, x, y, col);
        }
    }
}
