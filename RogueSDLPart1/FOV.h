#pragma once
#include "Common.h"

class Map;

// ============================================================
//  FOV  -  field of view via Bresenham line-of-sight.
// ============================================================
//
//  For every tile within `radius` of `origin`, walk a straight
//  line from the origin to that tile. If the line is clear
//  (no walls in the way), mark the tile visible AND explored.
//
//  Simple, correct, easy to read. Not the most accurate
//  symmetric algorithm in the world — symmetric shadow-
//  casting is the gold standard — but plenty good enough for
//  the small maps we have, and the code fits on one screen.
// ============================================================

namespace FOV
{
    void compute(Map& map, Point origin, int radius);
}
