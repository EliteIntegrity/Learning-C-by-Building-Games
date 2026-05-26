#pragma once
#include "Entity.h"

// ============================================================
//  Item / ItemKind
// ============================================================
//
//  Now spans pickups (potion, gold), consumable scrolls, and
//  wieldable weapons. The kind is a single enum; per-kind
//  data (display name, glyph, color, weapon bonus) lives in
//  small free helper functions you can find in Item.cpp.
// ============================================================

enum class ItemKind
{
    None = 0,            // sentinel — used for "no weapon wielded"

    Potion,
    Gold,

    ScrollMagicMap,
    ScrollFireball,

    WeaponDagger,
    WeaponSword,
    WeaponHammer,
};

const char* itemDisplayName(ItemKind k);
char        itemGlyph(ItemKind k);
SDL_Color   itemColor(ItemKind k);

bool isScroll(ItemKind k);
bool isWeapon(ItemKind k);

// 0 for non-weapons.
int  weaponDamageBonus(ItemKind k);

class Item : public Entity
{
public:
    Item(Point start, ItemKind kind, int amount = 1);

    ItemKind kind()   const { return kind_; }
    int      amount() const { return amount_; }

private:
    ItemKind kind_;
    int      amount_;
};
