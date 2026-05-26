#pragma once
#include "Entity.h"

// Player IS-A Entity. No overrides — uses the Animator's
// rainbow tint and the base's update logic. Centered.
class Player : public Entity
{
public:
    Player(float screenWidth, float screenHeight);
};
