#include "FOV.h"
#include "Map.h"
#include "Tile.h"

#include <cstdlib>

namespace
{
    void marchRay(Map& map, int x0, int y0, int x1, int y1)
    {
        int dx =  std::abs(x1 - x0);
        int dy = -std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx + dy;

        int x = x0, y = y0;
        for (;;)
        {
            if (!map.inBounds(x, y)) return;
            Tile& t = map.at(x, y);
            t.visible  = true;
            t.explored = true;
            if (t.terrain == Terrain::Wall) return;
            if (x == x1 && y == y1) return;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x += sx; }
            if (e2 <= dx) { err += dx; y += sy; }
        }
    }
}

namespace FOV
{
    void compute(Map& map, Point origin, int radius)
    {
        map.clearVisible();

        int x0 = origin.x, y0 = origin.y;

        for (int x = x0 - radius; x <= x0 + radius; ++x)
        {
            marchRay(map, x0, y0, x, y0 - radius);
            marchRay(map, x0, y0, x, y0 + radius);
        }
        for (int y = y0 - radius; y <= y0 + radius; ++y)
        {
            marchRay(map, x0, y0, x0 - radius, y);
            marchRay(map, x0, y0, x0 + radius, y);
        }

        if (map.inBounds(x0, y0))
        {
            map.at(x0, y0).visible  = true;
            map.at(x0, y0).explored = true;
        }
    }

    bool hasLineOfSight(const Map& map, Point from, Point to)
    {
        // Bresenham again, but read-only — no flag-touching, stop on
        // wall and report false; reach the target and report true.
        int x0 = from.x, y0 = from.y, x1 = to.x, y1 = to.y;
        int dx =  std::abs(x1 - x0);
        int dy = -std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx + dy;

        int x = x0, y = y0;
        for (;;)
        {
            if (x == x1 && y == y1) return true;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x += sx; }
            if (e2 <= dx) { err += dx; y += sy; }
            if (!map.inBounds(x, y)) return false;
            // Walls block sight (but DON'T block the moment we ARE the wall —
            // line-of-sight to the wall cell itself succeeds, just not past it).
            if (x == x1 && y == y1) return true;
            if (map.at(x, y).terrain == Terrain::Wall) return false;
        }
    }
}
