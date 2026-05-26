#pragma once
#include <vector>
#include <memory>
#include "Common.h"

class Map;
class Enemy;
class Item;

class MapGenerator
{
public:
    explicit MapGenerator(unsigned seed = 0);

    Point generate(Map& map,
                   std::vector<std::unique_ptr<Enemy>>& enemies,
                   std::vector<std::unique_ptr<Item>>&  items,
                   int depth);

private:
    void splitNode(int x, int y, int w, int h, int depth);
    void placeRoom(int rx, int ry, int rw, int rh);
    void carveCorridor(Point a, Point b);

    void populate(std::vector<std::unique_ptr<Enemy>>& enemies,
                  std::vector<std::unique_ptr<Item>>&  items,
                  Point playerStart, int depth);

    int rng(int lo, int hi);

    Map*               map_ = nullptr;
    std::vector<Point> roomCenters_;
    unsigned           seed_;
};
