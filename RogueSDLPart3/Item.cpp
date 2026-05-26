#include "Item.h"

const char* itemDisplayName(ItemKind k)
{
    switch (k)
    {
        case ItemKind::None:           return "(none)";
        case ItemKind::Potion:         return "Potion of Healing";
        case ItemKind::Gold:           return "Gold";
        case ItemKind::ScrollMagicMap: return "Scroll of Magic Mapping";
        case ItemKind::ScrollFireball: return "Scroll of Fireball";
        case ItemKind::WeaponDagger:   return "Dagger (+2)";
        case ItemKind::WeaponSword:    return "Sword (+5)";
        case ItemKind::WeaponHammer:   return "Warhammer (+8)";
    }
    return "?";
}

char itemGlyph(ItemKind k)
{
    switch (k)
    {
        case ItemKind::Potion:         return '!';
        case ItemKind::Gold:           return '$';
        case ItemKind::ScrollMagicMap:
        case ItemKind::ScrollFireball: return '?';
        case ItemKind::WeaponDagger:
        case ItemKind::WeaponSword:
        case ItemKind::WeaponHammer:   return ')';
        default:                       return '*';
    }
}

SDL_Color itemColor(ItemKind k)
{
    switch (k)
    {
        case ItemKind::Potion:         return Palette::ITEM_POTION;
        case ItemKind::Gold:           return Palette::ITEM_GOLD;
        case ItemKind::ScrollMagicMap:
        case ItemKind::ScrollFireball: return Palette::ITEM_SCROLL;
        case ItemKind::WeaponDagger:
        case ItemKind::WeaponSword:
        case ItemKind::WeaponHammer:   return Palette::ITEM_WEAPON;
        default:                       return Palette::ITEM_GOLD;
    }
}

bool isScroll(ItemKind k)
{
    return k == ItemKind::ScrollMagicMap || k == ItemKind::ScrollFireball;
}

bool isWeapon(ItemKind k)
{
    return k == ItemKind::WeaponDagger
        || k == ItemKind::WeaponSword
        || k == ItemKind::WeaponHammer;
}

int weaponDamageBonus(ItemKind k)
{
    switch (k)
    {
        case ItemKind::WeaponDagger: return 2;
        case ItemKind::WeaponSword:  return 5;
        case ItemKind::WeaponHammer: return 8;
        default:                     return 0;
    }
}

Item::Item(Point start, ItemKind kind, int amount)
    : Entity(start, itemGlyph(kind), itemColor(kind))
    , kind_(kind), amount_(amount)
{
}
