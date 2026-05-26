#include "Enemy.h"

namespace
{
    // Three monster types — stat lookup keyed by enum value.
    const MonsterStats kRatStats    { /*hp*/  4, /*dmg*/ 2, /*sight*/ 6, /*glyph*/ 'r' };
    const MonsterStats kGoblinStats { /*hp*/  8, /*dmg*/ 3, /*sight*/ 7, /*glyph*/ 'g' };
    const MonsterStats kOrcStats    { /*hp*/ 14, /*dmg*/ 5, /*sight*/ 8, /*glyph*/ 'o' };
}

const MonsterStats& statsFor(MonsterKind k)
{
    switch (k)
    {
        case MonsterKind::Rat:    return kRatStats;
        case MonsterKind::Goblin: return kGoblinStats;
        case MonsterKind::Orc:    return kOrcStats;
    }
    return kRatStats;
}

static SDL_Color colorFor(MonsterKind k)
{
    switch (k)
    {
        case MonsterKind::Rat:    return Palette::ENEMY_RAT;
        case MonsterKind::Goblin: return Palette::ENEMY_GOBLIN;
        case MonsterKind::Orc:    return Palette::ENEMY_ORC;
    }
    return Palette::ENEMY_RAT;
}

Enemy::Enemy(Point start, MonsterKind kind)
    : Entity(start, statsFor(kind).glyph, colorFor(kind))
    , kind_(kind)
    , stats_(statsFor(kind))
    , hp_(stats_.maxHp)
{
}
