#include "Item.h"

namespace
{
    char glyphFor(ItemKind k)
    {
        switch (k)
        {
            case ItemKind::Potion: return '!';
            case ItemKind::Gold:   return '$';
        }
        return '?';
    }
    SDL_Color colorFor(ItemKind k)
    {
        switch (k)
        {
            case ItemKind::Potion: return Palette::ITEM_POTION;
            case ItemKind::Gold:   return Palette::ITEM_GOLD;
        }
        return Palette::ITEM_GOLD;
    }
}

Item::Item(Point start, ItemKind kind, int amount)
    : Entity(start, glyphFor(kind), colorFor(kind))
    , kind_(kind), amount_(amount)
{
}
