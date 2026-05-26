#include "Player.h"
#include "Map.h"

Player::Player(Point start)
    : Entity(start, '@', Palette::PLAYER)
{
}

bool Player::tryMove(int dx, int dy, const Map& map)
{
    int nx = pos_.x + dx;
    int ny = pos_.y + dy;
    if (map.isBlocked(nx, ny)) return false;
    pos_.x = nx;
    pos_.y = ny;
    return true;
}
