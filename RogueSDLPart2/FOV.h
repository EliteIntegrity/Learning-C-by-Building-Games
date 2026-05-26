#pragma once
#include "Common.h"

class Map;

namespace FOV
{
    void compute(Map& map, Point origin, int radius);

    // Quick line-of-sight check: can `from` see `to`? Doesn't change
    // tile flags. Used by enemy AI to decide whether to chase.
    bool hasLineOfSight(const Map& map, Point from, Point to);
}
