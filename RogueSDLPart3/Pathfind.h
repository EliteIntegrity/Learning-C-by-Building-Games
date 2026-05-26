#pragma once
#include <vector>
#include "Common.h"

class Map;

// ============================================================
//  Pathfind  -  A* on the dungeon grid.
// ============================================================
//
//  Returns a list of Points from `from` to `to` (inclusive of
//  the destination, exclusive of the start). Empty vector if
//  no path exists. Treats walls as blocking; treats other
//  entities as walkable (the caller decides what to do when
//  it bumps into one).
//
//  Manhattan heuristic — admissible for 4-connected grids.
// ============================================================

namespace Pathfind
{
    std::vector<Point> astar(const Map& map, Point from, Point to);
}
