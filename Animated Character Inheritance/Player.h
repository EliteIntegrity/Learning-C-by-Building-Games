#pragma once
#include "Entity.h"

// ============================================================
//  Player  -  Chapter 19
// ============================================================
//
//  A Player IS-A Entity. It just picks its own starting size
//  and position (center of the screen) and lets the base
//  class handle everything else. No overrides needed.
// ============================================================

class Player : public Entity
{
public:
    Player(float screenWidth, float screenHeight);
};
