#include "Player.h"

Player::Player(float screenWidth, float screenHeight)
    : Entity( (screenWidth  - 120.0f) * 0.5f,
              (screenHeight - 120.0f) * 0.5f,
              120.0f, 120.0f )
{
    // No body — everything is set up by the base constructor.
}
