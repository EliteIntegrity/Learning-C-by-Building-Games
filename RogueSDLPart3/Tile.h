#pragma once

enum class Terrain
{
    Wall,
    Floor,
    StairsDown
};

struct Tile
{
    Terrain terrain  = Terrain::Wall;
    bool    explored = false;
    bool    visible  = false;
};
