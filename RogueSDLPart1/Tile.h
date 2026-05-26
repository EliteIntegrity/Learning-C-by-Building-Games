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
    bool    explored = false;   // ever seen by the player
    bool    visible  = false;   // visible THIS turn
};
