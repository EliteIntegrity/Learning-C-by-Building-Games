#pragma once
#include "Entity.h"

enum class MonsterKind
{
    Rat,
    Goblin,
    Orc
};

struct MonsterStats
{
    int  maxHp;
    int  damage;
    int  sightRange;
    char glyph;
};

// Lookup table; defined in Enemy.cpp.
const MonsterStats& statsFor(MonsterKind k);

class Enemy : public Entity
{
public:
    Enemy(Point start, MonsterKind kind);

    MonsterKind kind() const { return kind_; }

    int  hp()     const { return hp_; }
    int  damage() const { return stats_.damage; }
    bool alive()  const { return hp_ > 0; }

    void takeDamage(int amount) { hp_ -= amount; if (hp_ < 0) hp_ = 0; }

private:
    MonsterKind         kind_;
    const MonsterStats& stats_;
    int                 hp_;
};
