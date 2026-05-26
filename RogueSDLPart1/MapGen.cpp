#include "MapGen.h"
#include "Map.h"
#include "Tile.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

// --- Tunables -------------------------------------------------
// A leaf below this size becomes a single room (no further split).
static constexpr int MIN_LEAF = 10;
// Rooms shrink in from the leaf edge by this much on each side.
static constexpr int ROOM_PAD = 1;
// Hard cap on recursion depth in case rng goes wild.
static constexpr int MAX_DEPTH = 6;
// --------------------------------------------------------------

MapGenerator::MapGenerator(unsigned seed)
    : seed_(seed)
{
    if (seed_ == 0)
        seed_ = (unsigned)time(nullptr);
    srand(seed_);
}

int MapGenerator::rng(int lo, int hi)
{
    if (hi <= lo) return lo;
    return lo + (rand() % (hi - lo + 1));
}

Point MapGenerator::generate(Map& map)
{
    map_ = &map;
    map_->fillWall();
    roomCenters_.clear();

    splitNode(0, 0, map_->width(), map_->height(), 0);

    // Connect each new room to the previous one with a corridor.
    for (size_t i = 1; i < roomCenters_.size(); ++i)
        carveCorridor(roomCenters_[i - 1], roomCenters_[i]);

    // Stairs down in the last room.
    if (!roomCenters_.empty())
    {
        Point s = roomCenters_.back();
        map_->at(s.x, s.y).terrain = Terrain::StairsDown;
    }

    // Player starts in the first room.
    return roomCenters_.empty()
         ? Point{ map_->width() / 2, map_->height() / 2 }
         : roomCenters_.front();
}

void MapGenerator::splitNode(int x, int y, int w, int h, int depth)
{
    // Base case: small enough, or recursion depth hit. Place a room.
    if (depth >= MAX_DEPTH || (w < MIN_LEAF * 2 && h < MIN_LEAF * 2))
    {
        int rw = std::max(4, w - ROOM_PAD * 2);
        int rh = std::max(3, h - ROOM_PAD * 2);

        // Slightly randomize room size within the leaf.
        rw = rng(4, rw);
        rh = rng(3, rh);

        int rx = rng(x + ROOM_PAD, x + w - rw - ROOM_PAD);
        int ry = rng(y + ROOM_PAD, y + h - rh - ROOM_PAD);

        placeRoom(rx, ry, rw, rh);
        return;
    }

    // Choose split axis: prefer the longer dimension.
    bool splitHoriz;
    if (w > h * 5 / 4)       splitHoriz = false;   // wider -> vertical cut
    else if (h > w * 5 / 4)  splitHoriz = true;    // taller -> horizontal cut
    else                     splitHoriz = (rng(0, 1) == 0);

    if (splitHoriz)
    {
        int splitAt = rng(MIN_LEAF, h - MIN_LEAF);
        splitNode(x, y,               w, splitAt,         depth + 1);
        splitNode(x, y + splitAt,     w, h - splitAt,     depth + 1);
    }
    else
    {
        int splitAt = rng(MIN_LEAF, w - MIN_LEAF);
        splitNode(x,            y, splitAt,         h, depth + 1);
        splitNode(x + splitAt,  y, w - splitAt,    h, depth + 1);
    }
}

void MapGenerator::placeRoom(int rx, int ry, int rw, int rh)
{
    for (int y = ry; y < ry + rh; ++y)
    {
        for (int x = rx; x < rx + rw; ++x)
        {
            if (map_->inBounds(x, y))
                map_->at(x, y).terrain = Terrain::Floor;
        }
    }
    roomCenters_.push_back({ rx + rw / 2, ry + rh / 2 });
}

void MapGenerator::carveCorridor(Point a, Point b)
{
    // L-shape: horizontal first then vertical (50/50 with the reverse).
    bool horizFirst = (rng(0, 1) == 0);

    int sx = std::min(a.x, b.x);
    int ex = std::max(a.x, b.x);
    int sy = std::min(a.y, b.y);
    int ey = std::max(a.y, b.y);

    if (horizFirst)
    {
        for (int x = sx; x <= ex; ++x)
            if (map_->inBounds(x, a.y))
                map_->at(x, a.y).terrain = Terrain::Floor;
        for (int y = sy; y <= ey; ++y)
            if (map_->inBounds(b.x, y))
                map_->at(b.x, y).terrain = Terrain::Floor;
    }
    else
    {
        for (int y = sy; y <= ey; ++y)
            if (map_->inBounds(a.x, y))
                map_->at(a.x, y).terrain = Terrain::Floor;
        for (int x = sx; x <= ex; ++x)
            if (map_->inBounds(x, b.y))
                map_->at(x, b.y).terrain = Terrain::Floor;
    }
}
