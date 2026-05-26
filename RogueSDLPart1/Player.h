#pragma once
#include "Entity.h"

class Map;

class Player : public Entity
{
public:
    explicit Player(Point start);

    // Attempt to step in (dx, dy). Returns true if the move
    // succeeded (a turn was used).
    bool tryMove(int dx, int dy, const Map& map);
};
