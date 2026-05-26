#include "FOV.h"
#include "Map.h"
#include "Tile.h"

#include <cstdlib>   // abs

namespace
{
    // Walk a Bresenham line from (x0, y0) to (x1, y1). For every
    // tile along the way, mark it visible+explored. If we hit a
    // wall, mark it visible+explored then stop (you can SEE the
    // wall that blocks your view).
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

        // March a ray from the origin to every tile on the
        // boundary of the FOV square. Diagonals naturally end
        // up doing more work than necessary; for our small
        // radius (~8) the cost is invisible.
        int x0 = origin.x;
        int y0 = origin.y;
        int x1, y1;

        // Top and bottom edges of the FOV square
        for (int x = x0 - radius; x <= x0 + radius; ++x)
        {
            marchRay(map, x0, y0, x, y0 - radius);
            marchRay(map, x0, y0, x, y0 + radius);
        }
        // Left and right edges
        for (int y = y0 - radius; y <= y0 + radius; ++y)
        {
            marchRay(map, x0, y0, x0 - radius, y);
            marchRay(map, x0, y0, x0 + radius, y);
        }

        // The origin tile is always visible.
        if (map.inBounds(x0, y0))
        {
            map.at(x0, y0).visible  = true;
            map.at(x0, y0).explored = true;
        }

        (void)x1; (void)y1;
    }
}
