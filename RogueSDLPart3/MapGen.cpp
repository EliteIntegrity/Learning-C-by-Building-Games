#include "MapGen.h"
#include "Map.h"
#include "Tile.h"
#include "Enemy.h"
#include "Item.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

static constexpr int MIN_LEAF  = 10;
static constexpr int ROOM_PAD  = 1;
static constexpr int MAX_DEPTH = 6;

MapGenerator::MapGenerator(unsigned seed) : seed_(seed)
{
    if (seed_ == 0) seed_ = (unsigned)time(nullptr);
    srand(seed_);
}

int MapGenerator::rng(int lo, int hi)
{
    if (hi <= lo) return lo;
    return lo + (rand() % (hi - lo + 1));
}

Point MapGenerator::generate(Map& map,
                             std::vector<std::unique_ptr<Enemy>>& enemies,
                             std::vector<std::unique_ptr<Item>>&  items,
                             int depth)
{
    map_ = &map;
    map_->fillWall();
    roomCenters_.clear();
    enemies.clear();
    items.clear();

    splitNode(0, 0, map_->width(), map_->height(), 0);

    for (size_t i = 1; i < roomCenters_.size(); ++i)
        carveCorridor(roomCenters_[i - 1], roomCenters_[i]);

    if (!roomCenters_.empty())
    {
        Point s = roomCenters_.back();
        map_->at(s.x, s.y).terrain = Terrain::StairsDown;
    }

    Point playerStart = roomCenters_.empty()
        ? Point{ map_->width() / 2, map_->height() / 2 }
        : roomCenters_.front();

    populate(enemies, items, playerStart, depth);
    return playerStart;
}

void MapGenerator::splitNode(int x, int y, int w, int h, int depth)
{
    if (depth >= MAX_DEPTH || (w < MIN_LEAF * 2 && h < MIN_LEAF * 2))
    {
        int rw = rng(4, std::max(4, w - ROOM_PAD * 2));
        int rh = rng(3, std::max(3, h - ROOM_PAD * 2));
        int rx = rng(x + ROOM_PAD, x + w - rw - ROOM_PAD);
        int ry = rng(y + ROOM_PAD, y + h - rh - ROOM_PAD);
        placeRoom(rx, ry, rw, rh);
        return;
    }

    bool splitHoriz;
    if      (w > h * 5 / 4) splitHoriz = false;
    else if (h > w * 5 / 4) splitHoriz = true;
    else                    splitHoriz = (rng(0, 1) == 0);

    if (splitHoriz)
    {
        int s = rng(MIN_LEAF, h - MIN_LEAF);
        splitNode(x, y,     w, s,     depth + 1);
        splitNode(x, y + s, w, h - s, depth + 1);
    }
    else
    {
        int s = rng(MIN_LEAF, w - MIN_LEAF);
        splitNode(x,     y, s,     h, depth + 1);
        splitNode(x + s, y, w - s, h, depth + 1);
    }
}

void MapGenerator::placeRoom(int rx, int ry, int rw, int rh)
{
    for (int y = ry; y < ry + rh; ++y)
        for (int x = rx; x < rx + rw; ++x)
            if (map_->inBounds(x, y))
                map_->at(x, y).terrain = Terrain::Floor;
    roomCenters_.push_back({ rx + rw / 2, ry + rh / 2 });
}

void MapGenerator::carveCorridor(Point a, Point b)
{
    bool horizFirst = (rng(0, 1) == 0);
    int sx = std::min(a.x, b.x), ex = std::max(a.x, b.x);
    int sy = std::min(a.y, b.y), ey = std::max(a.y, b.y);

    if (horizFirst)
    {
        for (int x = sx; x <= ex; ++x)
            if (map_->inBounds(x, a.y)) map_->at(x, a.y).terrain = Terrain::Floor;
        for (int y = sy; y <= ey; ++y)
            if (map_->inBounds(b.x, y)) map_->at(b.x, y).terrain = Terrain::Floor;
    }
    else
    {
        for (int y = sy; y <= ey; ++y)
            if (map_->inBounds(a.x, y)) map_->at(a.x, y).terrain = Terrain::Floor;
        for (int x = sx; x <= ex; ++x)
            if (map_->inBounds(x, b.y)) map_->at(x, b.y).terrain = Terrain::Floor;
    }
}

namespace
{
    Point findFloor(Map& map, Point avoid)
    {
        for (int tries = 0; tries < 500; ++tries)
        {
            int x = rand() % map.width();
            int y = rand() % map.height();
            if (map.isBlocked(x, y)) continue;
            if (map.at(x, y).terrain == Terrain::StairsDown) continue;
            Point p { x, y };
            if (p == avoid) continue;
            return p;
        }
        return avoid;
    }
}

void MapGenerator::populate(std::vector<std::unique_ptr<Enemy>>& enemies,
                            std::vector<std::unique_ptr<Item>>&  items,
                            Point playerStart, int depth)
{
    int totalEnemies = 3 + depth + rng(0, 2);

    for (int i = 0; i < totalEnemies; ++i)
    {
        Point p = findFloor(*map_, playerStart);
        if (p == playerStart) continue;

        int roll = rand() % 100;
        MonsterKind kind = MonsterKind::Rat;
        if (depth >= 2 && roll >= 60) kind = MonsterKind::Goblin;
        if (depth >= 4 && roll >= 85) kind = MonsterKind::Orc;

        enemies.push_back(std::make_unique<Enemy>(p, kind));
    }

    // Potions + gold (as before).
    int potionCount = 1 + rng(0, 2);
    int goldCount   = 2 + rng(0, 3);

    for (int i = 0; i < potionCount; ++i)
    {
        Point p = findFloor(*map_, playerStart);
        if (p == playerStart) continue;
        items.push_back(std::make_unique<Item>(p, ItemKind::Potion));
    }
    for (int i = 0; i < goldCount; ++i)
    {
        Point p = findFloor(*map_, playerStart);
        if (p == playerStart) continue;
        items.push_back(std::make_unique<Item>(p, ItemKind::Gold, rng(5, 15)));
    }

    // Scrolls — one of each kind roughly every couple of levels.
    if (rng(0, 1) == 0)
    {
        Point p = findFloor(*map_, playerStart);
        if (p != playerStart)
            items.push_back(std::make_unique<Item>(p, ItemKind::ScrollMagicMap));
    }
    if (rng(0, 1) == 0)
    {
        Point p = findFloor(*map_, playerStart);
        if (p != playerStart)
            items.push_back(std::make_unique<Item>(p, ItemKind::ScrollFireball));
    }

    // Weapons — chance scales gently with depth.
    int weaponRoll = rand() % 100;
    if (weaponRoll < 25 + depth * 5)
    {
        ItemKind w = ItemKind::WeaponDagger;
        if (depth >= 3 && rng(0, 1) == 0) w = ItemKind::WeaponSword;
        if (depth >= 5 && rng(0, 2) == 0) w = ItemKind::WeaponHammer;

        Point p = findFloor(*map_, playerStart);
        if (p != playerStart)
            items.push_back(std::make_unique<Item>(p, w));
    }
}
