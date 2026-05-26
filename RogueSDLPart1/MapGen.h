#pragma once
#include <vector>
#include "Common.h"

class Map;

// ============================================================
//  MapGenerator  -  BSP-based dungeon generator.
// ============================================================
//
//  Recursively splits the map area into smaller and smaller
//  rectangles. Each leaf rectangle gets a room. Adjacent
//  rooms (in generation order) get connected by L-shaped
//  corridors. The last room placed receives the stairs down.
// ============================================================

class MapGenerator
{
public:
    explicit MapGenerator(unsigned seed = 0);

    // Generate a fresh dungeon into `map`. Returns the
    // recommended player start tile (center of the first room).
    Point generate(Map& map);

private:
    // Recursive split.
    void splitNode(int x, int y, int w, int h, int depth);

    // Carve out a room at (rx, ry) of size (rw x rh). Remembers
    // the room's center for corridor connection.
    void placeRoom(int rx, int ry, int rw, int rh);

    // Carve an L-shaped corridor from `a` to `b`.
    void carveCorridor(Point a, Point b);

    int rng(int lo, int hi);   // inclusive on both ends

    Map*               map_ = nullptr;
    std::vector<Point> roomCenters_;
    unsigned           seed_;
};
