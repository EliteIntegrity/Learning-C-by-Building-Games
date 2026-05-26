#pragma once
#include "Entity.h"

enum class ItemKind
{
    Potion,
    Gold
};

class Item : public Entity
{
public:
    Item(Point start, ItemKind kind, int amount = 1);

    ItemKind kind()   const { return kind_; }
    int      amount() const { return amount_; }

private:
    ItemKind kind_;
    int      amount_;   // potion -> ignored; gold -> coin count
};
