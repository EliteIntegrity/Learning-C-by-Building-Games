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

void Player::resetForNewGame(Point start)
{
    pos_     = start;
    hp_      = 20;
    maxHp_   = 20;
    damage_  = 4;
    gold_    = 0;
    potions_ = 0;
}
